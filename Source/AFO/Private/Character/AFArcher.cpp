#include "Character/AFArcher.h"
#include "GameFramework/Controller.h"

AAFArcher::AAFArcher()
{
	PrimaryActorTick.bCanEverTick = true;

	AimYaw = 0.f;
	AimPitch = 0.f;
	AimAlpha = 0.f;
}

void AAFArcher::BeginPlay()
{
	Super::BeginPlay();
}

void AAFArcher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Controller) return;

	// 컨트롤러 회전 (카메라 기준)
	const FRotator ControlRot = Controller->GetControlRotation();
	// 캐릭터 기준 회전
	const FRotator ActorRot = GetActorRotation();

	// 차이 계산
	const FRotator DeltaRot = (ControlRot - ActorRot).GetNormalized();

	AimYaw = DeltaRot.Yaw;
	AimPitch = DeltaRot.Pitch;

	// 임시: 항상 AimOffset 켜기
	// 나중에 활 조준 상태 변수로 교체하면 됨
	AimAlpha = 1.f;
}