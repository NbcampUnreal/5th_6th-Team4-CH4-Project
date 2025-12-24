#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_TeamSelect.generated.h"

class UButton;
class UTextBlock;
class UVerticalBox;

UCLASS()
class AFO_API UUW_TeamSelect : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnClickToRed();

	UFUNCTION()
	void OnClickToBlue();

	UFUNCTION()
	void OnClickNext();

	UFUNCTION()
	void RefreshUI();

	// [추가] 팀 리스트 문자열 만들기(람다 제거용)
	FString BuildTeamList(uint8 TeamId) const;

	// [유지] 팀 패널 갱신(여기서만 Red/Blue Count/List를 세팅하도록 통일)
	void RebuildTeamLists();

	// [선택] 디버그용 전체 리스트(원하면 남기고, 싫으면 RefreshUI에서 호출 제거)
	void RebuildPlayerList();

	// [유지] GameState 바인딩 재시도
	void TryBindGameState();

private:
	FTimerHandle BindRetryHandle;
	bool bBound = false;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BtnToRed;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BtnToBlue;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BtnNext;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TxtCounts;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> VBPlayerList;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_RedCount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_RedList;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_BlueCount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_BlueList;
};
