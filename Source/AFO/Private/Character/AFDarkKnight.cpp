#include "Character/AFDarkKnight.h"

void AAFDarkKnight::StartSprint(const FInputActionValue& Value)
{
	Super::StartSprint(Value);

	bIsSprinting = true;
}

void AAFDarkKnight::StopSprint(const FInputActionValue& Value)
{
	Super::StopSprint(Value);

	bIsSprinting = false;
}