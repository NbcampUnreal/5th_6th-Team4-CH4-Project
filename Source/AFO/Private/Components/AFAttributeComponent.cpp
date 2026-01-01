// AFAttributeComponent.cpp

#include "Components/AFAttributeComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Player/AFPlayerState.h"
#include "Game/AFGameMode.h"
#include "UI/AFFloatingDamageManager.h"
#include "Kismet/GameplayStatics.h"
<<<<<<< Updated upstream
=======
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Types/AFGameTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/AFPlayerCharacter.h"
>>>>>>> Stashed changes

// ===========================================
// 0. 초기화 및 동기화
// ===========================================
#pragma region InitializeSetting
UAFAttributeComponent::UAFAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

<<<<<<< Updated upstream
=======
AAFPlayerState* UAFAttributeComponent::GetPlayerState() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	return OwnerPawn ? OwnerPawn->GetPlayerState<AAFPlayerState>() : nullptr;
}

void UAFAttributeComponent::InitializeAttributes(const FAFPlayerCharacterStatRow& StatRow)
{
	if (!GetOwner()->HasAuthority()) return;

	// 1. 내부 수치 설정
	MaxHealth = StatRow.MaxHp;
	CurrentHealth = MaxHealth;
	MaxMana = StatRow.MaxMana;
	CurrentMana = MaxMana;
	AttackPower = StatRow.Attack;
	BaseWalkSpeed = StatRow.MoveSpeed;

	// 2. 이동 속도 설정 (CharacterMovement 제어)
	if (ACharacter* OwnerChar = Cast<ACharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* MoveComp = OwnerChar->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = BaseWalkSpeed;
		}
	}

	// 3. PlayerState로 즉시 동기화
	SyncToPlayerState();
}

void UAFAttributeComponent::SyncToPlayerState()
{
	AAFPlayerState* PS = GetPlayerState();
	if (PS)
	{
		PS->SetHealth(CurrentHealth, MaxHealth);
		PS->SetMana(CurrentMana, MaxMana);
	}
}

>>>>>>> Stashed changes
void UAFAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

<<<<<<< Updated upstream
	Health = MaxHealth;
	bIsDead = false;

	UE_LOG(LogTemp, Warning, TEXT("Attribute Begin"));

	if (GetOwner()->HasAuthority())
	{
		SyncHealthToPlayerState();
	}
}
=======
	bIsDead = false;

	if (GetOwner()->HasAuthority())
	{
		SyncToPlayerState();
	}
}

void UAFAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAFAttributeComponent, CurrentShield);
}

#pragma endregion

// ===========================================
// 1. 속성 제어 
// ===========================================
#pragma region ControlAttribute
>>>>>>> Stashed changes

void UAFAttributeComponent::ApplyDamage(float Damage, AController* InstigatedBy)
{
	// 1. 서버 전용 및 유효성 체크
	if (!GetOwner() || !GetOwner()->HasAuthority() || bIsDead || Damage <= 0.f)
	{
		return;
	}

	Health -= Damage;
	Health = FMath::Clamp(Health, 0.f, MaxHealth);

<<<<<<< Updated upstream
	UE_LOG(LogTemp, Warning, TEXT("[%s] HP: %f"), *GetOwner()->GetName(), Health);
=======
	// 2. 공격자(InstigatedBy)의 버프 확인  -- Attack버프존
	if (InstigatedBy)
	{
		// 공격자의 PlayerState나 Character에서 AttributeComponent를 찾아 배수를 가져옴
		if (APawn* InstigatorPawn = InstigatedBy->GetPawn())
		{
			if (UAFAttributeComponent* AttackerAttr = InstigatorPawn->FindComponentByClass<UAFAttributeComponent>())
			{
				FinalDamage *= AttackerAttr->GetAttackMultiplier();	// 공격자의 배수를 최종 데미지에 곱함
			}
		}
	}

	// 3. 보호막(Shield) 처리
	if (CurrentShield > 0.f)
	{
		float DamageToShield = FMath::Min(CurrentShield, FinalDamage);
		CurrentShield -= DamageToShield;
		FinalDamage -= DamageToShield;
		OnRep_CurrentShield();
	}

	// 4. 체력 차감 및 보고
	if (FinalDamage > 0.f)
	{

		// 1. 받은 피해 기록 (본인 PS)
		if (AAFPlayerState* MyPS = GetPlayerState())
		{
			MyPS->AddDamageTaken(FinalDamage);
		}
>>>>>>> Stashed changes

		// 2. 가한 피해 기록 (공격자 PS)
		if (InstigatedBy)
		{
			if (AAFPlayerState* AttackerPS = InstigatedBy->GetPlayerState<AAFPlayerState>())
			{
				AttackerPS->AddDamageDealt(FinalDamage);
			}
		}

<<<<<<< Updated upstream
	// InstigatedBy를 통해 '누가 때렸는지'를 판별하여 공격자/피격자 색상을 결정
	Multicast_NotifyDamage(Damage, GetOwner()->GetActorLocation(), InstigatedBy, false);
=======
		CurrentHealth = FMath::Clamp(CurrentHealth - FinalDamage, 0.f, MaxHealth);
		SyncToPlayerState();
>>>>>>> Stashed changes

		Multicast_NotifyDamage(FinalDamage, GetOwner()->GetActorLocation(), InstigatedBy, false);

<<<<<<< Updated upstream

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
=======
		if (CurrentHealth <= 0.f)
		{
			HandleDeath(InstigatedBy);
		}
	}
}
>>>>>>> Stashed changes

void UAFAttributeComponent::HandleDeath(AController* InstigatedBy)
{
	if (bIsDead) return;
	bIsDead = true;

	SyncToPlayerState();

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

	if (AAFPlayerCharacter* OwnerChar = Cast<AAFPlayerCharacter>(GetOwner()))
	{
		OwnerChar->StartDeath(InstigatedBy);
	}
}

<<<<<<< Updated upstream

void UAFAttributeComponent::SyncHealthToPlayerState()
=======
void UAFAttributeComponent::ApplyHealthChange(float Amount)
>>>>>>> Stashed changes
{
	if (!GetOwner()->HasAuthority() || bIsDead) return;

	// 3. 힐량 기록 (본인 PS)
	if (AAFPlayerState* MyPS = GetPlayerState())
	{
		MyPS->AddHealingDone(Amount);
	}

	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.f, MaxHealth);
	SyncToPlayerState();
}

void UAFAttributeComponent::ApplyManaChange(float Amount)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	CurrentMana = FMath::Clamp(CurrentMana + Amount, 0.f, MaxMana);
	SyncToPlayerState();
}


#pragma endregion

// ===========================================
// 3. 보호막 & 버프
// ===========================================
#pragma region Shield&Buff
void UAFAttributeComponent::ApplyAttackBuff(float Multiplier, float Duration)
{
	if (GetOwnerRole() < ROLE_Authority) return;

	AttackMultiplier = Multiplier;

	// 기존 타이머가 있다면 초기화 (버프 시간 갱신)
	GetWorld()->GetTimerManager().SetTimer(AttackBuffTimerHandle, [this]()
		{
			AttackMultiplier = 1.0f; // 원래대로 복구
			UE_LOG(LogTemp, Log, TEXT("Attack Buff Expired!"));
		}, Duration, false);
}

<<<<<<< Updated upstream

=======
void UAFAttributeComponent::AddShield(float Amount, float Duration)
{
	if (!GetOwner()->HasAuthority()) return;

	CurrentShield += Amount;
	OnRep_CurrentShield(); // 서버에서도 즉시 알림

	// 지속시간이 지나면 보호막 제거 타이머 설정
	GetWorld()->GetTimerManager().SetTimer(ShieldTimerHandle, this, &UAFAttributeComponent::OnShieldExpired, Duration, false);
}
>>>>>>> Stashed changes

void UAFAttributeComponent::OnRep_CurrentShield()
{
	OnShieldChanged.Broadcast(CurrentShield);
}

void UAFAttributeComponent::OnShieldExpired()
{
	CurrentShield = 0.f;
	OnRep_CurrentShield();
}
#pragma endregion

// ===========================================
// 4. 시각효과 (Notify, Aura)
// ===========================================
#pragma region Notify&Aura

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
<<<<<<< Updated upstream

=======
void UAFAttributeComponent::Multicast_ApplyAura_Implementation(UNiagaraSystem* AuraSystem, FLinearColor Color, float Duration)
{
	if (!AuraSystem || !GetOwner()) return;

	// [중요] 기존 오오라 제거 로직을 별도로 분리
	if (ActiveAuraComponent && ActiveAuraComponent->IsValidLowLevel())
	{
		ActiveAuraComponent->DestroyComponent();
	}
	ActiveAuraComponent = nullptr;

	// 오라 붙이기
	ActiveAuraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
		AuraSystem,
		GetOwner()->GetRootComponent(),
		NAME_None,
		FVector(0.f, 0.f, -90.f),
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		true
	);

	if (ActiveAuraComponent)
	{
		// 색상 적용 전후로 로그를 찍어 Alpha값 확인
		ActiveAuraComponent->SetNiagaraVariableLinearColor(TEXT("User.BaseColor"), Color);
		ActiveAuraComponent->Activate(true);

		UE_LOG(LogTemp, Warning, TEXT("[%s] Aura Attached! Color Alpha: %f"), *GetOwner()->GetName(), Color.A);

		// 타이머 로직
		FTimerHandle AuraTimer;
		TWeakObjectPtr<UNiagaraComponent> WeakComp = ActiveAuraComponent;
		GetWorld()->GetTimerManager().SetTimer(AuraTimer, [WeakComp]()
			{
				if (WeakComp.IsValid())
				{
					WeakComp->DestroyComponent();
				}
			}, Duration, false);
	}
}

#pragma endregion
>>>>>>> Stashed changes
