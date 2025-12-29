// AFSkillSlotWidget.cpp


#include "UI/AFSkillSlotWidget.h"
#include "Components/Image.h"

void UAFSkillSlotWidget::SetSkillSlotInfo(FAFSkillInfo NewSkillInfo)
{
    MySkillData = NewSkillInfo;

    // 1. 이미지 변수가 유효한지 확인 (UPROPERTY(meta = (BindWidget))로 연결된 변수)
    if (SkillIcon)
    {
        // 2. 데이터 테이블에서 로드된 Texture가 있는지 확인
        if (MySkillData.SkillIcon)
        {
            SkillIcon->SetBrushFromTexture(MySkillData.SkillIcon);
            UE_LOG(LogTemp, Log, TEXT("아이콘 설정 성공: %s"), *MySkillData.SkillName.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("%s 의 아이콘 데이터가 비어있습니다!"), *MySkillData.SkillName.ToString());
        }
    }
}
