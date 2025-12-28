#include "Character/AFWereWolf.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Components/AFAttributeComponent.h"


void AAFWereWolf::BeginPlay()
{
	Super::BeginPlay();
	
}

AAFWereWolf::AAFWereWolf()
{
	PrimaryActorTick.bCanEverTick = false;
}

// 패시브: 평타 피흡 (공격 적중 시 외부에서 호출)
void AAFWereWolf::ApplyLifeSteal()
{
	if (!HasAuthority()) return;

	if (UAFAttributeComponent* Attr = FindComponentByClass<UAFAttributeComponent>())
	{
		Attr->ApplyHealthChange(+10);	// 10 회복
	}
}

// Q 스킬: 이속 증가
void AAFWereWolf::Server_ApplySpeedBoost_Implementation(float Ratio, float Duration)
{
	if (!HasAuthority()) return;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp) return;

	// 기존 속도 저장
	if (OriginalWalkSpeed == 0.f)
		OriginalWalkSpeed = MoveComp->MaxWalkSpeed;

	// 30% 증가 → *1.3f
	MoveComp->MaxWalkSpeed = OriginalWalkSpeed * Ratio;

	// 타이머 초기화 후 만료 시 원복
	GetWorld()->GetTimerManager().ClearTimer(SpeedBoostTimer);
	GetWorld()->GetTimerManager().SetTimer(
		SpeedBoostTimer,
		this,
		&AAFWereWolf::ResetSpeedBoost,
		Duration,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("[WereWolf] SpeedBoost: %.2f for %.2fs"), Ratio, Duration);
}

void AAFWereWolf::ResetSpeedBoost()
{
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = OriginalWalkSpeed;
	}
}


// E 스킬: 출혈 데미지 (초당 반복)

void AAFWereWolf::Server_ApplyBleeding_Implementation(AActor* Target, float DamagePerSec, float Duration)
{
	if (!HasAuthority() || !Target) return;

	int32 TickCount = FMath::FloorToInt(Duration); // Duration=3 → 3초

	GetWorld()->GetTimerManager().SetTimer(
		BleedingTimer,
		FTimerDelegate::CreateLambda([=,this]()
		{
			ApplyBleedingTick(Target, DamagePerSec);
		}),
		1.0f,
		true
	);

	// Duration 후 정지
	FTimerHandle StopHandle;
	GetWorld()->GetTimerManager().SetTimer(
		StopHandle,
		[this]()
		{
			GetWorld()->GetTimerManager().ClearTimer(BleedingTimer);
		},
		Duration,
		false
	);
}

void AAFWereWolf::ApplyBleedingTick(AActor* Target, float DamagePerSec)
{
	if (!HasAuthority() || !Target) return;

	if (UAFAttributeComponent* Attr = Target->FindComponentByClass<UAFAttributeComponent>())
	{
		Attr->ApplyDamage(DamagePerSec, GetController());
	}

	UE_LOG(LogTemp, Warning, TEXT("[WereWolf] Bleeding: -%f"), DamagePerSec);
}

