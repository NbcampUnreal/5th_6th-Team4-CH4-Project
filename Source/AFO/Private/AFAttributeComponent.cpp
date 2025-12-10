// Fill out your copyright notice in the Description page of Project Settings.


#include "AFAttributeComponent.h"
#include "GameFramework/Actor.h"

UAFAttributeComponent::UAFAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false; 
}

void UAFAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
}

void UAFAttributeComponent::ApplyDamage(float Damage)
{
	if (Damage <= 0.f) return;

	Health -= Damage;
	Health = FMath::Clamp(Health, 0.f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("%s HP: %f"), *GetOwner()->GetName(), Health);

	if (Health <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is Dead!"), *GetOwner()->GetName());
	}
}

