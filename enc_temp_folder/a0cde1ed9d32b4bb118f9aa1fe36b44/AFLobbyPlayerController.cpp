#include "Controller/AFLobbyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Game/AFTeamSelectGameMode.h"
#include "Game/AFCharacterSelectGameMode.h"

void AAFLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;

	SetupUIForCurrentMap();
}

void AAFLobbyPlayerController::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	if (!IsLocalController()) return;

	SetupUIForCurrentMap();
}

void AAFLobbyPlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();

	if (!IsLocalController()) return;

	UE_LOG(LogTemp, Warning, TEXT("[UI] BeginPlayingState -> SetupUI"));
	SetupUIForCurrentMap();
}

void AAFLobbyPlayerController::ClearCurrentUI()
{
	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}
}

void AAFLobbyPlayerController::SetUIInputMode(bool bUIOnly)
{
	if (bUIOnly)
	{
		FInputModeUIOnly Mode;
		if (CurrentWidget)
		{
			Mode.SetWidgetToFocus(CurrentWidget->TakeWidget());
		}
		SetInputMode(Mode);
		bShowMouseCursor = true;
	}
	else
	{
		FInputModeGameOnly Mode;
		SetInputMode(Mode);
		bShowMouseCursor = false;
	}
}

void AAFLobbyPlayerController::SetupUIForCurrentMap()
{
	const FString MapName = UGameplayStatics::GetCurrentLevelName(this, true);

	UE_LOG(LogTemp, Warning, TEXT("[UI] SetupUI Map=%s Local=%d PC=%s TeamWBP=%s CharWBP=%s Cur=%s"),
		*MapName,
		IsLocalController() ? 1 : 0,
		*GetClass()->GetName(),
		TeamSelectWidgetClass ? TEXT("OK") : TEXT("NULL"),
		CharacterSelectWidgetClass ? TEXT("OK") : TEXT("NULL"),
		CurrentWidget ? TEXT("EXIST") : TEXT("NONE"));

	TSubclassOf<UUserWidget> DesiredClass = nullptr;

	if (MapName == TEXT("AFOTeamSelect"))
	{
		DesiredClass = TeamSelectWidgetClass;
	}
	else if (MapName == TEXT("AFOCharacterSelectMap"))
	{
		DesiredClass = CharacterSelectWidgetClass;
	}

	if (!DesiredClass)
	{
		ClearCurrentUI();
		SetUIInputMode(false);
		return;
	}

	if (CurrentWidget && CurrentWidget->IsA(DesiredClass))
	{
		SetUIInputMode(true);
		return;
	}

	ClearCurrentUI();

	if (!DesiredClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[UI] DesiredClass is NULL on Map=%s"), *MapName);
		return;
	}

	CurrentWidget = CreateWidget<UUserWidget>(this, DesiredClass);
	if (!CurrentWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[UI] CreateWidget FAILED Map=%s Class=%s"),
			*MapName, *DesiredClass->GetName());
		return;
	}

	CurrentWidget->AddToViewport();
	SetUIInputMode(true);

	UE_LOG(LogTemp, Warning, TEXT("[UI] Widget Added: %s"), *DesiredClass->GetName());
}


void AAFLobbyPlayerController::ServerRequestSetTeam_Implementation(uint8 NewTeamId)
{
	if (!GetWorld()) return;

	AGameModeBase* GMBase = GetWorld()->GetAuthGameMode();
	if (!GMBase) return;

	AAFTeamSelectGameMode* GM = Cast<AAFTeamSelectGameMode>(GMBase);
	if (!GM) return;

	if (!GM->RequestSetTeam(this, NewTeamId))
	{
		ClientShowMessage(TEXT("팀당 최대 2명만 가능합니다."));
	}
}

void AAFLobbyPlayerController::ServerRequestAdvanceToCharacterSelect_Implementation()
{
	if (AAFTeamSelectGameMode* GM = GetWorld()->GetAuthGameMode<AAFTeamSelectGameMode>())
	{
		if (!GM->AdvanceToCharacterSelect())
		{
			ClientShowMessage(TEXT("조건 미달: 4명 접속 + RED2/BLUE2가 되어야 합니다."));
		}
	}
}

void AAFLobbyPlayerController::ServerRequestSelectCharacter_Implementation(uint8 CharacterId)
{
	if (AAFCharacterSelectGameMode* GM = GetWorld()->GetAuthGameMode<AAFCharacterSelectGameMode>())
	{
		if (!GM->RequestSelectCharacter(this, CharacterId))
		{
			ClientShowMessage(TEXT("같은 팀 내 캐릭터 중복은 불가합니다."));
		}
	}
}

void AAFLobbyPlayerController::ServerRequestSetReady_Implementation(bool bNewReady)
{
	if (AAFCharacterSelectGameMode* GM = GetWorld()->GetAuthGameMode<AAFCharacterSelectGameMode>())
	{
		if (!GM->RequestSetReady(this, bNewReady))
		{
			ClientShowMessage(TEXT("캐릭터 선택 후에만 Ready 할 수 있습니다."));
		}
	}
}

void AAFLobbyPlayerController::ClientShowMessage_Implementation(const FString& Msg)
{
	ClientMessage(Msg);
}