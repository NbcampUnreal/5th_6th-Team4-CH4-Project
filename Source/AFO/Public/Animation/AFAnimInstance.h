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
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	UFUNCTION()
	void AnimNotify_AttackHit();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AAFPlayerCharacter> OwnerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovement;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector Velocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GroundSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DirectionX;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DirectionY;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bShouldMove : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bIsFalling : 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State")
	uint8 bIsAttack : 1;
};
