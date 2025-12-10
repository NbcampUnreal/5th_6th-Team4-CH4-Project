#include "Character/AFPlayerCharacter.h"

#include "AFAttributeComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "Player/AFPlayerController.h"

AAFPlayerCharacter::AAFPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AttributeComp = CreateDefaultSubobject<UAFAttributeComponent>(TEXT("AttributeComponent"));
	
	NormalSpeed = 300.f;
	SprintSpeedMultiplier = 1.5f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	// 스프링암 생성
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 350.f;
	SpringArm->bUsePawnControlRotation = true;

	// 카메라 생성
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AAFPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &AAFPlayerCharacter::OnAttackMontageEnded);
	}
	
	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
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
	RightDir   = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	/*// 실제 이동
	AddMovementInput(ForwardDir, MoveInput.X);
	AddMovementInput(RightDir,   MoveInput.Y);*/
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
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void AAFPlayerCharacter::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void AAFPlayerCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsAttacking = false; 
}

void AAFPlayerCharacter::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("AttackInput()"));
	
	if (bIsAttacking) 
	{
		return; // 이미 공격 중이면 무시
	}

	if (AttackMontage)
	{
		bIsAttacking = true;
		PlayAnimMontage(AttackMontage);
	}
}

void AAFPlayerCharacter::DealDamage()
{
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
				TargetAttr->ApplyDamage(20.f);
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


