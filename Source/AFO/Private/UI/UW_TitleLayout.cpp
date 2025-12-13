#include "UI/UW_TitleLayout.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/AFTitlePlayerController.h"

UUW_TitleLayout::UUW_TitleLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UUW_TitleLayout::NativeConstruct()
{
	Super::NativeConstruct();

	if (PlayButton)
	{
		PlayButton->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
	}

	if (OptionButton)
	{
		OptionButton->OnClicked.AddDynamic(this, &ThisClass::OnOptionButtonClicked);
	}
}

//void UUW_TitleLayout::OnPlayButtonClicked()
//{
//	AAFTitlePlayerController* PlayerController = GetOwningPlayer<AAFTitlePlayerController>();
//	if (IsValid(PlayerController) == true)
//	{
//		// FText ServerIP = ServerIPEditableText->GetText();
//		PlayerController->JoinServer();
//	}
//}

void UUW_TitleLayout::OnPlayButtonClicked()
{
	AAFTitlePlayerController* PC = GetOwningPlayer<AAFTitlePlayerController>();
	if (!IsValid(PC)) return;

	FString IP;
	if (ServerIPEditableText)
	{
		IP = ServerIPEditableText->GetText().ToString().TrimStartAndEnd();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ServerIPEditableText is null. Check BindWidget name."));
	}

	PC->JoinServer(IP);
}


void UUW_TitleLayout::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UUW_TitleLayout::OnOptionButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Option Button Clicked!"));
}