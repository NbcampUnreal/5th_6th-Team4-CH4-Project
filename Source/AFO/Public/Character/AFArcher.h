#pragma once

#include "CoreMinimal.h"
#include "Character/AFPlayerCharacter.h"
#include "AFArcher.generated.h"

UCLASS()
class AFO_API AAFArcher : public AAFPlayerCharacter
{
	GENERATED_BODY()

public:
	AAFArcher();

	virtual void Tick(float DeltaTime) override;

	// AimOffset 값
	UPROPERTY(BlueprintReadOnly, Category="Aim")
	float AimYaw;

	UPROPERTY(BlueprintReadOnly, Category="Aim")
	float AimPitch;

	UPROPERTY(BlueprintReadOnly, Category="Aim")
	float AimAlpha;


protected:
	virtual void BeginPlay() override;
};