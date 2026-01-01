#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
<<<<<<< Updated upstream
#include "AFPlayerCharacter.generated.h"

class UAFStatusEffectComponent;
class USpringArmComponent;
class UCameraComponent;
=======
#include "Types/AFGameTypes.h"
#include "AFPlayerCharacter.generated.h"

#pragma region ForwardDeclare
>>>>>>> Stashed changes
class UAnimMontage;
class UDataTable;
class UAFSkillMainWidget;
struct FInputActionValue;


#pragma endregion

#pragma region HitDirectionEnum
// Hit Direction Enum
UENUM(BlueprintType)
enum class EAFHitDir : uint8
{
	Front, 	Back, 	Left, 	Right
};
#pragma endregion
UCLASS()
class AFO_API AAFPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	// ===========================================
	// 0. 기본 설정
	// ===========================================
#pragma region InitializeSetting
public:
	AAFPlayerCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	void Tick(float DeltaTime);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void OnRep_PlayerState() override;
	void CacheEssentials();
#pragma endregion

	// ===========================================
	// 1. 핵심 컴포넌트
	// ===========================================
#pragma region MainComponent
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AF|Component")
	TObjectPtr<class USpringArmComponent> SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AF|Component")
	TObjectPtr<class UCameraComponent> Camera;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "AF|Component")
	TObjectPtr<class UAFAttributeComponent> AttributeComp; // 스탯/체력 관리
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AF | Components")
	TObjectPtr<class UAFSkillComponent> SkillComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AF|Component")
	TObjectPtr<class UAFStatusEffectComponent> StatusEffectComp;// 스킬효과 관리

public:
	// 컴포넌트 Getter
	UAFAttributeComponent* GetAttributeComponent() const { return AttributeComp; }
	UAFSkillComponent* GetSkillComponent() const { return SkillComp; }
#pragma endregion

	// ===========================================
	// 2. 이동 시스템
	// ===========================================
#pragma region Movement

protected:
	// 스프린트 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AF|Movement")
	float NormalSpeed = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AF|Movement")
	float SprintSpeedMultiplier = 1.5f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AF|Movement")
	float SprintSpeed;
	float DefaultMaxWalkSpeed;	// 캐릭터마다 다른 기본 속도를 저장
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AF|Movement")
	bool bCanSprint = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AF|Movement")
	bool bSprintHeld = false;	// 스프린트 입력 상태(누르고 있는지)
	// 입력 처리
	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	virtual void StartSprint(const FInputActionValue& Value);
	UFUNCTION()
	virtual void StopSprint(const FInputActionValue& Value);

	// 이동 잠금
	bool bMovementLocked = false;
	void LockMovement();
	void UnlockMovement();

	bool IsActuallyMoving() const; 	// 실제 이동 중인지(입력/속도 기반)

	// 버프시스템 연동
	FTimerHandle SpeedBuffTimerHandle;


public:
	// 공용 에임 오프셋 변수
	UPROPERTY(BlueprintReadOnly, Category = "AF|Aim")
	float AimYaw;

	UPROPERTY(BlueprintReadOnly, Category = "AF|Aim")
	float AimPitch;

	UPROPERTY(BlueprintReadOnly, Category = "AF|Aim")
	float AimAlpha = 1.0f;

	// 에임 오프셋 계산 공용 함수
	void UpdateAimOffset(float DeltaTime);
	void ApplySpeedBuff(float Multiplier, float Duration);

#pragma endregion

	// ===========================================
	// 3. 공격 시스템
	// ===========================================
#pragma region AttackSystem

	// 바인딩용 로컬 함수
public:
	void InputAttack();
	void InputHeavyAttack();
	void InputSkillQ();
	void InputSkillE();

protected:
	// 실제 로직 처리 공격 함수
	virtual void Attack();
	virtual void HeavyAttack();
	virtual void SkillQ();
	virtual void SkillE();

	// 서버 전송
	UFUNCTION(Server, Reliable)
	void ServerRPC_Attack();
	UFUNCTION(Server, Reliable)
	void ServerRPC_HeavyAttack();
	UFUNCTION(Server, Reliable)
	void ServerRPC_SkillQ();
	UFUNCTION(Server, Reliable)
	void ServerRPC_SkillE();

public:
	// 애니메이션 노티파이에서 호출될 공용 함수
	UFUNCTION(BlueprintCallable, Category = "AF|Combat")
	virtual void HandleOnCheckHit();
	UFUNCTION(BlueprintCallable, Category = "AF|Combat")
	virtual TArray<AActor*> HandleSkillHitCheck(float Radius, float Damage, float ForwardOffset, float RotationOffset);

#pragma endregion

	// ===========================================
	// 4. 공격 관련 함수
	// ===========================================
#pragma region AttackFunction
protected:

	// 기본 공격 (콤보)
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayComboSection(int32 ComboCount);


	// 공격 시 몽타주 재생 
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayMontage(UAnimMontage* TargetMontage);

	// 마나 소모 및 사용 가능 여부 체크
	bool CanConsumeMana(float RequiredMana);

	UFUNCTION()
	void OnCombatMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	bool IsHitReactMontage(UAnimMontage* InMontage) const;
	FString AttackAnimMontageSectionPrefix = FString(TEXT("Attack"));

	FOnMontageEnded OnMeleeAttackMontageEndedDelegate;



public:


	UFUNCTION()
	void HandleOnCheckInputAttack();

	void HandleOnCheckInputAttack_FromNotify(UAnimInstance* Anim);

#pragma endregion

	// ===========================================
	// 5. 공격 변수
	// ===========================================
#pragma region AttackVariable

protected:
	// 기본공격
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	bool bIsAttacking = false;
	int32 MaxComboCount = 3;
	int32 CurrentComboCount = 0;

	// 강공격
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	bool bIsHeavyAttacking = false;
	bool bIsAttackKeyPressed = false;

	// 스킬
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	bool bIsUsingSkill = false;

	// 피격
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hit")
	bool bIsHit = false;

	// Mana Cost
	UPROPERTY(EditAnywhere, Category = "Skill")
	float SkillEManaCost = 30.f;

	UPROPERTY(EditAnywhere, Category = "Skill")
	float SkillQManaCost = 80.f;

	// 캐싱
	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstance;
	UPROPERTY()
	TObjectPtr<class AAFPlayerController> CachedPC;
	UPROPERTY()
	TObjectPtr<class AAFPlayerState> CachedPS;


#pragma endregion

	// ===========================================
	// 6. 피격 및 사망
	// ===========================================

#pragma region Hit

		// 피격 반응
	void TriggerHitReact_FromAttacker(AActor* Attacker);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayHitReact(EAFHitDir Dir);

	// W / S 방향 (카메라 기준 Forward)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement|Directional Vectors")
	FVector ForwardDir = FVector::ZeroVector;

	// A / D 방향 (카메라 기준 Right)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement|Directional Vectors")
	FVector RightDir = FVector::ZeroVector;


<<<<<<< Updated upstream
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
	
	// Attack
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* HeavyAttackMontage;
	
	void InputHeavyAttack(const FInputActionValue& InValue);
	
	bool bIsHeavyAttacking = false;
	
	UPROPERTY(EditDefaultsOnly, Category="Hit")
	TObjectPtr<UAnimMontage> HitReactMontage_Front;

	UPROPERTY(EditDefaultsOnly, Category="Hit")
	TObjectPtr<UAnimMontage> HitReactMontage_Back;

	UPROPERTY(EditDefaultsOnly, Category="Hit")
	TObjectPtr<UAnimMontage> HitReactMontage_Left;

	UPROPERTY(EditDefaultsOnly, Category="Hit")
	TObjectPtr<UAnimMontage> HitReactMontage_Right;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hit")
	bool bIsHit = false;

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
	
	void HandleOnCheckInputAttack_FromNotify(UAnimInstance* Anim);
	
	void TriggerHitReact_FromAttacker(AActor* Attacker);

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Component")
	UAFStatusEffectComponent* StatusEffectComp; //캐릭터 스킬 효과 관리 component
	
	// 스프린트 입력 상태(누르고 있는지)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	bool bSprintHeld = false;

	// 실제 이동 중인지(입력/속도 기반)
	bool IsActuallyMoving() const;

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
	
	// 이동 잠금
	bool bMovementLocked = false;

	void LockMovement();
	void UnlockMovement();
	
	// 서버로 공격 요청을 보내는 함수 (클라이언트에서 호출)
	UFUNCTION(Server, Reliable)
	void ServerAttackRequest();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayAttackMontage();

	virtual void OnRep_PlayerState() override;
	
	// 콤보 서버 함수
	UFUNCTION(Server, Reliable)
	void Server_DoComboAttack();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayComboSection(int32 ComboCount);

	// 스킬 범위 함수
	UFUNCTION()
	void HandleSkillHitCheck(float Radius, float Damage, float RotationOffset = 0.f);

	// 아군인지 확인하는 함수
	bool IsAlly(AActor* InTargetActor);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayHitReact(EAFHitDir Dir);
	EAFHitDir CalcHitDir(AActor* Attacker) const;

	// AFPlayerCharacter.h
	
=======
	EAFHitDir CalcHitDir(AActor* Attacker) const;

public:
	// 서버에서 호출될 사망 처리 함수
	void StartDeath(AController* LastInstigator = nullptr);

protected:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDeath();

	void NotifyActorBeginOverlap(AActor* OtherActor); // 물에 빠졌을 때

public:
	// 헬퍼함수 
	bool IsDead() const;
#pragma endregion

	// ===========================================
	// 7. 데이터 관리
	// ===========================================

#pragma region DataManager
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AF | Data")
	FString CharacterKey;
	UPROPERTY(EditAnywhere, Category = "AF | Data")
	TObjectPtr<class UDataTable> StatDataTable;
	UPROPERTY(EditAnywhere, Category = "AF | Data")
	TObjectPtr<class UDataTable> SkillDataTable;

	public:
	void InitializeCharacterData(FString CharacterName); // 캐릭터 정보 초기화
	protected:
	bool IsAlly(AActor* InTargetActor); // 팀 구분

	DECLARE_MULTICAST_DELEGATE(FOnSkillDataReady);
	FOnSkillDataReady OnSkillDataReady;

	void BindUI(UAFSkillMainWidget* InMainWidget);

	// --- 로드된 데이터 저장 변수 ---
	UPROPERTY(BlueprintReadOnly, Category = "AF | Stat")
	FAFPlayerCharacterStatRow BaseStats;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "AF | Skill")
	TArray<FAFSkillInfo> CharacterSkills;

	// 데이터테이블에서 정보를 찾아오는 헬퍼 함수
	FAFSkillInfo* GetSkillInfo(FName SkillName);

	// 스킬 캐싱
	FName CachedAttackKey;
	FName CachedHeavyAttackKey;
	FName CachedSkillQKey;
	FName CachedSkillEKey;

	FAFSkillInfo* CachedAttackData = nullptr;
	FAFSkillInfo* CachedHeavyAttackData = nullptr;
	FAFSkillInfo* CachedSkillQData = nullptr;
	FAFSkillInfo* CachedSkillEData = nullptr;

	// 초기화 함수
	void CacheSkillKeys();

	public:
	// 데이터 포인터 Getter
	FAFSkillInfo* GetCachedAttackData() const { return CachedAttackData; }
	FAFSkillInfo* GetCachedHeavyAttackData() const { return CachedHeavyAttackData; }
	FAFSkillInfo* GetCachedSkillQData() const { return CachedSkillQData; }
	FAFSkillInfo* GetCachedSkillEData() const { return CachedSkillEData; }

	// 데이터 키(FName) Getter
	FName GetCachedAttackKey() const { return CachedAttackKey; }
	FName GetCachedHeavyAttackKey() const { return CachedHeavyAttackKey; }
	FName GetCachedSkillQKey() const { return CachedSkillQKey; }
	FName GetCachedSkillEKey() const { return CachedSkillEKey; }


#pragma endregion

	// ===========================================
	// 8. 애니메이션
	// ===========================================

#pragma region Animation

	UPROPERTY(EditAnywhere, Category = "AF|Animations")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, Category = "AF|Animations")
	TObjectPtr<UAnimMontage> HeavyAttackMontage;

	UPROPERTY(EditAnywhere, Category = "AF|Animations")
	TObjectPtr<UAnimMontage> SkillEMontage;

	UPROPERTY(EditAnywhere, Category = "AF|Animations")
	TObjectPtr<UAnimMontage> SkillQMontage;

	UPROPERTY(EditDefaultsOnly, Category = "AF|Animation")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "AF|Animations")
	TObjectPtr<UAnimMontage> HitReactMontage_Front;

	UPROPERTY(EditDefaultsOnly, Category = "AF|Animations")
	TObjectPtr<UAnimMontage> HitReactMontage_Back;

	UPROPERTY(EditDefaultsOnly, Category = "AF|Animations")
	TObjectPtr<UAnimMontage> HitReactMontage_Left;

	UPROPERTY(EditDefaultsOnly, Category = "AF|Animations")
	TObjectPtr<UAnimMontage> HitReactMontage_Right;

#pragma endregion

>>>>>>> Stashed changes
};