#include "Animation/AFCheckInput_Attack.h"
#include "Character/AFPlayerCharacter.h"

void UAFCheckInput_Attack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);


	AAFPlayerCharacter* C = Cast<AAFPlayerCharacter>(MeshComp->GetOwner());
	if (!C) return;

	UAnimInstance* Anim = MeshComp->GetAnimInstance();
	USkeletalMeshComponent* OwnerMesh = C->GetMesh();
	UAnimInstance* OwnerAnim = OwnerMesh ? OwnerMesh->GetAnimInstance() : nullptr;

	UE_LOG(LogTemp, Warning, TEXT("Notify Mesh=%s OwnerMesh=%s SameMesh=%d Anim=%p OwnerAnim=%p"),
		*GetNameSafe(MeshComp),
		*GetNameSafe(OwnerMesh),
		(MeshComp == OwnerMesh),
		Anim,
		OwnerAnim
	);

	C->HandleOnCheckInputAttack_FromNotify(Anim);
}
