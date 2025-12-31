#include "Character/AFDarkKnight.h"
#include "Components/AFAttributeComponent.h"
#include "TimerManager.h"

void AAFDarkKnight::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAFDarkKnight::ApplyPassiveBleed(AActor* Target)
{
	if (!HasAuthority()) return;
	if (!Target) return;

	if (UAFAttributeComponent* Attr = Target->FindComponentByClass<UAFAttributeComponent>())
	{
		Attr->ApplyHealthChange(-5);
	}
}

void AAFDarkKnight::UseSkillE_Implementation()
{
	HandleSkillHitCheck(200.f, 30.f, 0.f);
}

void AAFDarkKnight::ServerRPC_SkillQ_Implementation()
{
	if (!HasAuthority()) return;

	// 다른 캐릭터들처럼 막고 싶으면 유지
	if (bIsAttacking || bIsUsingSkill) return;
	if (bIsQBuffActive || bIsQCharging) return;

	bIsUsingSkill = true;
	bIsQCharging = true;

	UE_LOG(LogTemp, Warning, TEXT("[DK Q] ServerRPC_SkillQ_Implementation CALLED"));

	Multicast_PlayQChargeMontage();

	GetWorldTimerManager().ClearTimer(QChargeDelayTimer);
	GetWorldTimerManager().SetTimer(
		QChargeDelayTimer,
		this,
		&AAFDarkKnight::ApplyQBuff_Server,
		QChargeFxDelay,
		false
	);
}

void AAFDarkKnight::ApplyQBuff_Server()
{
	if (!HasAuthority()) return;
	if (!bIsQCharging) return;      // 차징 중일 때만
	if (bIsQBuffActive) return;     // 중복 방지

	bIsQCharging = false;
	bIsQBuffActive = true;

	UE_LOG(LogTemp, Warning, TEXT("[DK Q] ApplyQBuff_Server -> START FX + BUFF"));

	Multicast_StartQChargeFX();

	if (UAFAttributeComponent* Attr = FindComponentByClass<UAFAttributeComponent>())
	{
		Attr->ModifyMaxHealth(QBuffMultiplier);
	}

	GetWorldTimerManager().ClearTimer(QBuffTimer);
	GetWorldTimerManager().SetTimer(
		QBuffTimer,
		this,
		&AAFDarkKnight::EndQBuff,
		QBuffDuration,
		false
	);
}

void AAFDarkKnight::EndQBuff()
{
	if (!HasAuthority()) return;
	if (!bIsQBuffActive) return;

	UE_LOG(LogTemp, Warning, TEXT("[DK Q] EndQBuff -> STOP FX + RESET"));

	if (UAFAttributeComponent* Attr = FindComponentByClass<UAFAttributeComponent>())
	{
		Attr->ResetMaxHealth();
	}

	bIsQBuffActive = false;
	bIsQCharging = false;
	bIsUsingSkill = false;

	Multicast_StopQChargeFX();
}

void AAFDarkKnight::Multicast_PlayQChargeMontage_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("[DK Q] Multicast_PlayQChargeMontage"));

	if (SkillQChargeMontage)
	{
		PlayAnimMontage(SkillQChargeMontage);
	}
}

void AAFDarkKnight::Multicast_StartQChargeFX_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("[DK Q] Multicast_StartQChargeFX CALLED"));

	if (!QChargeFXBP || !GetMesh())
	{
		UE_LOG(LogTemp, Warning, TEXT("[DK Q] FXBP or Mesh is NULL"));
		return;
	}

	if (QChargeFXActor)
	{
		QChargeFXActor->Destroy();
		QChargeFXActor = nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	QChargeFXActor = GetWorld()->SpawnActor<AActor>(QChargeFXBP, GetActorLocation(), GetActorRotation(), Params);
	if (!QChargeFXActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DK Q] SpawnActor FAILED"));
		return;
	}

	QChargeFXActor->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		QChargeAttachSocket
	);

	QChargeFXActor->SetActorRelativeLocation(QChargeFXLocationOffset);
	QChargeFXActor->SetActorRelativeRotation(QChargeFXRotationOffset);

	UE_LOG(LogTemp, Warning, TEXT("[DK Q] FX SPAWN OK"));
}

void AAFDarkKnight::Multicast_StopQChargeFX_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("[DK Q] Multicast_StopQChargeFX"));

	if (QChargeFXActor)
	{
		QChargeFXActor->Destroy();
		QChargeFXActor = nullptr;
	}
}
