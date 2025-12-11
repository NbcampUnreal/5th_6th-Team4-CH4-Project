// Fill out your copyright notice in the Description page of Project Settings.

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

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attribute")
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attribute")
	float Health = 100.f;

	// 데미지 적용 함수
	void ApplyDamage(float Damage);
};
