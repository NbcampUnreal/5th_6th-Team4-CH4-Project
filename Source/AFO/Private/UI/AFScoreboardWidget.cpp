#include "UI/AFScoreboardWidget.h"
#include "UI/AFScoreboardRowWidget.h"
#include "Game/AFGameState.h"
#include "Player/AFPlayerState.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

void UAFScoreboardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Warning, TEXT("[Scoreboard] Construct SB_Red=%s SB_Blue=%s"),
		SB_Red ? TEXT("OK") : TEXT("NULL"),
		SB_Blue ? TEXT("OK") : TEXT("NULL"));

	RefreshPlayerList();
	RefreshTotals();
	RefreshResultText();
}

void UAFScoreboardWidget::SetMatchResult(const FAFMatchResult& InResult)
{
	CachedResult = InResult;

	RefreshResultText();
	RefreshTotals();
}

void UAFScoreboardWidget::RefreshResultText()
{
	if (!TB_Result) return;

	FString ResultStr = TEXT("RESULT");

	switch (CachedResult.WinnerTeam)
	{
	case EAFTeamId::Red:  ResultStr = TEXT("RED WIN");  break;
	case EAFTeamId::Blue: ResultStr = TEXT("BLUE WIN"); break;
	default:              ResultStr = TEXT("DRAW");     break;
	}

	TB_Result->SetText(FText::FromString(ResultStr));
}

void UAFScoreboardWidget::RefreshTotals()
{
	if (!TB_RedTotalKills || !TB_BlueTotalKills) return;

	const int32 RedKills = CachedResult.RedKills;
	const int32 BlueKills = CachedResult.BlueKills;

	TB_RedTotalKills->SetText(FText::FromString(
		FString::Printf(TEXT("RED : %d"), RedKills)
	));

	TB_BlueTotalKills->SetText(FText::FromString(
		FString::Printf(TEXT("BLUE : %d"), BlueKills)
	));
}

void UAFScoreboardWidget::RefreshPlayerList()
{
	if (!SB_Red || !SB_Blue) return;

	SB_Red->ClearChildren();
	SB_Blue->ClearChildren();

	AGameStateBase* GS = GetWorld() ? GetWorld()->GetGameState() : nullptr;
	if (!GS) return;

	if (!RowWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[Scoreboard] RowWidgetClass is NULL."));
		return;
	}

	for (APlayerState* PS : GS->PlayerArray)
	{
		AAFPlayerState* AFPS = Cast<AAFPlayerState>(PS);
		if (!AFPS) continue;

		UAFScoreboardRowWidget* Row = CreateWidget<UAFScoreboardRowWidget>(GetOwningPlayer(), RowWidgetClass);
		if (!Row) continue;

		Row->Init(AFPS);

		if (AFPS->GetTeamID() == 0)
		{
			SB_Red->AddChild(Row);
		}
		else
		{
			SB_Blue->AddChild(Row);
		}
	}
}