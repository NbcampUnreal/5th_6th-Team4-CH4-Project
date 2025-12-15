#include "Character/AFMage.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"

AAFMage::AAFMage()
{
	PrimaryActorTick.bCanEverTick = true;

	AimYaw   = 0.f;
	AimPitch = 0.f;
	AimAlpha = 1.f;

	// 기본 이동 속도
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
}

void AAFMage::StartSprint(const FInputActionValue& Value)
{
	Super::StartSprint(Value);
	bIsSprinting = true;
}

void AAFMage::StopSprint(const FInputActionValue& Value)
{
	Super::StopSprint(Value);
	bIsSprinting = false;
}

void AAFMage::Jump()
{
	// 마법사는 점프 불가
}

void AAFMage::StopJumping()
{
	// 마법사는 점프 불가
}

void AAFMage::BeginPlay()
{
	Super::BeginPlay();
}

void AAFMage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Controller) return;

	const FRotator ControlRot = Controller->GetControlRotation();
	const FRotator ActorRot   = GetActorRotation();
	const FRotator DeltaRot  = (ControlRot - ActorRot).GetNormalized();

	AimYaw   = DeltaRot.Yaw;
	AimPitch = FMath::Clamp(DeltaRot.Pitch, -90.f, 90.f);
	AimAlpha = 1.f;
}
