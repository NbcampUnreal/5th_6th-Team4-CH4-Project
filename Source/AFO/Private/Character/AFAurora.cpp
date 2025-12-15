#include "Character/AFAurora.h"
#include "GameFramework/Controller.h"

AAFAurora::AAFAurora()
{
	PrimaryActorTick.bCanEverTick = true;

	AimYaw   = 0.f;
	AimPitch = 0.f;
	AimAlpha = 1.f;
}

void AAFAurora::StartSprint(const FInputActionValue& Value)
{
	Super::StartSprint(Value);

	bIsSprinting = true;
}

void AAFAurora::StopSprint(const FInputActionValue& Value)
{
	Super::StartSprint(Value);

	bIsSprinting = true;
}

void AAFAurora::BeginPlay()
{
	Super::BeginPlay();
}

void AAFAurora::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Controller) return;

	const FRotator ControlRot = Controller->GetControlRotation();
	const FRotator ActorRot   = GetActorRotation();
	const FRotator DeltaRot  = (ControlRot - ActorRot).GetNormalized();

	AimYaw   = DeltaRot.Yaw;
	AimPitch = FMath::Clamp(DeltaRot.Pitch, -90.f, 90.f);
	AimAlpha = 1.f;   // Idle 상태라 항상 켜둠
}
