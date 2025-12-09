#include "Player/AFPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AAFPlayerController::AAFPlayerController()
: InputMappingContext(nullptr),
MoveAction(nullptr),
LookAction(nullptr),
SprintAction(nullptr),
Attack(nullptr)
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void AAFPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Enhanced Input 설정
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	// 캐릭터의 스프링암 가져오기
	APawn* P = GetPawn();
	if (P)
	{
		SpringArm = P->FindComponentByClass<USpringArmComponent>();
	}
}

void AAFPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (!SpringArm) return;

	float MouseX, MouseY;
	GetMousePosition(MouseX, MouseY);

	int32 ViewX, ViewY;
	GetViewportSize(ViewX, ViewY);

	FVector MoveDir = FVector::ZeroVector;

	if (MouseX <= EdgeThickness)
	{
		MoveDir.Y -= 1.f;
	}

	if (MouseX >= ViewX - EdgeThickness)
	{
		MoveDir.Y += 1.f;
	}

	if (MouseY <= EdgeThickness)
	{
		MoveDir.X += 1.f;
	}

	if (MouseY >= ViewY - EdgeThickness)
	{
		MoveDir.X -= 1.f;
	}

	if (!MoveDir.IsNearlyZero())
	{
		MoveDir = MoveDir.GetClampedToMaxSize(1.0f);
		SpringArm->AddWorldOffset(MoveDir * CameraMoveSpeed * DeltaSeconds);
	}
}

void AAFPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputComponent* EI = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// 캐릭터에서 바인딩 처리하므로 여기선 공격 등 추가 입력 가능
	}
}

