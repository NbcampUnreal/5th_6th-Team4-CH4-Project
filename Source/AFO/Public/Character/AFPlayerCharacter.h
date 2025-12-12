#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "AFPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;
struct FInputActionValue;
class UAFAttributeComponent;

UCLASS()
class AFO_API AAFPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAFPlayerCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Move(const FInputActionValue& value);          // 이동구현 앞뒤좌우
	UFUNCTION()
	void StartJump(const FInputActionValue& value);     // 점프 시작
	UFUNCTION()
	void StopJump(const FInputActionValue& value);      // 점프 종료
	UFUNCTION()
	void Look(const FInputActionValue& value);          // 마우스 시야회전
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);   // 달리기 시작
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);    // 달리기 끝
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	bool bIsAttacking = false;
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	// W : Forward Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Directional Speed")
	float ForwardSpeed = 1.0f;

	// S : Backward Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Directional Speed")
	float BackwardSpeed = 0.65f;

	// D : Right Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Directional Speed")
	float RightSpeed = 0.72f;

	// A : Left Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Directional Speed")
	float LeftSpeed = 0.72f;
	
	// W / S 방향 (카메라 기준 Forward)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Movement|Directional Vectors")
	FVector ForwardDir = FVector::ZeroVector;

	// A / D 방향 (카메라 기준 Right)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Movement|Directional Vectors")
	FVector RightDir = FVector::ZeroVector;
	
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
	UInputAction* JumpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	UInputAction* SprintAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	UInputAction* AttackAction;

	
	UPROPERTY(EditAnywhere, Category="Camera")
	float CameraPanSpeed = 2000.f;
	
protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Component")
	UAFAttributeComponent* AttributeComp; // 캐릭터 속성 관리 component

};
