#include "Player/AFPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "UI/AFESCWidget.h"
#include "UI/AFInGameWidget.h"
#include "UI/AFRespawnWidget.h"
#include "UI/AFKillLogContainer.h"
#include "EnhancedInputComponent.h"
#include "UI/AFSkillMainWidget.h"


AAFPlayerController::AAFPlayerController()
: InputMappingContext(nullptr),
MoveAction(nullptr),
JumpAction(nullptr),
LookAction(nullptr),
SprintAction(nullptr),
AttackAction(nullptr),
HeavyAttackAction(nullptr),
SkillEAction(nullptr),
SkillQAction(nullptr),
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



	// InGame HUD Widget ����

	if (!IsLocalController())
	{
		return;
	}

	if (IsLocalController() && SkillMainWidgetClass)
	{
		// 형변환을 통해 생성하여 클래스 전용 함수들을 사용할 수 있게 합니다.
		SkillMainWidget = CreateWidget<UAFSkillMainWidget>(this, SkillMainWidgetClass);
		if (SkillMainWidget)
		{
			SkillMainWidget->AddToViewport();
			UE_LOG(LogTemp, Warning, TEXT("@@@ [PC] SkillMain Initialized!"));
		}
	}

	if (IsLocalController())
	{
		if (IsValid(InGameWidgetClass))
		{
			UE_LOG(LogTemp, Log, TEXT("1.InGameWidgetClass��ȿ��.(�Ҵ缺��)"));

			InGameWidget = CreateWidget<UAFInGameWidget>(this, InGameWidgetClass);

			if (IsValid(InGameWidget))
			{
				UE_LOG(LogTemp, Warning, TEXT("2. InGameWidget �ν��Ͻ� ���� ����."));
				InGameWidget->AddToViewport();
			}
			else
			{
				// CreateWidget ����: Ŭ������ �Ҵ�Ǿ����� �ν��Ͻ�ȭ ���� (�ſ� �幮 ���)
				UE_LOG(LogTemp, Error, TEXT("2. InGameWidget �ν��Ͻ� ���� ����. (CreateWidget ����)"));
			}
		}
		else
		{
			// ���� ���� ���� ����: Ŭ������ �Ҵ���� �ʾҽ��ϴ�.
			UE_LOG(LogTemp, Fatal, TEXT("Ŭ�����Ҵ����"));
		}
		}

		// ESC �޴� ���� ���� (������ ����)
		if (IsValid(ESCWidgetClass))
		{
			ESCWidget = CreateWidget<UAFESCWidget>(this, ESCWidgetClass);
			if (IsValid(ESCWidget))
			{
				ESCWidget->AddToViewport(999); // HUD ���� ǥ�õǵ��� ���� ZOrder ���
				ESCWidget->SetVisibility(ESlateVisibility::Collapsed);
				UE_LOG(LogTemp, Log, TEXT("ESCWidget ���� ����!"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ESCWidget ���� ����!"));
			}
		}


		if (IsLocalController() && KillLogContainerClass)
		{
			KillLogContainer = CreateWidget<UAFKillLogContainer>(this, KillLogContainerClass);
			if (KillLogContainer)
			{
				KillLogContainer->AddToViewport();
				UE_LOG(LogTemp, Error, TEXT("kill log container init!"));
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
		UE_LOG(LogTemp, Warning, TEXT("ESCWidget�� ��ȿ���� �ʽ��ϴ�."));
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
		// �޴� �ݱ� (Resume ���)
		ESCWidget->SetVisibility(ESlateVisibility::Collapsed);

		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}

}


void AAFPlayerController::Client_ShowRespawnWidget_Implementation(float Duration)
{
	if (RespawnWidgetClass)
	{
		CurrentRespawnWidget = CreateWidget<UAFRespawnWidget>(this, RespawnWidgetClass);
		if (CurrentRespawnWidget)
		{
			CurrentRespawnWidget->InitRespawnTimer(Duration);
			CurrentRespawnWidget->AddToViewport();
		}
	}
}

void AAFPlayerController::Client_ClearRespawnWidget_Implementation()
{
	if (CurrentRespawnWidget)
	{
		CurrentRespawnWidget->RemoveFromParent();
		CurrentRespawnWidget = nullptr;
	}
}

void AAFPlayerController::Client_ShowKillLog_Implementation(const FString& KillerName, FLinearColor KillerColor, const FString& VictimName, FLinearColor VictimColor)
{
	if (KillLogContainer)
	{
		KillLogContainer->AddKillLog(KillerName, KillerColor, VictimName, VictimColor);
		UE_LOG(LogTemp, Error, TEXT("Add kill log init!"));
	}
}


void AAFPlayerController::RefreshSkillUI(UAFSkillComponent* InSkillComp)
{
	// SkillMainWidget2가 아닌, 위에서 생성한 SkillMainWidget을 사용해야 합니다.
	if (SkillMainWidget && InSkillComp)
	{
		SkillMainWidget->UpdateAllSlotsComponent(InSkillComp);
		UE_LOG(LogTemp, Warning, TEXT("@@@ [PC] Skill UI Refreshed with New Component!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("@@@ [PC] Refresh Failed: Widget(%s), Comp(%s)"),
			SkillMainWidget ? TEXT("Valid") : TEXT("NULL"),
			InSkillComp ? TEXT("Valid") : TEXT("NULL"));
	}
}