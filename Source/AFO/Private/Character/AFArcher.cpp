#include "Character/AFArcher.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"

AAFArcher::AAFArcher()
{
	CharacterKey = TEXT("Archer");
}

<<<<<<< Updated upstream
void AAFArcher::BeginPlay()
{
	Super::BeginPlay();
}

void AAFArcher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Controller) return;

	const FRotator ControlRot = Controller->GetControlRotation();
	const FRotator ActorRot = GetActorRotation();
	const FRotator DeltaRot = (ControlRot - ActorRot).GetNormalized();

	AimYaw   = DeltaRot.Yaw;
	AimPitch = DeltaRot.Pitch;
	AimAlpha = 1.f;
}
=======
>>>>>>> Stashed changes
