#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "AFLobbyPlayerController.generated.h"

class UUserWidget;

UCLASS()
class AFO_API AAFLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void PostSeamlessTravel() override;

	UFUNCTION(Server, Reliable)
	void ServerRequestSetTeam(uint8 NewTeamId);

	UFUNCTION(Server, Reliable)
	void ServerRequestAdvanceToCharacterSelect();

	UFUNCTION(Server, Reliable)
	void ServerRequestSelectCharacter(uint8 CharacterId); // 0~3

	UFUNCTION(Server, Reliable)
	void ServerRequestSetReady(bool bNewReady);

	UFUNCTION(Client, Reliable)
	void ClientShowMessage(const FString& Msg);

private:
	void SetupUIForCurrentMap();
	void ClearCurrentUI();
	void SetUIInputMode(bool bUIOnly);

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> TeamSelectWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> CharacterSelectWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentWidget;
};