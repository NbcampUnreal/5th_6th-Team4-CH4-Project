#include "Character/AFMage.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"

AAFMage::AAFMage()
{
	CharacterKey = TEXT("Mage");
	PrimaryActorTick.bCanEverTick = true;
<<<<<<< Updated upstream

	AimYaw   = 0.f;
	AimPitch = 0.f;
	AimAlpha = 1.f;

	// 기본 이동 속도
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
}

void AAFMage::StartSprint(const FInputActionValue& Value)
{
	Super::StartSprint(Value);
	bIsSprinting = true;
}

void AAFMage::StopSprint(const FInputActionValue& Value)
{
	Super::StopSprint(Value);
	bIsSprinting = false;
}

void AAFMage::Jump()
{
	// 마법사는 점프 불가
}

void AAFMage::StopJumping()
{
	// 마법사는 점프 불가
}

void AAFMage::BeginPlay()
{
	Super::BeginPlay();
=======
>>>>>>> Stashed changes
}

void AAFMage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
<<<<<<< Updated upstream

	if (!Controller) return;

	const FRotator ControlRot = Controller->GetControlRotation();
	const FRotator ActorRot   = GetActorRotation();
	const FRotator DeltaRot  = (ControlRot - ActorRot).GetNormalized();

	AimYaw   = DeltaRot.Yaw;
	AimPitch = FMath::Clamp(DeltaRot.Pitch, -90.f, 90.f);
	AimAlpha = 1.f;
}
=======
	// [비주얼] 에임 오프셋 계산 로직 (간소화)
	if (Controller)
	{
		FRotator DeltaRot = (Controller->GetControlRotation() - GetActorRotation()).GetNormalized();
		AimYaw = DeltaRot.Yaw;
		AimPitch = FMath::Clamp(DeltaRot.Pitch, -90.f, 90.f);
	}
}

#pragma region Movement

void AAFMage::Jump()
{
	// 마법사는 점프 불가
}
void AAFMage::StopJumping()
{
	// 마법사는 점프 불가
}
#pragma endregion

// E 스킬: 보호막 (공격 판정이 아니라 아군 서포트)
void AAFMage::ServerRPC_SkillE_Implementation()
{
	if (SkillComp->CanUseSkill(CachedSkillEKey) && AttributeComp)
	{
		float RequiredMana = CachedSkillEData->ManaCost;
		if (AttributeComp->GetMana() >= RequiredMana)
		{
			bIsUsingSkill = true;
			AttributeComp->ApplyManaChange(-RequiredMana);
			ApplyShieldToAllies(CachedSkillEData->Radius, E_ShieldAmount);
			Multicast_PlayMontage(SkillEMontage);
		}

	}
}

void AAFMage::HandleOnCheckHit()
{
    if (!HasAuthority() || IsDead() || !SkillComp || !AnimInstance) return;

    // 데이터 캐싱 확인
    FName TargetKey = NAME_None;
    FAFSkillInfo* TargetData = nullptr;

    if (bIsHeavyAttacking) { TargetKey = CachedHeavyAttackKey; TargetData = CachedHeavyAttackData; }
    else if (bIsUsingSkill)
    {
        if (AnimInstance->Montage_IsPlaying(SkillEMontage)) { TargetKey = CachedSkillEKey; TargetData = CachedSkillEData; }
        else if (AnimInstance->Montage_IsPlaying(SkillQMontage)) { TargetKey = CachedSkillQKey; TargetData = CachedSkillQData; }
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

    // [CASE 1] 일반 공격 (마나 번 패시브)
    if (bIsAttacking && AnimInstance->Montage_IsPlaying(AttackMontage))
    {
        // 평타 판정 (TargetData가 없을 경우를 대비해 수동 입력 가능)
        TArray<AActor*> HitVictims = HandleSkillHitCheck(TargetData->Radius, TargetData->Damage, TargetData->ForwardOffset, TargetData->RotationOffset);
        for (AActor* Victim : HitVictims)
        {
            if (AAFPlayerCharacter* Enemy = Cast<AAFPlayerCharacter>(Victim))
            {
                if (UAFAttributeComponent* EnemyAttr = Enemy->GetAttributeComponent())
                {
                    EnemyAttr->ApplyManaChange(-ManaBurnAmount); // 내 마나가 아니라 적의 마나를 깎음
                }
            }
        }
    }
    // [CASE 2] Q 스킬 (특정 프레임에서 이펙트 소환 및 판정)
    else if (TargetKey == CachedSkillQKey && TargetData)
    {
        if (!bQFxPlayed && AnimInstance->Montage_GetPosition(SkillQMontage) >= 1.4f)
        {
            bQFxPlayed = true;
            SpawnEffectAndDamage(SkillQEffectBP, TargetData);
            SkillComp->StartCooldown(TargetKey, TargetData->Cooldown);
        }
    }
    // [CASE 3] 강공격 (특정 프레임에서 이펙트 소환 및 판정)
    else if (bIsHeavyAttacking && TargetData)
    {
        if (!bHeavyFxPlayed && AnimInstance->Montage_GetPosition(HeavyAttackMontage) >= 0.5f)
        {
            bHeavyFxPlayed = true;
            SpawnEffectAndDamage(HeavyAttackEffectBP, TargetData);
            SkillComp->StartCooldown(TargetKey, TargetData->Cooldown);
        }
    }
}

// --- 3. 공통 이펙트 생성 및 데미지 처리 (내부 함수) ---
void AAFMage::SpawnEffectAndDamage(TSubclassOf<AActor> EffectClass, FAFSkillInfo* Data)
{
    if (!Data) return;

    // 바닥 트레이스를 통한 소환 위치 계산
    FVector FxBase = GetActorLocation() + (GetActorForwardVector() * Data->ForwardOffset);
    FHitResult Hit;
    FVector TraceStart = FxBase + FVector(0, 0, 1000.f);
    FVector TraceEnd = FxBase - FVector(0, 0, 1500.f);

    FVector FinalLoc = FxBase;
    if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility))
    {
        FinalLoc = Hit.ImpactPoint;
    }

    // 1) 데미지 판정
    HandleSkillHitCheck(Data->Radius, Data->Damage, Data->ForwardOffset, Data->RotationOffset);

    // 2) 이펙트 소환 멀티캐스트 (통합된 함수 호출)
    Multicast_SpawnEffect(EffectClass, FinalLoc, GetActorRotation());
}

// --- 4. 시각 효과 (멀티캐스트) ---
void AAFMage::Multicast_SpawnEffect_Implementation(TSubclassOf<AActor> EffectClass, FVector Location, FRotator Rotation)
{
    if (!EffectClass) return;
    GetWorld()->SpawnActor<AActor>(EffectClass, Location, Rotation);
}
		
// 보호막 전용 로직
void AAFMage::ApplyShieldToAllies(float Radius, float Amount)
{
    if (!HasAuthority()) return;

    TArray<FOverlapResult> OverlapResults;
    FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);

    if (GetWorld()->OverlapMultiByChannel(OverlapResults, GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere))
    {
        for (auto& Result : OverlapResults)
        {
            AActor* HitActor = Result.GetActor();
            if (HitActor && (HitActor == this || IsAlly(HitActor)))
            {
                if (UAFAttributeComponent* Attr = HitActor->FindComponentByClass<UAFAttributeComponent>())
                {
                    // 1. 서버에서 데이터 변경 (복제됨)
                    Attr->AddShield(Amount, E_Duration);

                    // 2. 모든 클라이언트에게 이펙트 재생 명령
                    Multicast_PlayShieldEffect(HitActor);
                }
            }
        }
    }
}

void AAFMage::Multicast_PlayShieldEffect_Implementation(AActor* TargetActor)
{
    // 타겟이 유효하고 팀원이 만든 나이아가라 에셋이 할당되어 있다면 재생
    if (TargetActor && ShieldEffect)
    {
        // 핵심: SpawnSystemAtLocation이 아니라 SpawnSystemAttached를 사용합니다.
        UNiagaraFunctionLibrary::SpawnSystemAttached(
            ShieldEffect,               // 재생할 나이아가라 시스템
            TargetActor->GetRootComponent(), // 부착할 부모 컴포넌트 (Root)
            NAME_None,                  // 부착할 소켓 이름 (필요 없으면 None)
            FVector::ZeroVector,        // 상대적 위치 (0,0,0이면 타겟 정중앙)
            FRotator::ZeroRotator,      // 상대적 회전
            EAttachLocation::SnapToTarget, // 핵심: 타겟 위치에 딱 붙임
            true                        // 시스템 자동 파괴 (이펙트 종료 시)
        );

        UE_LOG(LogTemp, Log, TEXT("Shield Effect Played on: %s"), *TargetActor->GetName());
    }
}
>>>>>>> Stashed changes
