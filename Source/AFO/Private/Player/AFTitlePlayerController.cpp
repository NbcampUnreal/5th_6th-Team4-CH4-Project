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
			TitleBGMComponent->bIsUISound = true; // ¼±ÅÃ
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

//void AAFTitlePlayerController::JoinServer(const FString& InIPAddress)
//{
//	FName NextLevelName = FName(*InIPAddress);
//	UGameplayStatics::OpenLevel(GetWorld(), NextLevelName, true);
//}

void AAFTitlePlayerController::JoinServer()
{
	FName NextLevelName = FName(TEXT("AFOBattleZone"));
	UGameplayStatics::OpenLevel(GetWorld(), NextLevelName, true);

	FInputModeGameOnly GameMode;
	SetInputMode(GameMode);

	bShowMouseCursor = false;
 }

//void AAFTitlePlayerController::JoinServer(const FString& InIPAddress)
//{
//	ClientTravel("127.0.0.1", TRAVEL_Absolute);
//}