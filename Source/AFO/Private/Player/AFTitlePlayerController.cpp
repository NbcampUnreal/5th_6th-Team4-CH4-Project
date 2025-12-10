#include "Player/AFTitlePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

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
}

//void AAFTitlePlayerController::JoinServer(const FString& InIPAddress)
//{
//	FName NextLevelName = FName(*InIPAddress);
//	UGameplayStatics::OpenLevel(GetWorld(), NextLevelName, true);
//}

void AAFTitlePlayerController::JoinServer(const FString& InIPAddress)
{
	FName NextLevelName = FName(TEXT("AFOBattleZone"));
	UGameplayStatics::OpenLevel(GetWorld(), NextLevelName, true);
 }

//void AAFTitlePlayerController::JoinServer(const FString& InIPAddress)
//{
//	ClientTravel("127.0.0.1", TRAVEL_Absolute);
//}