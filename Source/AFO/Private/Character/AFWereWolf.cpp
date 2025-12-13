#include "Character/AFWereWolf.h"
#include "GameFramework/CharacterMovementComponent.h"

AAFWereWolf::AAFWereWolf()
{
	bCanSprint = false;
}

void AAFWereWolf::BeginPlay()
{
	Super::BeginPlay();
	
	NormalSpeed = WereWolfMoveSpeed;

	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}
