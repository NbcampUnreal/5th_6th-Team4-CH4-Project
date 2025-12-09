#include "Player/AFPlayerController.h"
#include "EnhancedInputSubsystems.h"

AAFPlayerController::AAFPlayerController()
: InputMappingContext(nullptr),
MoveAction(nullptr),
LookAction(nullptr),
SprintAction(nullptr),
Attack(nullptr)
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

