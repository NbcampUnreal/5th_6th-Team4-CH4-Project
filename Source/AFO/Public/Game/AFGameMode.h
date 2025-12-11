#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "AFGameMode.generated.h"

class AAFGameState;
class APlayerController;

UCLASS()
class AFO_API AAFGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    AAFGameMode();

    virtual void BeginPlay() override;
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void Logout(AController* Exiting) override;

    void ReportKill(APlayerController* Killer, APlayerController* Victim);

private:
    int32 RoundDuration;
    FTimerHandle RoundTimerHandle;

    TMap<APlayerController*, uint8> PlayerTeams;

    void StartRound();
    void TickRound();
    void EndRound();

    AAFGameState* GetAFGameState() const;
};
