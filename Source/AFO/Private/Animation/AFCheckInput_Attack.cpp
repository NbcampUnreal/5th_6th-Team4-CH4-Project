#include "Animation/AFCheckInput_Attack.h"
#include "Character/AFPlayerCharacter.h"

void UAFCheckInput_Attack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp) == true)
	{
		AAFPlayerCharacter* AttackingCharacter = Cast<AAFPlayerCharacter>(MeshComp->GetOwner());
		if (IsValid(AttackingCharacter) == true)
		{
			AttackingCharacter->HandleOnCheckInputAttack();
		}
	}
}
