// AFAttributeComponent.cpp



#include "Components/AFAttributeComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Player/AFPlayerState.h"
#include "Game/AFGameMode.h"
#include "UI/AFFloatingDamageManager.h"
#include "Kismet/GameplayStatics.h"

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


	// InstigatedBy를 통해 '누가 때렸는지'를 판별하여 공격자/피격자 색상을 결정
	Multicast_NotifyDamage(Damage, GetOwner()->GetActorLocation(), InstigatedBy, false);



	SyncHealthToPlayerState();
	// 타이머를 이용한 반복 로직은 SyncHealthToPlayerState 내부에서 처리되므로,
	// 여기서는 그냥 호출만 하면 됩니다.
	// 

	if (Health <= 0.f)
	{
		HandleDeath(InstigatedBy);
	}
}

//void UAFAttributeComponent::HandleDeath(AController* InstigatedBy)
//{
//	//중복 호출 방지
//	if (bIsDead)
//	{
//		return;
//	}
//
//	bIsDead = true;
//
//	if (GetWorld())
//	{
//		GetWorld()->GetTimerManager().ClearTimer(HealthSyncTimerHandle);
//	}
//
//	AActor* OwnerActor = GetOwner();
//	if (!OwnerActor) return;
//
//	UE_LOG(LogTemp, Warning, TEXT("[%s] is Dead"), *OwnerActor->GetName());
//	
//	// 피해자 Death 증가
//	if (APawn* PawnOwner = Cast<APawn>(OwnerActor))
//	{
//		if (AAFPlayerState* VictimPS = PawnOwner->GetPlayerState<AAFPlayerState>())
//		{
//			VictimPS->IncrementDeathCount();
//		}
//	}
//	
//	// 공격자 Kill 증가
//	if (InstigatedBy)
//	{
//		if (AAFPlayerState* AttackerPS = InstigatedBy->GetPlayerState<AAFPlayerState>())
//		{
//			AttackerPS->IncrementKillCount();
//		}
//	}
//}

void UAFAttributeComponent::HandleDeath(AController* InstigatedBy)
{
	if (bIsDead) return;
	bIsDead = true;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(HealthSyncTimerHandle);
	}

	SyncHealthToPlayerState();

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;

	UE_LOG(LogTemp, Warning, TEXT("[%s] is Dead"), *OwnerActor->GetName());

	if (APawn* PawnOwner = Cast<APawn>(OwnerActor))
	{
		AController* VictimController = PawnOwner->GetController();
		if (!VictimController) return;

		if (AAFGameMode* GM = GetWorld()->GetAuthGameMode<AAFGameMode>())
		{
			GM->HandlePlayerDeath(VictimController, InstigatedBy);
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




void UAFAttributeComponent::Multicast_NotifyDamage_Implementation(float Damage, FVector Location, AController* InstigatedBy, bool bIsCritical)
{
	AAFFloatingDamageManager* DamageManager = Cast<AAFFloatingDamageManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAFFloatingDamageManager::StaticClass()));

	if (DamageManager)
	{


		APlayerController* LocalPC = GetWorld()->GetFirstPlayerController();
		if (!LocalPC) return;

		AAFPlayerState* MyPS = LocalPC->GetPlayerState<AAFPlayerState>();
		if (!MyPS) return;

		// 2. 공격자의 PlayerState 가져오기
		AAFPlayerState* InstigatorPS = InstigatedBy ? InstigatedBy->GetPlayerState<AAFPlayerState>() : nullptr;

		bool bIsEnemyDamage = false;

		if (InstigatorPS)
		{
			// [팀 비교 로직]
			if (MyPS->GetTeamID() == InstigatorPS->GetTeamID())
			{
				bIsEnemyDamage = true; // 우리 팀의 공격 성공!
			}
		}
		else
		{
			// 공격자 정보가 없는 경우(예: 환경 데미지 등)는 기본적으로 빨간색
			bIsEnemyDamage = false;
		}

		DamageManager->ShowDamage(Damage, Location, bIsEnemyDamage, bIsCritical);
	}
}

