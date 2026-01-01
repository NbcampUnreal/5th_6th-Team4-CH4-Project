#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AFScoreboardRowWidget.generated.h"

class UTextBlock;
class AAFPlayerState;

UCLASS()
class AFO_API UAFScoreboardRowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(AAFPlayerState* InPS);

protected:
	virtual void NativeDestruct() override;

private:
	void RefreshTexts();

	UFUNCTION()
	void OnPlayerInfoChanged(AAFPlayerState* PS);

	UFUNCTION()
	void OnScoreChanged(int32 Kills, int32 Deaths, AAFPlayerState* PS);

private:
	UPROPERTY(Transient)
	TObjectPtr<AAFPlayerState> PlayerStateRef;

private:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Scoreboard", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> Text_PlayerName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Scoreboard", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> Text_CharacterName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Scoreboard", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> Text_Kills;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Scoreboard", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> Text_Deaths;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Scoreboard", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> Text_DamageDealt;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Scoreboard", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> Text_DamageTaken;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Scoreboard", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> Text_Healing;
};
