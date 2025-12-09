#include "Character/AFPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "Player/AFPlayerController.h"

AAFPlayerCharacter::AAFPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// 스프링암 생성
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 300.f;
	SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	SpringArm->bDoCollisionTest = false;

	// 캐릭터에서 완전히 분리
	SpringArm->SetupAttachment(nullptr);

	// 카메라 생성
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// 캐릭터 회전과 무관하게 카메라 고정
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AAFPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// 카메라 초기 위치를 캐릭터 근처로 이동
	FVector StartLoc = GetActorLocation();
	StartLoc.Z += 800.f;
	SpringArm->SetWorldLocation(StartLoc);
	
	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
}

void AAFPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAFPlayerCharacter::Move);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAFPlayerCharacter::Look);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &AAFPlayerCharacter::StartSprint);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &AAFPlayerCharacter::StopSprint);
		EnhancedInput->BindAction(AttackAction, ETriggerEvent::Started, this, &AAFPlayerCharacter::Attack);
	}
}

void AAFPlayerCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	FVector2D Input = value.Get<FVector2D>();
	if (Input.IsNearlyZero()) return;

	AddMovementInput(FVector::ForwardVector, Input.Y);
	AddMovementInput(FVector::RightVector, Input.X);
}

void AAFPlayerCharacter::Look(const FInputActionValue& value)
{
	FVector2D Input = value.Get<FVector2D>();
	if (Input.IsNearlyZero()) return;

	// 화면 이동 (월드 기준)
	FVector MoveDir(Input.Y, Input.X, 0.f);
	MoveDir *= CameraPanSpeed * GetWorld()->GetDeltaSeconds();

	SpringArm->AddWorldOffset(MoveDir);
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
	UE_LOG(LogTemp, Warning, TEXT("▶ AttackInput() 함수 호출됨! 공격 입력 감지됨."));
	
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
}

