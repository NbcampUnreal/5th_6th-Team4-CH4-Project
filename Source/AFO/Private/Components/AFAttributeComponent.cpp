// AFAttributeComponent.cpp


#include "Components/AFAttributeComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Player/AFPlayerState.h"

UAFAttributeComponent::UAFAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAFAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	bIsDead = false;

	UE_LOG(LogTemp, Warning, TEXT("Attribute Begin"));

	if (GetOwner()->HasAuthority())
	{
		SyncHealthToPlayerState();
	}
}

void UAFAttributeComponent::ApplyDamage(float Damage, AController* InstigatedBy)
{
	// 서버 전용, 이미 사망 시 등 기존 체크 유지
	if (!GetOwner() || !GetOwner()->HasAuthority() || bIsDead || Damage <= 0.f)
	{
		return;
	}

	Health -= Damage;
	Health = FMath::Clamp(Health, 0.f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("[%s] HP: %f"), *GetOwner()->GetName(), Health);

	// ★★★ 이 부분에 PlayerState로 동기화하는 로직을 추가합니다.
	SyncHealthToPlayerState();
	// 타이머를 이용한 반복 로직은 SyncHealthToPlayerState 내부에서 처리되므로,
	// 여기서는 그냥 호출만 하면 됩니다.
	// 

	if (Health <= 0.f)
	{
		HandleDeath(InstigatedBy);
	}
}

void UAFAttributeComponent::HandleDeath(AController* InstigatedBy)
{
	//중복 호출 방지
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;

	UE_LOG(LogTemp, Warning, TEXT("[%s] is Dead"), *OwnerActor->GetName());
	
	// 피해자 Death 증가
	if (APawn* PawnOwner = Cast<APawn>(OwnerActor))
	{
		if (AAFPlayerState* VictimPS = PawnOwner->GetPlayerState<AAFPlayerState>())
		{
			VictimPS->IncrementDeathCount();
		}
	}
	
	// 공격자 Kill 증가
	if (InstigatedBy)
	{
		if (AAFPlayerState* AttackerPS = InstigatedBy->GetPlayerState<AAFPlayerState>())
		{
			AttackerPS->IncrementKillCount();
		}
	}
}



void UAFAttributeComponent::SyncHealthToPlayerState()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		// 1. PlayerState를 찾으려 시도합니다.
		if (AAFPlayerState* PS = PawnOwner->GetPlayerState<AAFPlayerState>())
		{
			// ★★★ 동기화 성공! 타이머를 멈춥니다.
			GetWorld()->GetTimerManager().ClearTimer(HealthSyncTimerHandle);

			PS->SetHealth(Health, MaxHealth);
			UE_LOG(LogTemp, Warning, TEXT("Attribute Sync: SUCCESS! Timer Cleared."));
			return;
		}
	}

	// 2. 동기화 실패 시 (Failed Attribute Sync2 발생 시)
	UE_LOG(LogTemp, Warning, TEXT("Sync FAILED. Retrying in 0.2 seconds."));

	// PlayerState를 찾을 때까지 0.2초마다 이 함수를 반복 실행합니다.
	if (!GetWorld()->GetTimerManager().IsTimerActive(HealthSyncTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(
			HealthSyncTimerHandle,
			this,
			&UAFAttributeComponent::SyncHealthToPlayerState,
			0.2f, // 0.2초마다 반복
			true  // 반복 실행
		);
	}
}


