// AFSkillSlotWidget.cpp


#include "UI/AFSkillSlotWidget.h"
#include "Components/Image.h"

void UAFSkillSlotWidget::SetSkillSlotInfo(FAFSkillInfo NewSkillInfo)
{
    // 함수가 호출되는지부터 확인
    UE_LOG(LogTemp, Warning, TEXT("@@@ SetSkillSlotInfo Called! Skill: %s"), *NewSkillInfo.SkillName.ToString());

    MySkillData = NewSkillInfo;

    if (SkillIcon)
    {
        if (MySkillData.SkillIcon)
        {
            SkillIcon->SetBrushFromTexture(MySkillData.SkillIcon);
            UE_LOG(LogTemp, Log, TEXT("아이콘 설정 성공: %s"), *MySkillData.SkillName.ToString());
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("Texture Data is NULL for: %s"), *NewSkillInfo.SkillName.ToString());
        }
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("SkillIcon Widget is NOT Bounded!"));
    }
}
