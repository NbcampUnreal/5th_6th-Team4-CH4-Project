#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AFAnimInstance.generated.h"

class AAFPlayerCharacter;
class UCharacterMovementComponent;

UCLASS()
class AFO_API UAFAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	
	UFUNCTION()
	void AnimNotify_AttackHit();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State")
	uint8 bIsAttack : 1;
};
