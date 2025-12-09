#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AFPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;

UCLASS()
class AFO_API AAFPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AAFPlayerController();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* SprintAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* Attack;
	
protected:
	virtual void SetupInputComponent() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float EdgeThickness = 20.f;     // 에지 감지 두께

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float CameraMoveSpeed = 1500.f; // 카메라 이동 속도

	UPROPERTY()
	USpringArmComponent* SpringArm; // 캐릭터에서 가져옴
};
