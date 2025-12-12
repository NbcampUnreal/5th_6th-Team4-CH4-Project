#pragma once

#include "CoreMinimal.h"
#include "Animation/AFAnimInstance.h"
#include "AFArcherAnimInstance.generated.h"

class AAFArcher;

UCLASS()
class AFO_API UAFArcherAnimInstance : public UAFAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	// Aim Offset 값들 (AnimGraph에서 사용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	float AimYaw = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	float AimPitch = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	float AimAlpha = 0.f;
};