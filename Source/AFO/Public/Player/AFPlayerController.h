#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AFPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UAFInGameWidget;  	// InGameUI
class UAFESCWidget;          	// InGameUI

UCLASS()
class AFO_API AAFPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AAFPlayerController();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* SprintAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* ESC;   	// InGameUI
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* SkillEAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* SkillQAction;
	
protected:
	virtual void BeginPlay() override;


	// === InGameUI ===  //



	// �ΰ��� HUD ���� Ŭ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AFO|Widgets")
	TSubclassOf<UAFInGameWidget> InGameWidgetClass;

	// ESC �޴� ���� Ŭ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AFO|Widgets")
	TSubclassOf<UAFESCWidget> ESCWidgetClass;

	// ������ �ΰ��� HUD �ν��Ͻ�
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AFO|Widgets")
	TObjectPtr<UAFInGameWidget> InGameWidget;

	// ������ ESC �޴� �ν��Ͻ�
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AFO|Widgets")
	TObjectPtr<UAFESCWidget> ESCWidget;

public:
	// ESC �޴� Ȱ��ȭ ����
	UPROPERTY(VisibleInstanceOnly, Category = "AFO|Widgets")
	bool bIsESCMenuOpen = false;

	// ESC �޴� ��� �Լ�
	void ToggleESCMenu();

private:
	// �Է� ������Ʈ ����
	virtual void SetupInputComponent() override;
};
