// AFStatusEffectComponent.cpp


#include "Components/AFStatusEffectComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Components/AFAttributeComponent.h"

UAFStatusEffectComponent::UAFStatusEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UAFStatusEffectComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAFStatusEffectComponent, bIsSlowed);
	DOREPLIFETIME(UAFStatusEffectComponent, bIsBleeding);
}

void UAFStatusEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return;

	MoveComp = OwnerCharacter->GetCharacterMovement();
	if (MoveComp)
	{
		OriginalWalkSpeed = MoveComp->MaxWalkSpeed;
	}
}

void UAFStatusEffectComponent::ApplySlow(float InSlowRatio, float InDuration)
{
	//서버 전용
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	if (!MoveComp) return;

	//더 약한 슬로우는 무시
	if (bIsSlowed && InSlowRatio >= SlowRatio)
	{
		return;
	}

	//20% 슬로우
	SlowRatio = FMath::Clamp(InSlowRatio, 0.1f, 1.f);
	SlowDuration = InDuration;

	ApplySlow_Internal();

	// 타이머 리셋
	GetWorld()->GetTimerManager().ClearTimer(SlowTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(
		SlowTimerHandle,
		this,
		&UAFStatusEffectComponent::ClearSlow,
		SlowDuration,
		false
	);
}

void UAFStatusEffectComponent::ApplySlow_Internal()
{
	if (!MoveComp) return;

	bIsSlowed = true;
	MoveComp->MaxWalkSpeed = OriginalWalkSpeed * SlowRatio;

	UE_LOG(LogTemp, Warning, TEXT("[Slow] Applied Ratio=%.2f"), SlowRatio);
}

void UAFStatusEffectComponent::ClearSlow()
{
	if (!MoveComp) return;

	MoveComp->MaxWalkSpeed = OriginalWalkSpeed;
	bIsSlowed = false;
	SlowRatio = 1.f;

	UE_LOG(LogTemp, Warning, TEXT("[Slow] Cleared"));
}

void UAFStatusEffectComponent::OnRep_IsSlowed()
{
	if (!OwnerCharacter || !OwnerCharacter->GetMesh()) return;

	if (bIsSlowed)
	{
		// 1. 몸을 파랗게 만드는 머티리얼 덧씌우기 (UE5 기능)
		OwnerCharacter->GetMesh()->SetOverlayMaterial(SlowOverlayMaterial);

		// 2. 필요하다면 애니메이션 속도 조절 (옵션)
		OwnerCharacter->GetMesh()->GlobalAnimRateScale = 0.5f;
	}
	else
	{
		// 원상 복구
		OwnerCharacter->GetMesh()->SetOverlayMaterial(nullptr);
		OwnerCharacter->GetMesh()->GlobalAnimRateScale = 1.0f;
	}
}

void UAFStatusEffectComponent::ApplyBleed(float DamagePerSec, float Duration)
{
	if (GetOwner() == nullptr || !GetOwner()->HasAuthority() || bIsBleeding) return;

	// 출혈 비주얼 동기화 (RepNotify 변수 bIsBleeding 활용)
	bIsBleeding = true;
	OnRep_IsBleeding(); // 서버에서도 즉시 적용

	// 1초마다 데미지 주기
	GetWorld()->GetTimerManager().SetTimer(BleedTickTimer, FTimerDelegate::CreateLambda([this, DamagePerSec]()
		{
			if (UAFAttributeComponent* Attr = OwnerCharacter->FindComponentByClass<UAFAttributeComponent>())
			{
				Attr->ApplyDamage(DamagePerSec, nullptr);
			}
		}), 1.0f, true);

	// 지속시간 후 정지
	FTimerHandle StopHandle;
	GetWorld()->GetTimerManager().SetTimer(StopHandle, FTimerDelegate::CreateLambda([this]()
		{
			GetWorld()->GetTimerManager().ClearTimer(BleedTickTimer);
			bIsBleeding = false;
			OnRep_IsBleeding();
		}), Duration, false);
}

void UAFStatusEffectComponent::SetBleedVisual(bool bInBleeding)
{
	if (GetOwner() == nullptr || !GetOwner()->HasAuthority()) return;
	bIsBleeding = bInBleeding;
	OnRep_IsBleeding(); // 서버는 직접 호출
}

void UAFStatusEffectComponent::OnRep_IsBleeding()
{
	if (!OwnerCharacter || !OwnerCharacter->GetMesh()) return;

	if (bIsBleeding)
	{
		// 몸을 빨갛게 만드는 머티리얼 덧씌우기
		OwnerCharacter->GetMesh()->SetOverlayMaterial(BleedOverlayMaterial);
	}
	else
	{
		// 원상 복구 (슬로우가 걸려있다면 슬로우 머티리얼로, 아니면 nullptr)
		if (bIsSlowed)
			OwnerCharacter->GetMesh()->SetOverlayMaterial(SlowOverlayMaterial);
		else
			OwnerCharacter->GetMesh()->SetOverlayMaterial(nullptr);
	}
}
