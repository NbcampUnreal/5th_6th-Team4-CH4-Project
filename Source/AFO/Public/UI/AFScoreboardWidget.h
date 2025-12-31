#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Game/AFGameState.h"
#include "AFScoreboardWidget.generated.h"

class UScrollBox;
class UTextBlock;
class UAFScoreboardRowWidget;

UCLASS()
class AFO_API UAFScoreboardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// PC에서 호출
	UFUNCTION()
	void SetMatchResult(const FAFMatchResult& InResult);

	UFUNCTION()
	void RefreshPlayerList();

	UFUNCTION()
	void RefreshTotals();

protected:
	virtual void NativeConstruct() override;

private:
	void RefreshResultText();

private:
	FAFMatchResult CachedResult;

	UPROPERTY(EditDefaultsOnly, Category = "Scoreboard")
	TSubclassOf<UAFScoreboardRowWidget> RowWidgetClass;

private:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Scoreboard", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UScrollBox> SB_Red;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Scoreboard", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UScrollBox> SB_Blue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Scoreboard", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> TB_RedTotalKills;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Scoreboard", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> TB_BlueTotalKills;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Scoreboard", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> TB_Result;
};
