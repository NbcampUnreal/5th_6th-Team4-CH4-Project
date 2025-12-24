#pragma once

#include "CoreMinimal.h"
#include "Character/AFPlayerCharacter.h"
#include "AFMage.generated.h"

UCLASS()
class AFO_API AAFMage : public AAFPlayerCharacter
{
	GENERATED_BODY()

public:
	AAFMage();
	
	virtual void StartSprint(const FInputActionValue& Value) override;
	virtual void StopSprint(const FInputActionValue& Value) override;
	
	// 점프 차단
	virtual void Jump() override;
	virtual void StopJumping() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	bool bIsSprinting = false;

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
