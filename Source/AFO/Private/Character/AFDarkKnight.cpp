#include "Character/AFDarkKnight.h"
<<<<<<< Updated upstream

void AAFDarkKnight::StartSprint(const FInputActionValue& Value)
{
	Super::StartSprint(Value);

	bIsSprinting = true;
}

void AAFDarkKnight::StopSprint(const FInputActionValue& Value)
{
	Super::StopSprint(Value);

	bIsSprinting = false;
}
=======
#include "Components/AFAttributeComponent.h"
#include "TimerManager.h"
#include "Components/AFStatusEffectComponent.h"
#include "Components/AFSkillComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AAFDarkKnight::AAFDarkKnight()
{
	CharacterKey = TEXT("DarkKnight");
	bCanSprint = true;
}

void AAFDarkKnight::HandleOnCheckHit()
{
	if (!HasAuthority() || IsDead() || !SkillComp) return;

	FName TargetKey = NAME_None;
	FAFSkillInfo* TargetData = nullptr;
	TArray<AActor*> HitVictims;

	// 1. 현재 어떤 공격 중인지 판별 및 데이터 확보
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
	}

	if (HitVictims.Num() > 0 && HasAuthority())
	{
		for (AActor* Victim : HitVictims)
		{
			// 20% 확률
			float RandomValue = FMath::FRand();
			if (RandomValue <= PassiveBleedProbability)
			{
				if (UAFStatusEffectComponent* Status = Victim->FindComponentByClass<UAFStatusEffectComponent>())
				{
					// 초당 5 데미지, 10초 동안 출혈
					Status->ApplyBleed(5.f, 10.f);
					UE_LOG(LogTemp, Warning, TEXT("[DK] Passive Bleed Applied!"));
				}
			}
		}
	}
}

void AAFDarkKnight::ApplyQBuff_Server()
{
	if (!HasAuthority() || !bIsQCharging || bIsQBuffActive) return;

	bIsQCharging = false;
	bIsQBuffActive = true;


	if (AttributeComp)
	{
		// 1. 체력 20% 즉시 회복
		float MaxHP = AttributeComp->GetMaxHealth();
		float HealAmount = MaxHP * QBuff_HealRatio;
		AttributeComp->ApplyHealthChange(HealAmount);
	
		// 2. 공격력 증가
		AttributeComp->ApplyAttackBuff(1.2f, QBuffDuration);
	}
	
	// 3. 이동속도 증가
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed *= QBuff_SpeedRatio;
	}

	// [VFX] 이펙트 시작
	Multicast_StartQChargeFX();

	// 10초 후 버프 종료
	GetWorldTimerManager().SetTimer(QBuffTimer, this, &AAFDarkKnight::EndQBuff, QBuffDuration, false);
}

void AAFDarkKnight::EndQBuff()
{
	if (!HasAuthority() || !bIsQBuffActive) return;

	// 이동속도 원복
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed /= QBuff_SpeedRatio;
	}

	bIsQBuffActive = false;
	bIsUsingSkill = false;

	Multicast_StopQChargeFX();
	UE_LOG(LogTemp, Warning, TEXT("[DK Q] Buff Ended (Speed Restored)"));
}

void AAFDarkKnight::Multicast_PlayQChargeMontage_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("[DK Q] Multicast_PlayQChargeMontage"));

	if (SkillQChargeMontage)
	{
		PlayAnimMontage(SkillQChargeMontage);
	}
}

void AAFDarkKnight::Multicast_StartQChargeFX_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("[DK Q] Multicast_StartQChargeFX CALLED"));

	if (!QChargeFXBP || !GetMesh())
	{
		UE_LOG(LogTemp, Warning, TEXT("[DK Q] FXBP or Mesh is NULL"));
		return;
	}

	if (QChargeFXActor)
	{
		QChargeFXActor->Destroy();
		QChargeFXActor = nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	QChargeFXActor = GetWorld()->SpawnActor<AActor>(QChargeFXBP, GetActorLocation(), GetActorRotation(), Params);
	if (!QChargeFXActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DK Q] SpawnActor FAILED"));
		return;
	}

	QChargeFXActor->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		QChargeAttachSocket
	);

	QChargeFXActor->SetActorRelativeLocation(QChargeFXLocationOffset);
	QChargeFXActor->SetActorRelativeRotation(QChargeFXRotationOffset);

	UE_LOG(LogTemp, Warning, TEXT("[DK Q] FX SPAWN OK"));
}

void AAFDarkKnight::Multicast_StopQChargeFX_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("[DK Q] Multicast_StopQChargeFX"));

	if (QChargeFXActor)
	{
		QChargeFXActor->Destroy();
		QChargeFXActor = nullptr;
	}
}
>>>>>>> Stashed changes
