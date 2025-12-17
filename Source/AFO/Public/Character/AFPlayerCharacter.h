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

	UFUNCTION()
	void HandleOnCheckHit();
	UFUNCTION()
	void HandleOnCheckInputAttack();

protected:
	void TryGotoNextCombo();
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
	virtual void StartSprint(const FInputActionValue& Value);
	UFUNCTION()
	virtual void StopSprint(const FInputActionValue& Value);
	// Q / E 스킬
	UFUNCTION()
	void SkillQ(const FInputActionValue& Value);
	UFUNCTION()
	void SkillE(const FInputActionValue& Value);
	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float NormalSpeed = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float SprintSpeedMultiplier = 1.5f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	float SprintSpeed; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement")
	bool bCanSprint = true;

	// 콤보 상태
	UPROPERTY(VisibleAnywhere, Category="Combat")
	int32 ComboIndex = 0;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	int32 MaxCombo = 3;

	// 콤보 입력 창(AnimNotify로 열림)
	bool bCanNextCombo = false;

	// 입력을 미리 눌러두면 저장(버퍼)
	bool bNextComboQueued = false;
	
public:
	void Attack();
	UFUNCTION()
	void DealDamage();
	
private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;
	
	UPROPERTY(EditAnywhere, Category="Combat")
	UAnimMontage* AttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	UInputAction* SkillQAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	UInputAction* SkillEAction;
	
protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Component")
	UAFAttributeComponent* AttributeComp; // 캐릭터 속성 관리 component



	// 서버로 공격 요청을 보내는 함수 (클라이언트에서 호출)
	UFUNCTION(Server, Reliable)
	void ServerAttackRequest();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayAttackMontage();

	virtual void OnRep_PlayerState() override;
};
