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
	
protected:
	virtual void BeginPlay() override;


	// === InGameUI ===  //



	// 인게임 HUD 위젯 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AFO|Widgets")
	TSubclassOf<UAFInGameWidget> InGameWidgetClass;

	// ESC 메뉴 위젯 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AFO|Widgets")
	TSubclassOf<UAFESCWidget> ESCWidgetClass;

	// 생성된 인게임 HUD 인스턴스
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AFO|Widgets")
	TObjectPtr<UAFInGameWidget> InGameWidget;

	// 생성된 ESC 메뉴 인스턴스
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AFO|Widgets")
	TObjectPtr<UAFESCWidget> ESCWidget;

public:
	// ESC 메뉴 활성화 상태
	UPROPERTY(VisibleInstanceOnly, Category = "AFO|Widgets")
	bool bIsESCMenuOpen = false;

	// ESC 메뉴 토글 함수
	void ToggleESCMenu();

private:
	// 입력 컴포넌트 설정
	virtual void SetupInputComponent() override;
};
