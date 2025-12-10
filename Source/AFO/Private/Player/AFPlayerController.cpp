#include "Player/AFPlayerController.h"
#include "EnhancedInputSubsystems.h"

AAFPlayerController::AAFPlayerController()
: InputMappingContext(nullptr),
MoveAction(nullptr),
JumpAction(nullptr),
LookAction(nullptr),
SprintAction(nullptr),
AttackAction(nullptr)
{
}

void AAFPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}