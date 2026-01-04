// AFAttributeComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AFAttributeComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AFO_API UAFAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAFAttributeComponent();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	float MaxHealth = 300.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attribute")
	float Health = 300.f;

	//사망 상태 플래그
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attribute")
	bool bIsDead = false;
	
public: 
	bool IsDead() const { return bIsDead; }

public:
	void ApplyDamage(float Damage, AController* InstigatedBy);

protected:
	// 사망 처리 분리
	void HandleDeath(AController* InstigatedBy);



public:
	// Health 변경 시 PlayerState에 동기화 하는 함수
	void SyncHealthToPlayerState();


private:
	FTimerHandle HealthSyncTimerHandle;


};