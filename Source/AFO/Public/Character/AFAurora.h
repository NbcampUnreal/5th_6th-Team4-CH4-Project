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

	virtual void Tick(float DeltaTime) override;

	// ===== Aim Offset =====
	UPROPERTY(BlueprintReadOnly, Category="Aim")
	float AimYaw;

	UPROPERTY(BlueprintReadOnly, Category="Aim")
	float AimPitch;

	UPROPERTY(BlueprintReadOnly, Category="Aim")
	float AimAlpha;

protected:
	virtual void BeginPlay() override;
};
