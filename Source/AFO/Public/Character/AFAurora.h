#pragma once

#include "CoreMinimal.h"
#include "Character/AFPlayerCharacter.h"
#include "AFAurora.generated.h"

UCLASS()
class AFO_API AAFAurora : public AAFPlayerCharacter
{
	GENERATED_BODY()

public:
	AAFAurora();

protected:
	virtual void HandleOnCheckHit() override;

	UPROPERTY(EditAnywhere, Category = "AF|Stat")
	float SlowAmount = 0.5f;

	UPROPERTY(EditAnywhere, Category = "AF|Stat")
	float SlowDuration = 2.0f;
};
