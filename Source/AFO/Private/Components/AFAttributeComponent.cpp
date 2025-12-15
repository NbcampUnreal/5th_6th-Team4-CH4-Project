// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AFAttributeComponent.h"
#include "GameFramework/Actor.h"
#include "Player/AFPlayerState.h"

UAFAttributeComponent::UAFAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false; 
}

void UAFAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
}

void UAFAttributeComponent::ApplyDamage(float Damage, AController* InstigatedBy)
{
	if (Damage <= 0.f) return;

	Health -= Damage;
	Health = FMath::Clamp(Health, 0.f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("%s HP: %f"), *GetOwner()->GetName(), Health);

	if (Health <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is Dead!"), *GetOwner()->GetName());

		// 죽은 플레이어 PS
		if (AActor* OwnerActor = GetOwner())
		{
			if (APawn* PawnOwner = Cast<APawn>(OwnerActor))
			{
				if (AAFPlayerState* VictimPS = PawnOwner->GetPlayerState<AAFPlayerState>())
				{
					VictimPS->AddDeath();
				}
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
}

