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
		Attr->ApplyHealthChange(-5); // 1초마다 HP -5
	}
}


 // E 스킬 : 한바퀴 돌며 광역 공격

void AAFDarkKnight::UseSkillE_Implementation()
{
	HandleSkillHitCheck(200.f, 30.f, 0.f); // Radius, Damage, RotationOffset
}

// Q 스킬 : 20초 버프 (HP/MP/데미지 5% 증가)
void AAFDarkKnight::UseSkillQ_Implementation()
{
	if (!HasAuthority()) return;
	if (bIsQBuffActive) return;

	if (UAFAttributeComponent* Attr = FindComponentByClass<UAFAttributeComponent>())
	{
		Attr->ModifyMaxHealth (1.05f);
	}

	bIsQBuffActive = true;

	GetWorld()->GetTimerManager().SetTimer(
		QBuffTimer, this, &AAFDarkKnight::EndQBuff, 20.f, false
	);
	
}

void AAFDarkKnight::EndQBuff()
{
	if (!HasAuthority()) return;
	if (!bIsQBuffActive) return;

	if (UAFAttributeComponent* Attr = FindComponentByClass<UAFAttributeComponent>())
	{
		Attr->ResetMaxHealth();
	}


	bIsQBuffActive = false;
	
}
