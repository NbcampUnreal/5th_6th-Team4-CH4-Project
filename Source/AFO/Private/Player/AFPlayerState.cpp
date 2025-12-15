// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AFPlayerState.h"

#include "Player/AFPlayerController.h"
#include "Net/UnrealNetwork.h"

AAFPlayerState::AAFPlayerState()
{
	KillCount = 0;
	DeathCount = 0;
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

void AAFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAFPlayerState, KillCount);
	DOREPLIFETIME(AAFPlayerState, DeathCount);
}

