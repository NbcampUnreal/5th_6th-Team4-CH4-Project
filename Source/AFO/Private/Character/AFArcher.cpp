#include "Character/AFArcher.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"

AAFArcher::AAFArcher()
{
	PrimaryActorTick.bCanEverTick = true;
	bCanSprint = false;

	AimYaw = 0.f;
	AimPitch = 0.f;
	AimAlpha = 1.f;
	
	// 기본 이동 속도 설정
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

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