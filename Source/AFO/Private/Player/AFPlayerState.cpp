// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AFPlayerState.h"

#include "Player/AFPlayerController.h"
#include "Net/UnrealNetwork.h"

AAFPlayerState::AAFPlayerState()
{
	KillCount = 0;
	DeathCount = 0;
	
	MaxMana = 10000.f;
	CurrentMana = MaxMana;
}

void AAFPlayerState::AddKill()
{
	KillCount++;

	// 클라이언트에게 갱신
	OnRep_KillCount();
}

void AAFPlayerState::AddDeath()
{
	DeathCount++;

	// 클라이언트에게 갱신
	OnRep_DeathCount();
}

void AAFPlayerState::OnRep_KillCount()
{
	UE_LOG(LogTemp, Warning, TEXT("KillCount Updated: %d"), KillCount);
}

void AAFPlayerState::OnRep_DeathCount()
{
	UE_LOG(LogTemp, Warning, TEXT("DeathCount Updated: %d"), DeathCount);
}

//Mana추가

void AAFPlayerState::SetMana(float NewMana)
{
	if (!HasAuthority()) return;

	CurrentMana = FMath::Clamp(NewMana, 0.f, MaxMana);
}

void AAFPlayerState::AddMana(float Amount)
{
	if (!HasAuthority()) return;

	SetMana(CurrentMana + Amount);
}

bool AAFPlayerState::ConsumeMana(float Amount)
{
	if (!HasAuthority()) return false;

	if (CurrentMana < Amount)
	{
		return false;
	}

	SetMana(CurrentMana - Amount);
	return true;
}

void AAFPlayerState::OnRep_CurrentMana()
{
	UE_LOG(LogTemp, Log, TEXT("Mana Updated: %f / %f"), CurrentMana, MaxMana);
	
}

void AAFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAFPlayerState, KillCount);
	DOREPLIFETIME(AAFPlayerState, DeathCount);
}

