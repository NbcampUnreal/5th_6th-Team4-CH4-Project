#include "Animation/AFArcherAnimInstance.h"
#include "Character/AFArcher.h"

void UAFArcherAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (AAFArcher* Archer = Cast<AAFArcher>(OwnerCharacter))
	{
		AimYaw   = Archer->AimYaw;
		AimPitch = Archer->AimPitch;
		AimAlpha = Archer->AimAlpha;
	}
}