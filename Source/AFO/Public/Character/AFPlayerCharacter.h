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

	UFUNCTION(Server, Reliable)
	void ServerRPC_SkillE();
	UFUNCTION(Server, Reliable)
	void ServerRPC_SkillQ();

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
	virtual void StartSprint(const FInputActionValue& Value);
	UFUNCTION()
	virtual void StopSprint(const FInputActionValue& Value);


	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	bool bIsAttacking = false;
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// W : Forward Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Directional Speed")
	float ForwardSpeed = 1.0f;

	// S : Backward Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Directional Speed")
	float BackwardSpeed = 0.65f;

	// D : Right Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Directional Speed")
	float RightSpeed = 0.72f;

	// A : Left Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Directional Speed")
	float LeftSpeed = 0.72f;

	// W / S 방향 (카메라 기준 Forward)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement|Directional Vectors")
	FVector ForwardDir = FVector::ZeroVector;

	// A / D 방향 (카메라 기준 Right)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement|Directional Vectors")
	FVector RightDir = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float NormalSpeed = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeedMultiplier = 1.5f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	bool bCanSprint = true;

	// Skill Montages
	UPROPERTY(EditAnywhere, Category = "Skill")
	UAnimMontage* SkillEMontage;

	UPROPERTY(EditAnywhere, Category = "Skill")
	UAnimMontage* SkillQMontage;

	// Mana Cost
	UPROPERTY(EditAnywhere, Category = "Skill")
	float SkillEManaCost = 30.f;

	UPROPERTY(EditAnywhere, Category = "Skill")
	float SkillQManaCost = 80.f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	bool bIsUsingSkill = false;

public:
	void Attack();

	UFUNCTION()
	void DealDamage();

	UFUNCTION()
	void HandleOnCheckHit();

	UFUNCTION()
	void HandleOnCheckInputAttack();

	virtual void BeginAttack();

	UFUNCTION()
	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped);

private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* AttackMontage;

	void InputAttackMelee(const FInputActionValue& InValue);

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UAFAttributeComponent* AttributeComp; // 캐릭터 속성 관리 component

	FString AttackAnimMontageSectionPrefix = FString(TEXT("Attack"));

	int32 MaxComboCount = 3;

	int32 CurrentComboCount = 0;

	bool bIsNowAttacking = false;

	bool bIsAttackKeyPressed = false;

	FOnMontageEnded OnMeleeAttackMontageEndedDelegate;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySkillEMontage();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySkillQMontage();



	// 서버로 공격 요청을 보내는 함수 (클라이언트에서 호출)
	UFUNCTION(Server, Reliable)
	void ServerAttackRequest();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayAttackMontage();

	virtual void OnRep_PlayerState() override;
};