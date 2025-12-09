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
		/*OwnerCharacter = Cast<AAFPlayerCharacter>(OwnerPawn);
		OwnerCharacterMovement = OwnerCharacter->GetCharacterMovement();*/
		bIsAttack = false;
	}
}

void UAFAnimInstance::AnimNotify_AttackHit()
{
	if (auto Owner = TryGetPawnOwner())
	{
		if (auto Wolf = Cast<AAFPlayerCharacter>(Owner))
		{
			/*AAFPlayerCharacter->DealDamage();*/
		}
	}
}

