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
		PlayAnimMontage(SkillEMontage);
	}
}

void AAFPlayerCharacter::Multicast_PlaySkillQMontage_Implementation()
{
	// 서버와 모든 클라이언트에서 동시에 실행됨
	if (SkillQMontage)
	{
		bIsUsingSkill = true;
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

			if (PlayerController->AttackAction)
			{
				EnhancedInput->BindAction(
					PlayerController->AttackAction,
					ETriggerEvent::Started,
					this,
					&AAFPlayerCharacter::Attack
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

			EnhancedInput->BindAction(PlayerController->AttackAction, ETriggerEvent::Started, this, &ThisClass::InputAttackMelee);
		}
	}
}

void AAFPlayerCharacter::Move(const FInputActionValue& value)
{
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
	bIsAttacking = false;

	if (Montage == AttackMontage)
	{
		bIsAttacking = false;
	}

	if (Montage == SkillEMontage || Montage == SkillQMontage)
	{
		bIsUsingSkill = false;
	}
}

void AAFPlayerCharacter::HandleOnCheckHit()
{
	UKismetSystemLibrary::PrintString(this, TEXT("HandleOnCheckHit()"));
}

void AAFPlayerCharacter::HandleOnCheckInputAttack()
{
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	checkf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	if (bIsAttackKeyPressed == true)
	{
		CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, MaxComboCount);

		const FName NextSectionName = *FString::Printf(TEXT("%s%02d"), *AttackAnimMontageSectionPrefix, CurrentComboCount);

		if (AttackMontage)
		{
			UE_LOG(LogTemp, Error, TEXT("JumpToSection: %s (Montage: %s)"), *NextSectionName.ToString(),
				*AttackMontage->GetName());

			AnimInstance->Montage_JumpToSection(NextSectionName, AttackMontage);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("JumpToSection Failed: AttackMontage is NULL"));
		}

		bIsAttackKeyPressed = false;
	}
}

void AAFPlayerCharacter::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	ensureMsgf(CurrentComboCount != 0, TEXT("CurrentComboCount == 0"));

	CurrentComboCount = 0;
	bIsAttackKeyPressed = false;
	bIsNowAttacking = false;

	if (OnMeleeAttackMontageEndedDelegate.IsBound() == true)
	{
		OnMeleeAttackMontageEndedDelegate.Unbind();
	}
}

void AAFPlayerCharacter::InputAttackMelee(const FInputActionValue& InValue)
{
	if (GetCharacterMovement()->IsFalling() == true)
	{
		return;
	}

	if (0 == CurrentComboCount)
	{
		UE_LOG(LogTemp, Error, TEXT("33"));
		BeginAttack();
	}
	else
	{
		ensure(FMath::IsWithinInclusive<int32>(CurrentComboCount, 1, MaxComboCount));
		bIsAttackKeyPressed = true;
	}
}

void AAFPlayerCharacter::BeginAttack()
{
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	checkf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	bIsNowAttacking = true;

	if (IsValid(AttackMontage) && !AnimInstance->Montage_IsPlaying(AttackMontage))
	{
		AnimInstance->Montage_Play(AttackMontage);
	}

	CurrentComboCount = 1;

	if (!OnMeleeAttackMontageEndedDelegate.IsBound())
	{
		OnMeleeAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndAttack);
		AnimInstance->Montage_SetEndDelegate(OnMeleeAttackMontageEndedDelegate, AttackMontage);
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

void AAFPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

}
