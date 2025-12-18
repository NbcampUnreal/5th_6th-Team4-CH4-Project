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
#include "Animation/AnimInstance.h"

AAFPlayerCharacter::AAFPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AttributeComp = CreateDefaultSubobject<UAFAttributeComponent>(TEXT("AttributeComponent"));

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

}

void AAFPlayerCharacter::ServerRPC_SkillE_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Server: SkillE Input received"));

	if (bIsAttacking || bIsUsingSkill) return;

	AAFPlayerState* PS = GetPlayerState<AAFPlayerState>();
	if (!PS) return;

	if (!PS->ConsumeMana(SkillEManaCost))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillE 실패: 서버에서 Mana 부족 확인"));
		return;
	}

	if (SkillEMontage)
	{
		bIsUsingSkill = true;
		// 서버에서 몽타주를 재생하면 설정에 따라 클라이언트들에게 복제
		Multicast_PlaySkillEMontage();
	}
}

void AAFPlayerCharacter::ServerRPC_SkillQ_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Server: SkillQ Input received"));

	if (bIsAttacking || bIsUsingSkill) return;

	AAFPlayerState* PS = GetPlayerState<AAFPlayerState>();
	if (!PS) return;

	if (!PS->ConsumeMana(SkillQManaCost))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillE 실패: 서버에서 Mana 부족 확인"));
		return;
	}

	if (SkillQMontage)
	{
		bIsUsingSkill = true;
		// 서버에서 몽타주를 재생하면 설정에 따라 클라이언트들에게 복제
		Multicast_PlaySkillQMontage();
	}
}

void AAFPlayerCharacter::Multicast_PlaySkillEMontage_Implementation()
{
	// 서버와 모든 클라이언트에서 동시에 실행됨
	if (SkillEMontage)
	{
		bIsUsingSkill = true;
		
		LockMovement();
		PlayAnimMontage(SkillEMontage);
	}
}

void AAFPlayerCharacter::Multicast_PlaySkillQMontage_Implementation()
{
	// 서버와 모든 클라이언트에서 동시에 실행됨
	if (SkillQMontage)
	{
		bIsUsingSkill = true;
		
		LockMovement();  
		PlayAnimMontage(SkillQMontage);
	}
}

void AAFPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &AAFPlayerCharacter::OnAttackMontageEnded);
	}
}

void AAFPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AAFPlayerController* PlayerController = Cast<AAFPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&AAFPlayerCharacter::Move
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AAFPlayerCharacter::Look
				);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&AAFPlayerCharacter::StartJump
				);

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&AAFPlayerCharacter::StopJump
				);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Started,
					this,
					&AAFPlayerCharacter::StartSprint
				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&AAFPlayerCharacter::StopSprint
				);
			}

			if (PlayerController->SkillEAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SkillEAction,
					ETriggerEvent::Started,
					this,
					&AAFPlayerCharacter::ServerRPC_SkillE
				);
			}

			if (PlayerController->SkillQAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SkillQAction,
					ETriggerEvent::Started,
					this,
					&AAFPlayerCharacter::ServerRPC_SkillQ
				);
			}
			
			if (PlayerController->AttackAction)
			{
				EnhancedInput->BindAction(
					PlayerController->AttackAction,
					ETriggerEvent::Started,
					this,
					&ThisClass::InputAttackMelee
				);
			}
			
			if (PlayerController->HeavyAttackAction)
			{
				EnhancedInput->BindAction(
					PlayerController->HeavyAttackAction,
					ETriggerEvent::Started,
					this,
					&ThisClass::InputHeavyAttack
				);
			}
		}
	}
}

void AAFPlayerCharacter::Move(const FInputActionValue& value)
{
	if (bMovementLocked) return;
	if (bIsUsingSkill) return;
	if (bIsHeavyAttacking) return;
	if (!Controller) return;

	const FVector2D MoveInput = value.Get<FVector2D>();
	if (MoveInput.IsNearlyZero()) return;

	// W / S 처리 (MoveInput.Y)
	if (MoveInput.X > 0.f)
	{
		// W
		AddMovementInput(ForwardDir, MoveInput.X * ForwardSpeed);
	}
	else if (MoveInput.X < 0.f)
	{
		// S
		AddMovementInput(ForwardDir, MoveInput.X * BackwardSpeed);
	}

	// A / D 처리 (MoveInput.X)
	if (MoveInput.Y > 0.f)
	{
		// D
		AddMovementInput(RightDir, MoveInput.Y * RightSpeed);
	}
	else if (MoveInput.Y < 0.f)
	{
		// A
		AddMovementInput(RightDir, MoveInput.Y * LeftSpeed);
	}

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
}

void AAFPlayerCharacter::HandleOnCheckHit()
{
	UKismetSystemLibrary::PrintString(this, TEXT("HandleOnCheckHit()"));
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
	ensureMsgf(CurrentComboCount != 0, TEXT("CurrentComboCount == 0"));

	CurrentComboCount = 0;
	bIsAttackKeyPressed = false;
	bIsNowAttacking = false;

	if (OnMeleeAttackMontageEndedDelegate.IsBound() == true)
	{
		OnMeleeAttackMontageEndedDelegate.Unbind();
	}
}

void AAFPlayerCharacter::HandleOnCheckInputAttack_FromNotify(UAnimInstance* Anim)
{
	UE_LOG(LogTemp, Warning, TEXT("HandleFromNotify CALLED pressed=%d combo=%d"), bIsAttackKeyPressed, CurrentComboCount);

	if (!Anim || !AttackMontage) return;

	const bool bPlaying = Anim->Montage_IsPlaying(AttackMontage);
	UE_LOG(LogTemp, Warning, TEXT("IsPlaying(OnNotifyAnim)=%d CurrentActive=%s"),
		bPlaying,
		*GetNameSafe(Anim->GetCurrentActiveMontage()));

	if (!bPlaying) return;

	if (bIsAttackKeyPressed)
	{
		CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, MaxComboCount);
		FName NextSectionName = *FString::Printf(TEXT("%s%02d"), *AttackAnimMontageSectionPrefix, CurrentComboCount);

		UE_LOG(LogTemp, Warning, TEXT("JumpToSection => %s"), *NextSectionName.ToString());

		Anim->Montage_JumpToSection(NextSectionName, AttackMontage);
		bIsAttackKeyPressed = false;
	}
}

void AAFPlayerCharacter::InputAttackMelee(const FInputActionValue& InValue)
{
	UE_LOG(LogTemp, Warning, TEXT("InputAttackMelee CALLED. CurrentComboCount=%d"), CurrentComboCount);
	
	if (GetCharacterMovement()->IsFalling() == true)
	{
		return;
	}

	if (0 == CurrentComboCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("BeginAttack()"));
		BeginAttack();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Buffer ON (bIsAttackKeyPressed=true)"));
		ensure(FMath::IsWithinInclusive<int32>(CurrentComboCount, 1, MaxComboCount));
		bIsAttackKeyPressed = true;
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
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("▶ DealDamage() 호출됨 — 실제 공격 판정 실행"));

	// 공격 범위(전방 150cm) 트레이스
	FVector Start = GetActorLocation();
	FVector End = Start + (GetActorForwardVector() * 150.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult Hit;

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECollisionChannel::ECC_Pawn,
		Params
	);

	if (bHit)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			UAFAttributeComponent* TargetAttr = HitActor->FindComponentByClass<UAFAttributeComponent>();

			if (TargetAttr)
			{
				TargetAttr->ApplyDamage(20.f, GetController());
				UE_LOG(LogTemp, Warning, TEXT("공격 성공 → %s에게 데미지 20 적용"), *HitActor->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("타겟에 AttributeComponent 없음"));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("공격 실패: 타격 없음"));
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

void AAFPlayerCharacter::LockMovement()
{
	if (bMovementLocked) return;
	bMovementLocked = true;

	// 이미 움직이고 있던 속도/가속 끊기
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
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

void AAFPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

}
