// AFBuffItem.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AFBuffItem.generated.h"

class UNiagaraComponent;

UENUM(BlueprintType)
enum class EBuffType : uint8
{
	Health      UMETA(DisplayName = "Health"),
	AttackPower UMETA(DisplayName = "AttackPower"),
	Speed       UMETA(DisplayName = "Speed")
};

UCLASS()
class AFO_API AAFBuffItem : public AActor
{
	GENERATED_BODY()
	
public:	

	AAFBuffItem();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, Category = "Buff")
	EBuffType BuffType;

	UPROPERTY(EditAnywhere, Category = "Buff")
	float BuffValue = 20.0f;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	UNiagaraComponent* BuffEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
	FLinearColor BuffColor;

	// 서버에서만 충돌을 처리하도록 설정
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// 아이템 회전 등 연출용 Tick
	virtual void Tick(float DeltaTime) override;
	
	
};
