#include "Player/AFTitlePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"

void AAFTitlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	if (IsValid(UIWidgetClass) == true)
	{
		UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);
		if (IsValid(UIWidgetInstance) == true)
		{
			UIWidgetInstance->AddToViewport();

			FInputModeUIOnly Mode;
			Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
			SetInputMode(Mode);

			bShowMouseCursor = true;
		}
	}

	if (IsValid(TitleBGM) && TitleBGMComponent == nullptr)
	{
		TitleBGMComponent = UGameplayStatics::SpawnSound2D(this, TitleBGM, 1.0f, 1.0f, 0.0f);
		if (TitleBGMComponent)
		{
			TitleBGMComponent->bIsUISound = true; // 선택
		}
	}
}

void AAFTitlePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TitleBGMComponent)
	{
		TitleBGMComponent->Stop();
		TitleBGMComponent = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void AAFTitlePlayerController::JoinServer(const FString& InIPAddress, const FString& InPlayerName)
{
	const FString Trimmed = InIPAddress.TrimStartAndEnd();
	const FString TrimmedName = InPlayerName.TrimStartAndEnd().IsEmpty() ? TEXT("DefaultPlayer") : InPlayerName;

	FInputModeGameOnly GameMode;
	SetInputMode(GameMode);
	bShowMouseCursor = false;

	if (IsValid(UIWidgetInstance))
	{
		UIWidgetInstance->RemoveFromParent();
		UIWidgetInstance = nullptr;
	}

	if (TitleBGMComponent)
	{
		TitleBGMComponent->Stop();
		TitleBGMComponent = nullptr;
	}

	// 1. 로컬에서 방을 직접 팔 때 (Listen Server)
	if (Trimmed.IsEmpty())
	{
		const FString MapURL = TEXT("/Game/01_ArenaFighter/01_Levels/AFOBattleZone");
		const FString Options = FString::Printf(TEXT("listen?Name=%s"), *TrimmedName);

		UGameplayStatics::OpenLevel(GetWorld(), FName(*MapURL), true, Options);
		return;
	}

	// 2. 다른 서버에 접속할 때 (Client Travel)
	const FString Address = FString::Printf(TEXT("%s:7777?Name=%s"), *Trimmed, *TrimmedName);

	UE_LOG(LogTemp, Log, TEXT("Traveling to: %s"), *Address);
	ClientTravel(Address, TRAVEL_Absolute);
}

//void AAFTitlePlayerController::JoinServer()
//{
//	FName NextLevelName = FName(TEXT("AFOBattleZone"));
//	UGameplayStatics::OpenLevel(GetWorld(), NextLevelName, true);
//
//	FInputModeGameOnly GameMode;
//	SetInputMode(GameMode);
//
//	bShowMouseCursor = false;
// }

//void AAFTitlePlayerController::JoinServer(const FString& InIPAddress)
//{
//	ClientTravel("127.0.0.1", TRAVEL_Absolute);
//}