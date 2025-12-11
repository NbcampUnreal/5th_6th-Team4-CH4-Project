// AFInGameWidget.cpp


#include "UI/AFInGameWidget.h"
#include "Components/TextBlock.h"
#include "Game/AFGameState.h"
#include "Kismet/GameplayStatics.h"

void UAFInGameWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();
	
	if (GameTimer)
	{
		GameTimer->SetText(FText::FromString("Wait..."));
	}

	// GameState를 찾아서 델리게이트 구독
	AAFGameState* GameState = Cast<AAFGameState>(GetWorld()->GetGameState());
	if (IsValid(GameState))
	{
		// 1. 이미 진행 중인 시간이 있을 수 있으므로 현재 시간으로 즉시 업데이트
		UpdateGameTimerText(GameState->RemainingTimeSeconds); 

		// 2. 앞으로 변할 때마다 호출되도록 바인딩
		GameState->OnTimerChanged.AddDynamic(this, &UAFInGameWidget::UpdateGameTimerText);
	}
}

void UAFInGameWidget::UpdateGameTimerText(int32 NewTime)
{
	if (!IsValid(GameTimer)) return;

	// "MM:SS" 포맷으로 변환
	int32 Minutes = NewTime / 60;
	int32 Seconds = NewTime % 60;

	FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	GameTimer->SetText(FText::FromString(TimeString));

	// 30초 미만일 때 빨간색으로 변경하는 연출
	if (NewTime <= 30)
	{
		GameTimer->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
	}
}

