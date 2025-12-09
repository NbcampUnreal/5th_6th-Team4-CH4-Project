#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AFPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

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
	
protected:
	virtual void BeginPlay() override;
};
