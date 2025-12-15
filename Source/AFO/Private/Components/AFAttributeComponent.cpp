// Fill out your copyright notice in the Description page of Project Settings.


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
}

void UAFAttributeComponent::ApplyDamage(float Damage, AController* InstigatedBy)
{
	// 서버 전용
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	// 이미 죽었으면 추가 데미지 차단
	if (bIsDead)
	{
		return;
	}

	if (Damage <= 0.f)
	{
		return;
	}

	Health -= Damage;
	Health = FMath::Clamp(Health, 0.f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("[%s] HP: %f"), *GetOwner()->GetName(), Health);

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
			VictimPS->AddDeath();
		}
	}
	
	// 공격자 Kill 증가
	if (InstigatedBy)
	{
		if (AAFPlayerState* AttackerPS = InstigatedBy->GetPlayerState<AAFPlayerState>())
		{
			AttackerPS->AddKill();
		}
	}
}

