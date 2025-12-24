#pragma once

#include "CoreMinimal.h"
#include "Character/AFPlayerCharacter.h"
#include "AFWereWolf.generated.h"

UCLASS()
class AFO_API AAFWereWolf : public AAFPlayerCharacter
{
	GENERATED_BODY()
	
public:
	AAFWereWolf();
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float WereWolfMoveSpeed = 200.f; 
};