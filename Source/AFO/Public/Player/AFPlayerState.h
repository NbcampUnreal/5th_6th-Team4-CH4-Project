// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AFPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class AFO_API AAFPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AAFPlayerState();

protected:
	// Replication 설정된 변수
	UPROPERTY(ReplicatedUsing= OnRep_KillCount)
	int32 KillCount;

	UPROPERTY(ReplicatedUsing= OnRep_DeathCount)
	int32 DeathCount;

public:
	//getter
	FORCEINLINE int32 GetKillCount() const { return KillCount; }
	FORCEINLINE int32 GetDeathCount() const { return DeathCount; }

	void AddKill();
	void AddDeath();

protected:
	UFUNCTION()
	void OnRep_KillCount();

	UFUNCTION()
	void OnRep_DeathCount();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
