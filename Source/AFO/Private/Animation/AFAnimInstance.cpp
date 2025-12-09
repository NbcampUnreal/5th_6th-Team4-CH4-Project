#include "Animation/AFAnimInstance.h"
#include "Character/AFPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UAFAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	APawn* OwnerPawn = TryGetPawnOwner();
	if (IsValid(OwnerPawn) == true)
	{
		OwnerCharacter = Cast<AAFPlayerCharacter>(OwnerPawn);
		OwnerCharacterMovement = OwnerCharacter->GetCharacterMovement();
		bIsAttack = false;
	}
}

void UAFAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (IsValid(OwnerCharacter) == true && IsValid(OwnerCharacterMovement) == true)
	{
		Velocity = OwnerCharacterMovement->Velocity;
		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);
		
		float GroundAcceleration = UKismetMathLibrary::VSizeXY(OwnerCharacterMovement->GetCurrentAcceleration());
		bool bIsAccelerationNearlyZero = FMath::IsNearlyZero(GroundAcceleration);
		bShouldMove = (KINDA_SMALL_NUMBER < GroundSpeed) && (bIsAccelerationNearlyZero == false);
		
		bIsFalling = OwnerCharacterMovement->IsFalling();
	}
}

void UAFAnimInstance::AnimNotify_AttackHit()
{
	if (auto Owner = TryGetPawnOwner())
	{
		if (auto Character = Cast<AAFPlayerCharacter>(Owner))
		{
			Character->DealDamage();
		}
	}
}

