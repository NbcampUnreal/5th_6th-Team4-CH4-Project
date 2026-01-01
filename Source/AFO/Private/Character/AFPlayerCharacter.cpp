#include "Character/AFPlayerCharacter.h"
#include "Components/AFAttributeComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "Animation/AFAnimInstance.h"
#include "Player/AFPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Player/AFPlayerState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/OverlapResult.h"
#include "Animation/AnimInstance.h"
#include "UI/AFHealthBarWidget.h"
#include "Blueprint/UserWidget.h"
#include <Components/WidgetComponent.h>
#include "Components/AFStatusEffectComponent.h"
#include "Gimmick/AFBuffItem.h"
#include "Components/AFSkillComponent.h"
#include "Game/AFGameMode.h"
#include "Components/CapsuleComponent.h"
#include "UI/AFSkillMainWidget.h"




// ===========================================
// 0. 기본 설정
// ===========================================
#pragma region InitializeSetting

AAFPlayerCharacter::AAFPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AttributeComp = CreateDefaultSubobject<UAFAttributeComponent>(TEXT("AttributeComp"));
	SkillComp = CreateDefaultSubobject<UAFSkillComponent>(TEXT("SkillComp"));
	StatusEffectComp = CreateDefaultSubobject<UAFStatusEffectComponent>(TEXT("StatusEffectComp"));

	AttributeComp->SetIsReplicated(true);

	NormalSpeed = 400.f;
	SprintSpeedMultiplier = 1.5f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	// 스프링암 생성
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetRelativeRotation(FRotator(-70.f, 0.f, 0.f));

	// 카메라 생성
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}
void AAFPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAFPlayerCharacter, bIsAttacking);
<<<<<<< Updated upstream

=======
	DOREPLIFETIME(AAFPlayerCharacter, bIsUsingSkill);
	DOREPLIFETIME(AAFPlayerCharacter, bIsHeavyAttacking);
>>>>>>> Stashed changes
}
void AAFPlayerCharacter::InitializeCharacterData(FString CharacterName)
{
	if (CharacterName.IsEmpty() || CharacterName.Equals(TEXT("Unknown"))) return;

	CharacterKey = CharacterName;

	// 1. 스탯 초기화
	//InitializeData();

	// 2. 스킬 컴포넌트에 데이터 테이블 정보 주입
	if (SkillComp && SkillDataTable)
	{
		// InternalSkillTable을 설정해두면 나중에 GetSkillData가 실패해도 테이블에서 찾아옵니다.
		// SkillComp에 InternalSkillTable Setter가 없다면 직접 접근하거나 멤버를 수정하세요.
		// SkillComp->InternalSkillTable = SkillDataTable; 

		SkillComp->InitializeSkills(CharacterKey, SkillDataTable);
	}

	// 3. 캐싱 실행 (이제 GetSkillData가 내부 테이블을 참조하여 성공함)
	CacheSkillKeys();
}
void AAFPlayerCharacter::BindUI(UAFSkillMainWidget* InMainWidget)
{
	if (!InMainWidget) return;

	// 1. 신호가 오면 UI를 갱신하도록 예약 (람다 활용)
	OnSkillDataReady.AddLambda([this, InMainWidget]()
		{
			if (InMainWidget)
			{
				TArray<FAFSkillInfo> Skills;
				TArray<FName> Names;

				// 데이터 수집 (캐싱된 데이터들을 배열로 모음)
				if (CachedAttackData) { Skills.Add(*CachedAttackData); Names.Add(CachedAttackKey); }
				if (CachedHeavyAttackData) { Skills.Add(*CachedHeavyAttackData); Names.Add(CachedHeavyAttackKey); }
				if (CachedSkillQData) { Skills.Add(*CachedSkillQData); Names.Add(CachedSkillQKey); }
				if (CachedSkillEData) { Skills.Add(*CachedSkillEData); Names.Add(CachedSkillEKey); }

				// UI 갱신 함수 호출!
				InMainWidget->UpdateSkillSlots(Skills, Names);
				InMainWidget->UpdateAllSlotsComponent(this->SkillComp);

				UE_LOG(LogTemp, Warning, TEXT("@@@ [Character] UI Update Triggered by OnSkillDataReady!"));
			}
		});

	// 2. 만약 이미 데이터가 준비된 상태에서 바인딩된 것이라면 즉시 한 번 호출
	if (CachedAttackData != nullptr)
	{
		OnSkillDataReady.Broadcast();
	}
}
void AAFPlayerCharacter::CacheSkillKeys()
{
	if (!SkillComp)
	{
		UE_LOG(LogTemp, Error, TEXT("@@@ [CacheSkillKeys] SkillComp is NULL!"));
		return;
	}

	FString BaseName = CharacterKey;
	UE_LOG(LogTemp, Warning, TEXT("@@@ [CacheSkillKeys] Starting Cache for: %s"), *BaseName);

	// 람다 함수 수정: 데이터 포인터뿐만 아니라 Key(FName)도 함께 저장해야 함
	auto TryCache = [&](FString Suffix, FName& OutKey, FAFSkillInfo*& OutData) {
		OutKey = FName(*(BaseName + TEXT("_") + Suffix)); // 1. 열쇠 이름 저장 (이게 없어서 UI가 None으로 뜸)
		OutData = SkillComp->GetSkillData(OutKey);      // 2. 데이터 테이블에서 정보 찾기

		if (OutData) {
			UE_LOG(LogTemp, Log, TEXT("@@@ [Cache Success] Key: %s, Data Found"), *OutKey.ToString());
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("@@@ [Cache Fail] Key: %s, No Data in SkillComp"), *OutKey.ToString());
		}
		};

	// 변수 이름은 헤더에 선언하신 것에 맞춰 수정하세요 (보통 CachedAttackKey 등)
	TryCache(TEXT("Left"), CachedAttackKey, CachedAttackData);
	TryCache(TEXT("Right"), CachedHeavyAttackKey, CachedHeavyAttackData);
	TryCache(TEXT("Q"), CachedSkillQKey, CachedSkillQData);
	TryCache(TEXT("E"), CachedSkillEKey, CachedSkillEData);

	if (OnSkillDataReady.IsBound())
	{
		OnSkillDataReady.Broadcast();
	}
}

void AAFPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

<<<<<<< Updated upstream
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
=======
	// [로그 추가] 현재 캐릭터의 이름과 CharacterKey 확인
	UE_LOG(LogTemp, Warning, TEXT("@@@ [BeginPlay] ActorName: %s, CharacterKey: %s"), *GetName(), *CharacterKey);

	CacheEssentials();
	if (CachedAttackData == nullptr)
	{
		CacheSkillKeys();
	}
	if (!CharacterKey.IsEmpty())
	{
		InitializeCharacterData(CharacterKey);
	}

	// 생성 시점에 이 캐릭터의 고유 속도를 저장
	if (GetCharacterMovement())
	{
		DefaultMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	}

	if (AnimInstance)
>>>>>>> Stashed changes
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &AAFPlayerCharacter::OnCombatMontageEnded);
	}

	if (GetNetMode() != NM_DedicatedServer)
	{
		// 1. 위젯 컴포넌트 찾기 (이름으로 찾거나 클래스로 찾기)
		UWidgetComponent* HPBarComp = FindComponentByClass<UWidgetComponent>();
		if (HPBarComp)
		{
			// 2. 생성된 위젯 인스턴스를 우리 C++ 클래스로 캐스팅
			UAFHealthBarWidget* HPWidget = Cast<UAFHealthBarWidget>(HPBarComp->GetUserWidgetObject());
			if (HPWidget)
			{
				// 3. ★ 여기서 깨워줘야 타이머가 돌기 시작합니다!
				HPWidget->BindToCharacter(this);
				UE_LOG(LogTemp, Warning, TEXT("[Character] Success: BindToCharacter called for %s"), *GetName());
			}
			else
			{
				// 캐스팅 실패 시 로그 (부모 클래스 설정 확인용)
				UE_LOG(LogTemp, Error, TEXT("[Character] Failed to Cast Widget for %s! Check Parent Class."), *GetName());
			}
		}
	}

}
void AAFPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAimOffset(DeltaTime);
}
void AAFPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AAFPlayerController* PlayerController = Cast<AAFPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction) { EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &AAFPlayerCharacter::Move); }
			if (PlayerController->LookAction) { EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &AAFPlayerCharacter::Look); }
			if (PlayerController->JumpAction) {
				EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &AAFPlayerCharacter::StartJump);
				EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &AAFPlayerCharacter::StopJump);
			}
			if (PlayerController->SprintAction) {
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Started, this, &AAFPlayerCharacter::StartSprint);
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed, this, &AAFPlayerCharacter::StopSprint);
			}
			if (PlayerController->SkillEAction) { EnhancedInput->BindAction(PlayerController->SkillEAction, ETriggerEvent::Started, this, &AAFPlayerCharacter::InputSkillE); }
			if (PlayerController->SkillQAction) { EnhancedInput->BindAction(PlayerController->SkillQAction, ETriggerEvent::Started, this, &AAFPlayerCharacter::InputSkillQ); }
			if (PlayerController->AttackAction) { EnhancedInput->BindAction(PlayerController->AttackAction, ETriggerEvent::Started, this, &AAFPlayerCharacter::InputAttack); }
			if (PlayerController->HeavyAttackAction) { EnhancedInput->BindAction(PlayerController->HeavyAttackAction, ETriggerEvent::Started, this, &AAFPlayerCharacter::InputHeavyAttack); }
		}
	}
}
void AAFPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AAFPlayerState* PS = Cast<AAFPlayerState>(GetPlayerState()))
	{
		FString KeyFromPS = PS->GetSelectedCharacterKey();

		// [로그 추가] 현재 상태와 들어오는 값 비교
		UE_LOG(LogTemp, Warning, TEXT("@@@ [OnRep_PS] CurrentKey: %s, NewKey: %s"), *CharacterKey, *KeyFromPS);

		// 1. 들어오는 값이 "Unknown"이면 무시합니다. (서버에서 아직 동기화가 안 된 상태임)
		if (KeyFromPS.IsEmpty() || KeyFromPS.Equals(TEXT("Unknown")))
		{
			UE_LOG(LogTemp, Warning, TEXT("@@@ [OnRep_PS] Ignored Unknown Key. Keeping current: %s"), *CharacterKey);
			return;
		}

		// 2. 이미 같은 키로 초기화되어 있다면 굳이 다시 할 필요 없습니다.
		if (CharacterKey.Equals(KeyFromPS))
		{
			return;
		}

		// 3. 진짜 새로운 키가 왔을 때만 갱신
		InitializeCharacterData(KeyFromPS);

		if (IsLocallyControlled())
		{
			// UI 리바인딩
			UWidgetComponent* HPBarComp = FindComponentByClass<UWidgetComponent>();
			if (HPBarComp && HPBarComp->GetUserWidgetObject())
			{
				if (UAFHealthBarWidget* HPWidget = Cast<UAFHealthBarWidget>(HPBarComp->GetUserWidgetObject()))
				{
					HPWidget->BindToCharacter(this);
				}
			}
		}
	}
}
void AAFPlayerCharacter::CacheEssentials()
{
	// AnimInstance 캐싱
	if (GetMesh()) { AnimInstance = GetMesh()->GetAnimInstance(); }
	if (!CachedPC) CachedPC = Cast<AAFPlayerController>(GetController());
	if (!CachedPS) CachedPS = GetPlayerState<AAFPlayerState>();
}

#pragma endregion

// ===========================================
// 2. 이동 시스템
// ===========================================
#pragma region Movement
void AAFPlayerCharacter::Move(const FInputActionValue& value)
{
	if (bMovementLocked) return;
	if (bIsUsingSkill) return;
	if (bIsHeavyAttacking) return;
	if (!Controller) return;
	
	const FVector2D MoveInput = value.Get<FVector2D>();
	if (MoveInput.IsNearlyZero()) return;

	// 컨트롤러(카메라)의 회전
	const FRotator ControlRot = Controller->GetControlRotation();
	const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

	// 카메라 기준 Forward / Right 벡터 생성
	ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
}

void AAFPlayerCharacter::StartJump(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT(" Jump "));

	if (value.Get<bool>())
	{
		Jump();
	}
}

void AAFPlayerCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void AAFPlayerCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void AAFPlayerCharacter::StartSprint(const FInputActionValue& value)
{
	if (!bCanSprint) return;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AAFPlayerCharacter::StopSprint(const FInputActionValue& value)
{
	if (!bCanSprint) return;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

<<<<<<< Updated upstream
void AAFPlayerCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == HeavyAttackMontage)
	{
		bIsAttacking = false;
		bIsHeavyAttacking = false;

		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->SetMovementMode(MOVE_Walking);
		}

		if (!bIsUsingSkill)
		{
			UnlockMovement();
		}
	}

	if (Montage == SkillEMontage || Montage == SkillQMontage)
	{
		bIsUsingSkill = false;

		if (!bIsHeavyAttacking)
		{
			UnlockMovement();
		}
	}

	if (Montage == AttackMontage)
	{
		bIsAttacking = false;
	}
	if (Montage == HitReactMontage_Front.Get() ||
		Montage == HitReactMontage_Back.Get() ||
		Montage == HitReactMontage_Left.Get() ||
		Montage == HitReactMontage_Right.Get())
	{
		bIsHit = false;
	}
}

void AAFPlayerCharacter::HandleOnCheckHit()
{
	if (!HasAuthority()) return;

	UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!Anim) return;

	UAnimMontage* CurrentMontage = Anim->GetCurrentActiveMontage();
	if (!CurrentMontage) return;

	// ★ 디버그 로그 추가: 현재 어떤 몽타주로 판정을 시도하는지 출력
	UE_LOG(LogTemp, Log, TEXT("HandleOnCheckHit Called! Montage: %s"), *CurrentMontage->GetName());

	FString MontageName = CurrentMontage->GetName();


	// 현재 어떤 몽타주가 나오느냐에 따라 판정 범위와 대미지를 다르게 설정
	if (Anim->Montage_IsPlaying(AttackMontage))
	{
		DealDamage(); // 기본 공격 (좁은 범위, 20 대미지)
	}
	else if (Anim->Montage_IsPlaying(SkillEMontage))
	{
		HandleSkillHitCheck(250.f, 40.f, 0.f); // E 스킬 (중간 범위, 40 대미지)
	}
	else if (Anim->Montage_IsPlaying(SkillQMontage))
	{
		// ★ 다단 히트 로직: 현재 애니메이션 재생 시간을 확인하여 대미지 분기
		float CurrentPos = Anim->Montage_GetPosition(SkillQMontage);

		if (CurrentPos > 1.4f)
		{
			HandleSkillHitCheck(200.f, 30.f, 90.f); // 막타: 넓은 범위, 강한 대미지
			UE_LOG(LogTemp, Warning, TEXT("Q Skill: Final Heavy Hit!"));
		}
		else
		{
			HandleSkillHitCheck(100.f, 10.f, 90.f); // 연타: 중간 범위, 약한 대미지
			UE_LOG(LogTemp, Warning, TEXT("Q Skill: Multi Hit..."));
		}
	}
}

void AAFPlayerCharacter::HandleOnCheckInputAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Handle CALLED. pressed=%d combo=%d"),
		bIsAttackKeyPressed, CurrentComboCount);

	UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	const bool bPlaying = Anim ? Anim->Montage_IsPlaying(AttackMontage) : false;
	UE_LOG(LogTemp, Warning, TEXT("IsPlaying AttackMontage=%d"), bPlaying);

	if (!bPlaying)
	{
		UE_LOG(LogTemp, Warning, TEXT("Skip Jump: AttackMontage not playing now"));
		return;
	}

	if (bIsAttackKeyPressed)
	{
		CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, MaxComboCount);

		FName NextSectionName = *FString::Printf(TEXT("%s%02d"),
			*AttackAnimMontageSectionPrefix, CurrentComboCount);

		UE_LOG(LogTemp, Warning, TEXT("JumpToSection => %s"), *NextSectionName.ToString());

		Anim->Montage_JumpToSection(NextSectionName, AttackMontage);
		bIsAttackKeyPressed = false;
	}
}

void AAFPlayerCharacter::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	UE_LOG(LogTemp, Warning, TEXT("EndAttack CALLED. Montage=%s Interrupted=%d Combo=%d"),*GetNameSafe(InMontage), bInterruped, CurrentComboCount);
	if (CurrentComboCount == 0 && !bIsNowAttacking)
	{
		return;
	}

	CurrentComboCount = 0;
	bIsAttackKeyPressed = false;
	bIsNowAttacking = false;

	if (OnMeleeAttackMontageEndedDelegate.IsBound() == true)
	{
		OnMeleeAttackMontageEndedDelegate.Unbind();
	}
}

bool AAFPlayerCharacter::IsActuallyMoving() const
{
	if (!GetCharacterMovement()) return false;

	// 속도 기반(서버/클라 공통으로 안정적)
	const float Speed2D = GetVelocity().Size2D();
	const bool bHasSpeed = Speed2D > 3.f; // 약간의 오차 제거용

	return bHasSpeed;
}

void AAFPlayerCharacter::BeginAttack()
{
	bIsNowAttacking = true;

	const float PlayedLen = PlayAnimMontage(AttackMontage, 1.f, FName("Attack01"));
	UE_LOG(LogTemp, Warning, TEXT("BeginAttack PlayAnimMontage Len=%.3f Montage=%s"),
		PlayedLen, *GetNameSafe(AttackMontage));

	if (PlayedLen <= 0.f)
	{
		UE_LOG(LogTemp, Error, TEXT("BeginAttack FAILED: montage not played (slot/animgraph 확인)"));
		bIsNowAttacking = false;
		return;
	}

	CurrentComboCount = 1;

	UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (Anim && !OnMeleeAttackMontageEndedDelegate.IsBound())
	{
		OnMeleeAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndAttack);
		Anim->Montage_SetEndDelegate(OnMeleeAttackMontageEndedDelegate, AttackMontage);
	}
}

void AAFPlayerCharacter::InputHeavyAttack(const FInputActionValue& InValue)
{
	if (GetCharacterMovement()->IsFalling()) return;
	if (bIsUsingSkill) return;
	if (!HeavyAttackMontage) return;

	// 콤보(좌클릭) 중이면 강공격 막기 (원하면 삭제 가능)
	if (CurrentComboCount > 0 || bIsNowAttacking) return;

	UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!Anim) return;

	// 1) 연타해도 재시작(칠랑말랑) 안 되게 막기
	if (bIsHeavyAttacking) return;
	if (Anim->Montage_IsPlaying(HeavyAttackMontage)) return;

	bIsAttacking = true;
	bIsHeavyAttacking = true;

	// 2) 움직이면서 사용 방지: 입력 무시 + 즉시 멈춤 + 이동모드 None
	LockMovement(); // 네가 만든 함수(StopMovementImmediately + IgnoreMoveInput)
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}

	const float Len = PlayAnimMontage(HeavyAttackMontage);
	UE_LOG(LogTemp, Warning, TEXT("HeavyAttack PlayAnimMontage Len=%.3f Montage=%s"),
		Len, *GetNameSafe(HeavyAttackMontage));

	if (Len <= 0.f)
	{
		// 재생 실패 시 원복
		bIsAttacking = false;
		bIsHeavyAttacking = false;

		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->SetMovementMode(MOVE_Walking);
		}
		UnlockMovement();
	}
}

void AAFPlayerCharacter::Attack()
{
	if (bIsAttacking) return;

	// 클라이언트에서 Server RPC 호출
	ServerAttackRequest();
}

void AAFPlayerCharacter::DealDamage()
{
	if (!HasAuthority()) return;

	FVector Center = GetActorLocation() + (GetActorForwardVector() * 120.f) + FVector(0, 0, 90.f);
	float Radius = 120.f;

	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->OverlapMultiByChannel(
		OverlapResults, Center, FQuat::Identity, ECC_Pawn, Sphere, Params
	);
	
	if (bHit)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* HitActor = Result.GetActor();
			if (HitActor && HitActor != this)
			{
				// 3. 아군 체크 로직
				if (IsAlly(HitActor)) continue;

				UAFAttributeComponent* Attr = HitActor->FindComponentByClass<UAFAttributeComponent>();
				if (Attr)
				{
					Attr->ApplyDamage(20.f, GetController());
				}
				
				if (AAFPlayerCharacter* Victim = Cast<AAFPlayerCharacter>(HitActor))
				{
					Victim->TriggerHitReact_FromAttacker(this);
				}
			}
		}
	}
}

void AAFPlayerCharacter::ServerAttackRequest_Implementation()
{
	// 서버에서만 실행됩니다.

	AAFPlayerState* PS = GetPlayerState<AAFPlayerState>();
	if (!PS)
	{
		return;
	}

	const float AttackManaCost = 5.f;

	// Mana 부족 체크는 서버에서 안전하게 처리됩니다.
	if (!PS->ConsumeMana(AttackManaCost))
	{
		return;
	}

	// Mana 충분 → 공격 실행 (서버)
	if (AttackMontage)
	{
		bIsAttacking = true;

		MulticastPlayAttackMontage();
		DealDamage();

		UE_LOG(LogTemp, Warning, TEXT("Attack 성공: Mana %.1f 소모 (서버)"), AttackManaCost);
	}
}

void AAFPlayerCharacter::MulticastPlayAttackMontage_Implementation()
{
	// 모든 클라이언트 (서버 포함)에서 모션을 재생
	PlayAnimMontage(AttackMontage);
}

=======
>>>>>>> Stashed changes
void AAFPlayerCharacter::LockMovement()
{
	if (bMovementLocked) return;
	bMovementLocked = true;

	// 이미 움직이고 있던 속도/가속 끊기
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}

	// 입력으로 인한 이동만 막기(몽타주 루트모션은 필요하면 유지 가능)
	if (AController* C = GetController())
	{
		C->SetIgnoreMoveInput(true);
	}
}

void AAFPlayerCharacter::UnlockMovement()
{
	bMovementLocked = false;

	if (AController* C = GetController())
	{
		C->SetIgnoreMoveInput(false);
	}
}

bool AAFPlayerCharacter::IsActuallyMoving() const
{
	if (!GetCharacterMovement()) return false;

	// 속도 기반(서버/클라 공통으로 안정적)
	const float Speed2D = GetVelocity().Size2D();
	const bool bHasSpeed = Speed2D > 3.f; // 약간의 오차 제거용

	return bHasSpeed;
}

void AAFPlayerCharacter::ApplySpeedBuff(float Multiplier, float Duration)
{
	if (!HasAuthority() || !GetCharacterMovement()) return;

	// 기본 속도 기준 배수 적용
	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed * Multiplier;

	// 3. 복구 타이머
	GetWorldTimerManager().SetTimer(SpeedBuffTimerHandle, [this]()
		{
			if (GetCharacterMovement())
			{
				GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
				UE_LOG(LogTemp, Log, TEXT("Speed Buff Expired!"));
			}
		}, Duration, false);
}

void AAFPlayerCharacter::UpdateAimOffset(float DeltaTime)
{
	if (!IsLocallyControlled() || !GetController()) return;

	const FRotator ControlRot = GetControlRotation();
	const FRotator ActorRot = GetActorRotation();
	const FRotator DeltaRot = (ControlRot - ActorRot).GetNormalized();

	AimYaw = DeltaRot.Yaw;
	AimPitch = DeltaRot.Pitch;
}
#pragma endregion

// ===========================================
// 3. 공격 시스템
// ===========================================
#pragma region AttackSystem

void AAFPlayerCharacter::InputAttack()
{
	if (bIsHit || bIsUsingSkill || IsDead()) return;

	if (!bIsAttacking)
	{
		// 1타 시작: 로컬 재생 후 서버 요청
		Attack();
		ServerRPC_Attack();
	}
	else
	{
		// 콤보 예약: 로컬에서 즉시 다음 섹션으로 이동 (반응성)
		HandleOnCheckInputAttack();

		// 서버에도 예약 상태 전달
		ServerRPC_Attack();
	}
}
void AAFPlayerCharacter::InputHeavyAttack()
{
	if (!CachedHeavyAttackData || (SkillComp && !SkillComp->CanUseSkill(CachedHeavyAttackKey))) return;
	if (bIsAttacking || bIsUsingSkill || bIsHit || IsDead()) return;

	HeavyAttack();
	ServerRPC_HeavyAttack();
}
void AAFPlayerCharacter::InputSkillQ()
{
	if (!CachedSkillQData || (SkillComp && !SkillComp->CanUseSkill(CachedSkillQKey))) return;

	if (!bIsUsingSkill && CanConsumeMana(CachedSkillQData->ManaCost) && !bIsHit && !IsDead())
	{
		SkillQ();
		ServerRPC_SkillQ();
	}
}
void AAFPlayerCharacter::InputSkillE()
{
	if (!CachedSkillEData || (SkillComp && !SkillComp->CanUseSkill(CachedSkillEKey))) return;

	if (!bIsUsingSkill && CanConsumeMana(CachedSkillEData->ManaCost) && !bIsHit && !IsDead())
	{
		SkillE();
		ServerRPC_SkillE();
	}
}

void AAFPlayerCharacter::Attack()
{
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
}
void AAFPlayerCharacter::HeavyAttack()
{
	if (HeavyAttackMontage)
	{
		PlayAnimMontage(HeavyAttackMontage);
	}
}
void AAFPlayerCharacter::SkillQ()
{
	if (SkillQMontage)
	{
		PlayAnimMontage(SkillQMontage);
	}
}
void AAFPlayerCharacter::SkillE()
{
	if (SkillEMontage)
	{
		PlayAnimMontage(SkillEMontage);
	}
}

void AAFPlayerCharacter::ServerRPC_Attack_Implementation()
{
	if (!bIsAttacking)
	{
		bIsAttacking = true;
		CurrentComboCount = 1;
		bIsAttackKeyPressed = false;

		Multicast_PlayComboSection(1);
	}
	else
	{
		bIsAttackKeyPressed = true;
	}
}
void AAFPlayerCharacter::ServerRPC_HeavyAttack_Implementation()
{
	// 1. 기본 상태 및 쿨타임 체크(보안)
	if (IsDead() || bIsUsingSkill || bIsHeavyAttacking || !SkillComp) return;
	if (!SkillComp->CanUseSkill(CachedHeavyAttackKey)) return;

	// 강공격은 점프 중 사용 불가 등 추가 조건
	if (GetCharacterMovement()->IsFalling()) return;

	bIsAttacking = true;
	bIsHeavyAttacking = true;

	LockMovement(); 

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}
	Multicast_PlayMontage(HeavyAttackMontage);

}
void AAFPlayerCharacter::ServerRPC_SkillE_Implementation()
{
	if (IsDead() || bIsAttacking || bIsUsingSkill || !SkillComp || !CachedSkillEData) return;

	if (SkillComp->CanUseSkill(CachedSkillEKey) && AttributeComp)
	{
		float RequiredMana = CachedSkillEData->ManaCost;
		if (AttributeComp->GetMana() >= RequiredMana)
		{
			AttributeComp->ApplyManaChange(-RequiredMana); 
			bIsUsingSkill = true;
			LockMovement();
			Multicast_PlayMontage(SkillEMontage);
		}
	}
}
void AAFPlayerCharacter::ServerRPC_SkillQ_Implementation()
{
	if (IsDead() || bIsAttacking || bIsUsingSkill || !SkillComp || !CachedSkillQData) return;

	if (SkillComp->CanUseSkill(CachedSkillQKey) && AttributeComp)
	{
		float RequiredMana = CachedSkillQData->ManaCost;
		if (AttributeComp->GetMana() >= RequiredMana)
		{
			AttributeComp->ApplyManaChange(-RequiredMana); 
			bIsUsingSkill = true;
			LockMovement();
			Multicast_PlayMontage(SkillQMontage);
		}
	}
}

void AAFPlayerCharacter::HandleOnCheckHit()
{
	if (!HasAuthority() || IsDead() || !SkillComp) return;

	FName TargetKey = NAME_None;
	FAFSkillInfo* TargetData = nullptr;

	// 1. 현재 상태에 맞는 캐싱 데이터 선택
	if (bIsHeavyAttacking)
	{
		TargetKey = CachedHeavyAttackKey;
		TargetData = CachedHeavyAttackData;
	}
	else if (bIsUsingSkill)
	{
		if (AnimInstance->Montage_IsPlaying(SkillEMontage))
		{
			TargetKey = CachedSkillEKey;
			TargetData = CachedSkillEData;
		}
		else if (AnimInstance->Montage_IsPlaying(SkillQMontage))
		{
			TargetKey = CachedSkillQKey;
			TargetData = CachedSkillQData;
		}
	}

	else if (bIsAttacking) // 일반 공격 추가
	{
		TargetKey = CachedAttackKey;
		TargetData = CachedAttackData;
	}

	if (TargetData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("@@@ [Mage] TargetData is NULL! CharacterKey: %s, IsAttacking: %d"), *CharacterKey, bIsAttacking);
		return; // 데이터가 없으면 아래 로직을 수행하지 않고 나감
	}

	// 2. 스킬/강공격 판정 및 쿨타임 시작
	if (TargetData)
	{
		HandleSkillHitCheck(TargetData->Radius, TargetData->Damage, TargetData->ForwardOffset, TargetData->RotationOffset);

		// 실제 타격 시점(또는 스킬 발동 시점)에 쿨타임 시작
		SkillComp->StartCooldown(TargetKey, TargetData->Cooldown);
		return;
	}

	// 3. 일반 공격
	if (bIsAttacking)
	{
		HandleSkillHitCheck(TargetData->Radius, TargetData->Damage, TargetData->ForwardOffset, TargetData->RotationOffset);
	}

}
TArray<AActor*> AAFPlayerCharacter::HandleSkillHitCheck(float Radius, float Damage, float ForwardOffset, float RotationOffset)
{
	TArray<AActor*> HitActors;
	if (!HasAuthority()) return HitActors;

	// 1. 캐릭터의 기본 전방 벡터
	FVector Forward = GetActorForwardVector();

	// 2. ★ 전방 벡터를 RotationOffset만큼 회전시킴
	FVector SkillDirection = Forward.RotateAngleAxis(RotationOffset, FVector(0, 0, 1));
<<<<<<< Updated upstream

	// 3. 캐릭터 위치에서 '회전된 방향'으로 Sphere를 배치
	FVector SphereLocation = GetActorLocation() + (SkillDirection * 150.f) + FVector(0, 0, 60.f);

	// 4. 디버그 구체 그리기 (위치가 맞는지 확인)
	// DrawDebugSphere(GetWorld(), SphereLocation, Radius, 16, FColor::Cyan, false, 1.0f);
=======
	// ForwardOffset을 인자로 받아 스킬마다 앞쪽으로 얼마나 뻗을지 결정
	FVector SphereLocation = GetActorLocation() + (SkillDirection * ForwardOffset) + FVector(0, 0, 60.f);
>>>>>>> Stashed changes

	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// 5. 판정 실행
	bool bHit = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		SphereLocation,
		FQuat::Identity,
		ECC_Pawn,
		Sphere,
		Params
	);
<<<<<<< Updated upstream
	
	if (bHit) return;
	{
		for (auto& Result : OverlapResults)
		{
			AActor* HitActor = Result.GetActor();
			if (HitActor)
			{
				if (HitActor == this) continue;     // 자기 자신 제외
				if (IsAlly(HitActor)) continue;     // 아군 제외
				
				UAFAttributeComponent* Attr = HitActor->FindComponentByClass<UAFAttributeComponent>();
				if (Attr)
				{
					Attr->ApplyDamage(Damage, GetController());
					UE_LOG(LogTemp, Warning, TEXT("Skill Hit! -> %s"), *HitActor->GetName());
				}
				
				if (AAFPlayerCharacter* Victim = Cast<AAFPlayerCharacter>(HitActor))
				{
					Victim->TriggerHitReact_FromAttacker(this);
				}
				// 슬로우 적용 
				if (UAFStatusEffectComponent* StatusComp = HitActor->FindComponentByClass<UAFStatusEffectComponent>())
				{
					StatusComp->ApplySlow(0.2f, 0.5f);
				}
			}
		}
=======
	// 디버그용 구체 그리기 (개발 단계에서 필수!)
	DrawDebugSphere(GetWorld(), SphereLocation, Radius, 12, FColor::Red, false, 1.0f);

	if (!bHit) return HitActors;

	for (const FOverlapResult& Result : OverlapResults)
	{
		AActor* HitActor = Result.GetActor();
		if (!HitActor || HitActor == this || IsAlly(HitActor)) continue;

		if (UAFAttributeComponent* Attr = HitActor->FindComponentByClass<UAFAttributeComponent>())
		{
			Attr->ApplyDamage(Damage, GetController());
			HitActors.Add(HitActor); // 맞은 플레이어 리스트
		}

		if (AAFPlayerCharacter* Victim = Cast<AAFPlayerCharacter>(HitActor))
		{
			Victim->TriggerHitReact_FromAttacker(this);
		}
	}
	return HitActors; // 리스트 반환
}
#pragma endregion

// ===========================================
// 4. 공격 관련 함수
// ===========================================
#pragma region AttackFunction

void AAFPlayerCharacter::Multicast_PlayMontage_Implementation(UAnimMontage* TargetMontage)
{
	PlayAnimMontage(TargetMontage);
}

void AAFPlayerCharacter::Multicast_PlayComboSection_Implementation(int32 ComboCount)
{

	if (ComboCount == 1)
	{
		PlayAnimMontage(AttackMontage, 1.f, FName("Attack01"));

		// 종료 시점 리셋을 위한 델리게이트
		OnMeleeAttackMontageEndedDelegate.BindUObject(this, &AAFPlayerCharacter::OnCombatMontageEnded);
		AnimInstance->Montage_SetEndDelegate(OnMeleeAttackMontageEndedDelegate, AttackMontage);
	}
	else
	{
		// 콤보 섹션 점프
		FName NextSectionName = *FString::Printf(TEXT("%s%02d"), *AttackAnimMontageSectionPrefix, ComboCount);
		AnimInstance->Montage_JumpToSection(NextSectionName, AttackMontage);
	}
}
void AAFPlayerCharacter::HandleOnCheckInputAttack()
{
	const bool bPlaying = AnimInstance ? AnimInstance->Montage_IsPlaying(AttackMontage) : false;

	if (!bPlaying)
	{
		UE_LOG(LogTemp, Warning, TEXT("Skip Jump: AttackMontage not playing now"));
		return;
	}

	if (bIsAttackKeyPressed)
	{
		CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, MaxComboCount);

		FName NextSectionName = *FString::Printf(TEXT("%s%02d"),
			*AttackAnimMontageSectionPrefix, CurrentComboCount);

		UE_LOG(LogTemp, Warning, TEXT("JumpToSection => %s"), *NextSectionName.ToString());

		AnimInstance->Montage_JumpToSection(NextSectionName, AttackMontage);
		bIsAttackKeyPressed = false;
	}
}
void AAFPlayerCharacter::HandleOnCheckInputAttack_FromNotify(UAnimInstance* Anim)
{
	if (!HasAuthority()) return;

	if (bIsAttackKeyPressed)
	{
		bIsAttackKeyPressed = false;
		CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, MaxComboCount);

		Multicast_PlayComboSection(CurrentComboCount);
>>>>>>> Stashed changes
	}
}

void AAFPlayerCharacter::OnCombatMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 0. 방어 코드 및 공용 언바인드
	if (!Montage || !AnimInstance) return;
	AnimInstance->OnMontageEnded.RemoveDynamic(this, &AAFPlayerCharacter::OnCombatMontageEnded);

	// 1. 일반 공격 (콤보)
	if (Montage == AttackMontage)
	{
		bIsAttacking = false;
		CurrentComboCount = 0;
		bIsAttackKeyPressed = false;
		return; // 찾았으면 바로 나감 (성능 최적화)
	}

	// 2. 강공격
	if (Montage == HeavyAttackMontage)
	{
		bIsAttacking = false;
		bIsHeavyAttacking = false;
		UnlockMovement();
		return;
	}

	// 3. 스킬 (Q, E)
	if (Montage == SkillQMontage || Montage == SkillEMontage)
	{
		bIsUsingSkill = false;
		UnlockMovement();
		return;
	}

	// 4. 피격 (Hit React)
	if (IsHitReactMontage(Montage))
	{
		bIsHit = false;
		return;
	}
}

bool AAFPlayerCharacter::IsHitReactMontage(UAnimMontage* InMontage) const
{
	// 피격 몽타주 여부를 체크하는 헬퍼 함수 (가독성 향상)
	return
		(InMontage == HitReactMontage_Front.Get() ||
			InMontage == HitReactMontage_Back.Get() ||
			InMontage == HitReactMontage_Left.Get() ||
			InMontage == HitReactMontage_Right.Get());
}

bool AAFPlayerCharacter::CanConsumeMana(float RequiredMana)
{
	if (AttributeComp)
	{
		// AttributeComp에 마나 소모 가능 여부를 묻거나 직접 확인
		return AttributeComp->GetMana() >= RequiredMana;
	}
	return false;
}

#pragma endregion

// ===========================================
// 6. 피격 및 사망
// ===========================================

#pragma region Hit

void AAFPlayerCharacter::TriggerHitReact_FromAttacker(AActor* Attacker)
{
	if (!HasAuthority()) return;
	if (bIsHit) return;

	const EAFHitDir Dir = CalcHitDir(Attacker);

	bIsHit = true;
	Multicast_PlayHitReact(Dir);
}

void AAFPlayerCharacter::Multicast_PlayHitReact_Implementation(EAFHitDir Dir)
{
	TObjectPtr<UAnimMontage> Montage = nullptr;

	switch (Dir)
	{
	case EAFHitDir::Front: Montage = HitReactMontage_Front; break;
	case EAFHitDir::Back:  Montage = HitReactMontage_Back;  break;
	case EAFHitDir::Left:  Montage = HitReactMontage_Left;  break;
	case EAFHitDir::Right: Montage = HitReactMontage_Right; break;
	}

	if (!Montage) return;

	if (UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
	{
		Anim->StopAllMontages(0.05f); // 맞으면 스킬/공격 끊기
	}

	bIsUsingSkill = false;
	bIsAttacking = false;
	bIsHeavyAttacking = false;

	UnlockMovement();

	PlayAnimMontage(Montage.Get(), 1.f);
}

EAFHitDir AAFPlayerCharacter::CalcHitDir(AActor* Attacker) const
{
	if (!Attacker) return EAFHitDir::Front;

	FVector Dir = Attacker->GetActorLocation() - GetActorLocation(); // 공격자 - 피해자
	Dir.Z = 0.f;

	if (Dir.IsNearlyZero()) return EAFHitDir::Front;
	Dir.Normalize();

	const float F = FVector::DotProduct(GetActorForwardVector(), Dir);
	const float R = FVector::DotProduct(GetActorRightVector(), Dir);

	if (FMath::Abs(F) >= FMath::Abs(R))
	{
		return (F >= 0.f) ? EAFHitDir::Front : EAFHitDir::Back;
	}
	return (R >= 0.f) ? EAFHitDir::Right : EAFHitDir::Left;
}

<<<<<<< Updated upstream
=======
void AAFPlayerCharacter::StartDeath(AController* LastInstigator)
{
	if (!HasAuthority() || IsDead()) return;

	// 1. 서버에서 게임모드에 알림 (점수/리스폰 처리)
	if (AAFGameMode* GM = GetWorld()->GetAuthGameMode<AAFGameMode>())
	{
		// LastInstigator는 나를 죽인 사람 (낙사면 nullptr)
		GM->HandlePlayerDeath(GetController(), LastInstigator);
	}

	// 2. 모든 클라이언트에게 사망 연출 명령
	Multicast_OnDeath();

}

void AAFPlayerCharacter::Multicast_OnDeath_Implementation()
{
	// 애니메이션 재생 (사망 몽타주)
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}

	// 전투 중이던 플래그들 정리
	bIsAttacking = false;
	bIsUsingSkill = false;

	// 이동 및 콜리전 정지
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AAFPlayerCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (HasAuthority() && OtherActor && OtherActor->ActorHasTag(TEXT("WaterFloor")))
	{
		// 낙사 (가해자 nullptr)
		StartDeath(nullptr);
	}
}

bool AAFPlayerCharacter::IsDead() const 
{ return AttributeComp ? AttributeComp->IsDead() : true; }

#pragma endregion

// ===========================================
// 7. 데이터 관리
// ===========================================
#pragma region DataManager



bool AAFPlayerCharacter::IsAlly(AActor* InTargetActor)
{
	if (!InTargetActor) return false;

	APawn* TargetPawn = Cast<APawn>(InTargetActor);
	if (!TargetPawn) return false;

	AAFPlayerState* MyPS = Cast<AAFPlayerState>(GetPlayerState());
	AAFPlayerState* TargetPS = Cast<AAFPlayerState>(TargetPawn->GetPlayerState());

	if (MyPS && TargetPS)
	{
		return MyPS->GetTeamID() == TargetPS->GetTeamID();
	}

	return false;
}





#pragma endregion
>>>>>>> Stashed changes




