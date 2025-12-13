// AFInGameWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AFInGameWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class AFO_API UAFInGameWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:

	// [GameTimer]
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GameTimer;

protected:
	virtual void NativeConstruct() override;

	// 델리게이트에 바인딩 될 함수
	UFUNCTION()
	void UpdateGameTimerText(int32 NewTime);

};
