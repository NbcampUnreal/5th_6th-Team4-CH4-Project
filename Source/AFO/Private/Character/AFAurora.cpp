#include "Character/AFAurora.h"
#include "GameFramework/Controller.h"
#include "Components/AFSkillComponent.h"
#include "Components/AFStatusEffectComponent.h"

AAFAurora::AAFAurora()
{
	CharacterKey = TEXT("Aurora");
	bCanSprint = true;
}

void AAFAurora::HandleOnCheckHit()
{
    if (!HasAuthority() || IsDead() || !SkillComp) return;

    FName TargetKey = NAME_None;
    FAFSkillInfo* TargetData = nullptr;
    TArray<AActor*> HitVictims;
    bool bShouldSlow = false;

    // 1. 현재 어떤 공격 중인지 판별 및 데이터 확보
    if (bIsHeavyAttacking)
    {
        TargetKey = CachedHeavyAttackKey;
        TargetData = CachedHeavyAttackData;
        bShouldSlow = true; 
    }
    else if (bIsUsingSkill)
    {
        if (AnimInstance->Montage_IsPlaying(SkillEMontage))
        {
            TargetKey = CachedSkillEKey;
            TargetData = CachedSkillEData;
            bShouldSlow = true; 
        }
        else if (AnimInstance->Montage_IsPlaying(SkillQMontage))
        {
            TargetKey = CachedSkillQKey;
            TargetData = CachedSkillQData;
            bShouldSlow = true; 
        }
    }
    else if (bIsAttacking) // 일반 공격 추가
    {
        TargetKey = CachedAttackKey;
        TargetData = CachedAttackData;
    }

    if (TargetData == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("@@@ [Mage] TargetData is NULL! CharacterKey: %s, IsAttacking: %d"), *CharacterKey, bIsAttacking);
        return; // 데이터가 없으면 아래 로직을 수행하지 않고 나감
    }

    // 2. 판정 실행 (스킬/강공격인 경우)
    if (TargetData)
    {
        HitVictims = HandleSkillHitCheck(TargetData->Radius, TargetData->Damage, TargetData->ForwardOffset, TargetData->RotationOffset);

        // 쿨타임 시작
        SkillComp->StartCooldown(TargetKey, TargetData->Cooldown);
    }
    // 3. 일반 공격인 경우
    else if (bIsAttacking)
    {
        HandleSkillHitCheck(TargetData->Radius, TargetData->Damage, TargetData->ForwardOffset, TargetData->RotationOffset);
        bShouldSlow = false;
    }

    // 4. 슬로우 적용 
    // 위에서 판정된 HitVictims 리스트를 순회하며 슬로우 적용
    if (bShouldSlow && HitVictims.Num() > 0)
    {
        for (AActor* Victim : HitVictims)
        {
            if (UAFStatusEffectComponent* Status = Victim->FindComponentByClass<UAFStatusEffectComponent>())
            {
                Status->ApplySlow(SlowAmount, SlowDuration);
            }
        }
    }
}