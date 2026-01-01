// AFStatusEffectcomponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AFStatusEffectComponent.generated.h"

class ACharacter;
class UCharacterMovementComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AFO_API UAFStatusEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAFStatusEffectComponent();


	// 서버 전용 슬로우 적용
	void ApplySlow(float InSlowRatio, float InDuration);

	void ApplyBleed(float DamagePerSec, float Duration);

protected:
	virtual void BeginPlay() override;

	// 캐시
	UPROPERTY()
	ACharacter* OwnerCharacter = nullptr;

	UPROPERTY()
	UCharacterMovementComponent* MoveComp = nullptr;

	// 상태
	UPROPERTY(ReplicatedUsing = OnRep_IsSlowed)
	bool bIsSlowed = false;

	UFUNCTION()
	void OnRep_IsSlowed();

	UPROPERTY(EditAnywhere, Category = "AF|Effect")
	TObjectPtr<UMaterialInterface> SlowOverlayMaterial;

	float OriginalWalkSpeed = 0.f;
	float SlowRatio = 1.f;
	float SlowDuration = 0.f;

	FTimerHandle SlowTimerHandle;

	void ApplySlow_Internal();

	void ClearSlow();
<<<<<<< Updated upstream
=======

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;




	UPROPERTY(ReplicatedUsing = OnRep_IsBleeding)
	bool bIsBleeding = false;

	UFUNCTION()
	void OnRep_IsBleeding(); // 서버에서도 즉시 적용

	UPROPERTY(EditAnywhere, Category = "AF|Effect")
	TObjectPtr<UMaterialInterface> BleedOverlayMaterial;

	FTimerHandle BleedTickTimer;

public:
	// 늑대인간 자신을 빨갛게 만들기 위한 함수
	void SetBleedVisual(bool bInBleeding);


	
>>>>>>> Stashed changes
};
