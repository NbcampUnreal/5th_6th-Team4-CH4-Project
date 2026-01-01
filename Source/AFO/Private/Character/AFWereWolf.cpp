// AFWereWolf.cpp

#include "Character/AFWereWolf.h"
#include "GameFramework/CharacterMovementComponent.h"
<<<<<<< Updated upstream

AAFWereWolf::AAFWereWolf()
{
	bCanSprint = false;
=======
#include "TimerManager.h"
#include "Components/AFAttributeComponent.h"
#include "Components/AFStatusEffectComponent.h"
#include "Components/AFSkillComponent.h"

AAFWereWolf::AAFWereWolf()
{
	CharacterKey = TEXT("WereWolf");
>>>>>>> Stashed changes
}

void AAFWereWolf::BeginPlay()
{
	Super::BeginPlay();
<<<<<<< Updated upstream
	
	NormalSpeed = WereWolfMoveSpeed;

	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
=======
}

void AAFWereWolf::ServerRPC_SkillE_Implementation()
{
	Super::ServerRPC_SkillE_Implementation();

	if (!HasAuthority()) return;

	bIsBleedModeActive = true;

	// [비주얼] 내 몸을 빨갛게 (StatusEffectComp의 OverlayMaterial 활용)
	if (UAFStatusEffectComponent* MyStatus = FindComponentByClass<UAFStatusEffectComponent>())
	{
		// 서버에서 변수만 바꾸면 OnRep으로 클라에서 빨갛게 보임 (오로라와 동일 구조)
		MyStatus->SetBleedVisual(true);
>>>>>>> Stashed changes
	}

	// 10초 동안 출혈 모드 유지
	GetWorldTimerManager().SetTimer(BleedModeTimerHandle, this, &AAFWereWolf::EndBleedMode, 10.0f, false);
}
<<<<<<< Updated upstream
=======

void AAFWereWolf::ServerRPC_SkillQ_Implementation()
{
	AAFPlayerCharacter::ServerRPC_SkillQ_Implementation();

	if (!HasAuthority() || !GetCharacterMovement()) return;

	// 1.5배 가속
	GetCharacterMovement()->MaxWalkSpeed *= SpeedBoostMutifly;

	// 15초 후 원복
	GetWorldTimerManager().SetTimer(SpeedBoostTimerHandle, this, &AAFWereWolf::ResetSpeedBoost, SpeedBoostDuration, false);
}

void AAFWereWolf::EndBleedMode()
{
    bIsBleedModeActive = false;
}

void AAFWereWolf::ResetSpeedBoost()
{
	GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
}

void AAFWereWolf::HandleOnCheckHit()
{
    if (!HasAuthority() || IsDead() || !SkillComp) return;

    FName TargetKey = NAME_None;
    FAFSkillInfo* TargetData = nullptr;
    TArray<AActor*> HitVictims;

    // 1. 데이터 확보
    if (bIsHeavyAttacking)
    {
        TargetKey = CachedHeavyAttackKey;
        TargetData = CachedHeavyAttackData;
    }
    else if (bIsUsingSkill)
    {
        if (AnimInstance->Montage_IsPlaying(SkillEMontage))
        {
            TargetKey = CachedSkillEKey;
            TargetData = CachedSkillEData;
        }
        else if (AnimInstance->Montage_IsPlaying(SkillQMontage))
        {
            TargetKey = CachedSkillQKey;
            TargetData = CachedSkillQData;
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
    // 2. 판정 실행 (강공격 또는 스킬)
    if (TargetData)
    {
        HitVictims = HandleSkillHitCheck(TargetData->Radius, TargetData->Damage, TargetData->ForwardOffset, TargetData->RotationOffset);

        // 스킬/강공격은 사용 시 쿨타임 시작
        SkillComp->StartCooldown(TargetKey, TargetData->Cooldown);
    }
    // 3. 일반 공격 판정
    else if (bIsAttacking)
        if (TargetData)
        {
            HitVictims = HandleSkillHitCheck(  TargetData->Radius, TargetData->Damage, TargetData->ForwardOffset, TargetData->RotationOffset );
        }


    // 4. 타격 후처리 (피흡 및 출혈)
    if (HitVictims.Num() > 0)
    {
        for (AActor* Victim : HitVictims)
        {
            // --- [핵심] 일반 공격(bIsAttacking) 혹은 강공격(bIsHeavyAttacking)일 때만 피흡 ---
            if (bIsAttacking || bIsHeavyAttacking)
            {
                if (AttributeComp)
                {
                    // 데미지의 20%만큼 내 체력 회복
                    // TargetData가 있으면 그 데미지를, 없으면 평타 기본 데미지(10.f) 기준
                    float DamageDealt = TargetData ? TargetData->Damage : 10.f;
                    float HealAmount = DamageDealt * 0.2f;

                    AttributeComp->ApplyHealthChange(HealAmount);

                    UE_LOG(LogTemp, Log, TEXT("[WereWolf] LifeSteal: +%.2f HP"), HealAmount);
                }
            }

            // --- [E 스킬 효과] 출혈 모드일 때만 출혈 적용 ---
            if (bIsBleedModeActive)
            {
                if (UAFStatusEffectComponent* TargetStatus = Victim->FindComponentByClass<UAFStatusEffectComponent>())
                {
                    // 초당 10데미지, 10초간 지속
                    TargetStatus->ApplyBleed(10.f, 10.f);
                }
            }
        }
    }
}



>>>>>>> Stashed changes
