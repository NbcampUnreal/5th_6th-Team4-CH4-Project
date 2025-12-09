#include "Game/AFGameMode.h"

void AAFGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AAFGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}
