#pragma once

#include "CoreMinimal.h"
#include "Character/AFPlayerCharacter.h"
#include "AFDarkKnight.generated.h"

UCLASS()
class AFO_API AAFDarkKnight : public AAFPlayerCharacter
{
	GENERATED_BODY()
	
public:
	virtual void StartSprint(const FInputActionValue& Value) override;
	virtual void StopSprint(const FInputActionValue& Value) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	bool bIsSprinting = false;
};
