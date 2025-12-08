#include "Character/AFPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"

AAFPlayerCharacter::AAFPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// SpringArm 생성
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 800.f;                // 카메라 거리
	SpringArm->bUsePawnControlRotation = false;        // 컨트롤러 회전 사용 X
	SpringArm->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));  // 위에서 내려다보기
	SpringArm->bDoCollisionTest = false;               // 카메라 충돌 끄기 (탑뷰는 필수)

	// 카메라 생성
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;           // 카메라 회전 고정
}

void AAFPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
}

void AAFPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AAFPlayerCharacter* PlayerController = Cast<AAFPlayerCharacter>(GetController()))
		{
			
		}
	}
}

void AAFPlayerCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	const FVector2D Input = value.Get<FVector2D>();
	if (Input.IsNearlyZero()) return;

	// 탑뷰는 월드 기준 X/Y 방향으로 이동
	AddMovementInput(FVector::ForwardVector, Input.Y);
	AddMovementInput(FVector::RightVector, Input.X);

	// 이동 방향으로 회전시키기
	FRotator NewRot = FVector(Input.Y, Input.X, 0.f).Rotation();
	SetActorRotation(NewRot);
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
	
}

void AAFPlayerCharacter::DealDamage()
{
}

