#include "Player/AFPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "UI/AFESCWidget.h"
#include "UI/AFInGameWidget.h"
#include "EnhancedInputComponent.h"


AAFPlayerController::AAFPlayerController()
: InputMappingContext(nullptr),
MoveAction(nullptr),
JumpAction(nullptr),
LookAction(nullptr),
SprintAction(nullptr),
AttackAction(nullptr),
ESC(nullptr)
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



	// InGame HUD Widget 생성

	if (!IsLocalController())
	{
		return;
	}

	if (IsLocalController())
	{
		if (IsValid(InGameWidgetClass))
		{
			UE_LOG(LogTemp, Log, TEXT("1.InGameWidgetClass유효함.(할당성공)"));

			InGameWidget = CreateWidget<UAFInGameWidget>(this, InGameWidgetClass);

			if (IsValid(InGameWidget))
			{
				UE_LOG(LogTemp, Warning, TEXT("2. InGameWidget 인스턴스 생성 성공."));
				InGameWidget->AddToViewport();
			}
			else
			{
				// CreateWidget 실패: 클래스는 할당되었으나 인스턴스화 실패 (매우 드문 경우)
				UE_LOG(LogTemp, Error, TEXT("2. InGameWidget 인스턴스 생성 실패. (CreateWidget 실패)"));
			}
		}
		else
		{
			// 가장 흔한 실패 원인: 클래스가 할당되지 않았습니다.
			UE_LOG(LogTemp, Fatal, TEXT("클래스할당실패"));
		}
		}

		// ESC 메뉴 위젯 생성 (숨겨진 상태)
		if (IsValid(ESCWidgetClass))
		{
			ESCWidget = CreateWidget<UAFESCWidget>(this, ESCWidgetClass);
			if (IsValid(ESCWidget))
			{
				ESCWidget->AddToViewport(999); // HUD 위에 표시되도록 높은 ZOrder 사용
				ESCWidget->SetVisibility(ESlateVisibility::Collapsed);
				UE_LOG(LogTemp, Log, TEXT("ESCWidget 생성 성공!"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ESCWidget 생성 실패!"));
			}
		}
	}


void AAFPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(ESC, ETriggerEvent::Started, this, &AAFPlayerController::ToggleESCMenu);
	}
}

void AAFPlayerController::ToggleESCMenu()
{
	if (!IsValid(ESCWidget))
	{
		UE_LOG(LogTemp, Warning, TEXT("ESCWidget이 유효하지 않습니다."));
		return;
	}

	bIsESCMenuOpen = !bIsESCMenuOpen;

	if (bIsESCMenuOpen)
	{
		ESCWidget->SetVisibility(ESlateVisibility::Visible);

		SetInputMode(FInputModeGameAndUI());
		bShowMouseCursor = true;
	}
	else
	{
		// 메뉴 닫기 (Resume 기능)
		ESCWidget->SetVisibility(ESlateVisibility::Collapsed);

		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}

}