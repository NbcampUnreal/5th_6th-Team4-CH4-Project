// AFSkillSlotWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/AFGameTypes.h"
#include "AFSkillSlotWidget.generated.h"


UCLASS()
class AFO_API UAFSkillSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 슬롯에 데이터를 채우는 핵심 함수
	UFUNCTION(BlueprintCallable, Category = "AF | UI")
	void SetSkillSlotInfo(FAFSkillInfo NewSkillInfo);

protected:
	// 블루프린트 위젯에 있는 Image 이름을 'SkillIconImage'로 지으면 자동으로 연결됨
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> SkillIcon;

	// 현재 슬롯이 들고 있는 데이터 (나중에 툴팁에 전달용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AF | Skill")
	FAFSkillInfo MySkillData;
};
