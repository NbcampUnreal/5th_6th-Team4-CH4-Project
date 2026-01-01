#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "AFGameMode.generated.h"

class AAFGameState;
class APlayerController;
class AController;

UCLASS()
class AFO_API AAFGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AAFGameMode();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

	void ReportKill(AController* KillerController);
	void EndRound();

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	void HandlePlayerDeath(AController* VictimController, AController* KillerController);




protected:
	int32 RoundDuration;

	TMap<APlayerController*, uint8> PlayerTeams;
	TMap<TWeakObjectPtr<AController>, FTimerHandle> RespawnTimers;
	void StartRound();
	AAFGameState* GetAFGameState() const;

	float RespawnDelay = 10.0f;




	// 게임 시작을 알리는 함수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle")
	int32 StartDelayTime = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TArray<TSubclassOf<APawn>> CharacterPawnClasses;

	FTimerHandle TimerHandle_GameStart;


	virtual void RestartPlayer(AController* NewPlayer) override;
};