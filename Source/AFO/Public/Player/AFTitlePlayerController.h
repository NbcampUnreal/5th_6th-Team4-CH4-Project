#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AFTitlePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AFO_API AAFTitlePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void JoinServer(const FString& InIPAddress);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ASUIPlayerController, Meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> UIWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ASUIPlayerController, Meta = (AllowPrivateAccess))
	TObjectPtr<UUserWidget> UIWidgetInstance;

};