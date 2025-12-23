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
	ClearCurrentUI();

	const FString MapName = UGameplayStatics::GetCurrentLevelName(this, true);

	if (MapName == TEXT("AFOTeamSelect"))
	{
		if (TeamSelectWidgetClass)
		{
			CurrentWidget = CreateWidget<UUserWidget>(this, TeamSelectWidgetClass);
			if (CurrentWidget)
			{
				CurrentWidget->AddToViewport();
				SetUIInputMode(true);
			}
		}
	}
	else if (MapName == TEXT("AFOCharacterSelectMap"))
	{
		if (CharacterSelectWidgetClass)
		{
			CurrentWidget = CreateWidget<UUserWidget>(this, CharacterSelectWidgetClass);
			if (CurrentWidget)
			{
				CurrentWidget->AddToViewport();
				SetUIInputMode(true);
			}
		}
	}
	else
	{
		// 배틀존 등
		SetUIInputMode(false);
	}
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