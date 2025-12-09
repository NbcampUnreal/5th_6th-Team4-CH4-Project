#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "AFPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;
struct FInputActionValue;

UCLASS()
class AFO_API AAFPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAFPlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION() // 언리얼 엔진의 함수 리플렉션 매크로 !
	void Move(const FInputActionValue& value);          // 이동구현 앞뒤좌우
	UFUNCTION()
	void Look(const FInputActionValue& value);          // 마우스 시야회전
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);   // 달리기 시작
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);    // 달리기 끝
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsAttacking = false;
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
public:
	void Attack();
	UFUNCTION()
	void DealDamage();
	
private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;
	
	float NormalSpeed;                                  // 기본 이동속도
	float SprintSpeedMultiplier;                        // 달리기 속도(곱해줄 값)
	float SprintSpeed;                                  // 실제 달리기 속도 ( 기본 속도 * 계산된 값 )
	float LookSensitive;                                // 마우스 민감도
	
	UPROPERTY(EditAnywhere, Category="Combat")
	UAnimMontage* AttackMontage;
	
	// 입력 액션들
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	UInputAction* SprintAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	UInputAction* AttackAction;
	
	UPROPERTY(EditAnywhere, Category="Camera")
	float CameraPanSpeed = 2000.f;
};
