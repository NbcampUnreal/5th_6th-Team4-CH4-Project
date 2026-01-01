<<<<<<< Updated upstream
// Fill out your copyright notice in the Description page of Project Settings.
=======
// AFGameInstance.h
>>>>>>> Stashed changes

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AFGameInstance.generated.h"

<<<<<<< Updated upstream
=======
USTRUCT(BlueprintType)
struct FAFGameResultData
{
    GENERATED_BODY()

    UPROPERTY() FString PlayerName;
    UPROPERTY() int32 Kills;
    UPROPERTY() int32 Deaths;
    UPROPERTY() float DamageDealt;
    UPROPERTY() float DamageTaken;
    UPROPERTY() float Healing;
};

>>>>>>> Stashed changes

UCLASS()
class AFO_API UAFGameInstance : public UGameInstance
{
	GENERATED_BODY()
<<<<<<< Updated upstream
	
=======

public:
	UPROPERTY(BlueprintReadWrite, Category = "AFO|Login")
	FString PendingPlayerName;

	UPROPERTY(BlueprintReadWrite, Category = "AFO|Login")
	FString PendingServerIP;

    UPROPERTY()
    TArray<FAFGameResultData> FinalMatchResults;

    void ClearResults() { FinalMatchResults.Empty(); }
>>>>>>> Stashed changes
};
