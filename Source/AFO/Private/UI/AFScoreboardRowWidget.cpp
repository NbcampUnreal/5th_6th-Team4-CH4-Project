// AFScoreboardRowWidget.cpp

#include "UI/AFScoreboardRowWidget.h"
#include "Player/AFPlayerState.h"
#include "Components/TextBlock.h"

void UAFScoreboardRowWidget::Init(AAFPlayerState* InPS)
{
	if (!InPS) return;
	PlayerStateRef = InPS;

	// 1. 기존 바인딩 제거 (중복 방지)
	PlayerStateRef->OnPlayerInfoChanged.RemoveAll(this);
	PlayerStateRef->OnScoreChanged.RemoveAll(this);

	// 2. 새 통합 델리게이트 바인딩
	PlayerStateRef->OnPlayerInfoChanged.AddDynamic(this, &UAFScoreboardRowWidget::OnPlayerInfoChanged);
	PlayerStateRef->OnScoreChanged.AddDynamic(this, &UAFScoreboardRowWidget::OnScoreChanged);

	// 3. 초기 데이터 출력
	RefreshTexts();
}

void UAFScoreboardRowWidget::NativeDestruct()
{
	// 메모리 누수 방지를 위한 바인딩 해제
	if (PlayerStateRef)
	{
		PlayerStateRef->OnPlayerInfoChanged.RemoveAll(this);
		PlayerStateRef->OnScoreChanged.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UAFScoreboardRowWidget::RefreshTexts()
{
	if (!PlayerStateRef) return;

	// PlayerState의 Getter를 활용해 데이터 확보
	const FString PlayerName = PlayerStateRef->GetPlayerName();
	const FString CharacterName = PlayerStateRef->GetSelectedCharacterName().ToString();
	const int32 Kills = PlayerStateRef->GetKillCount();
	const int32 Deaths = PlayerStateRef->GetDeathCount();

	// UI 텍스트 업데이트
	if (Text_PlayerName)
		Text_PlayerName->SetText(FText::FromString(PlayerName));

	if (Text_CharacterName)
		Text_CharacterName->SetText(FText::FromString(CharacterName));

	if (Text_Kills)
		Text_Kills->SetText(FText::AsNumber(Kills));

	if (Text_Deaths)
		Text_Deaths->SetText(FText::AsNumber(Deaths));

	if (Text_DamageDealt)
		Text_DamageDealt->SetText(FText::AsNumber(FMath::RoundToInt(PlayerStateRef->GetTotalDamageDealt())));

	if (Text_DamageTaken)
		Text_DamageTaken->SetText(FText::AsNumber(FMath::RoundToInt(PlayerStateRef->GetTotalDamageTaken())));

	if (Text_Healing)
		Text_Healing->SetText(FText::AsNumber(FMath::RoundToInt(PlayerStateRef->GetTotalHealingDone())));
}

void UAFScoreboardRowWidget::OnPlayerInfoChanged(AAFPlayerState* PS)
{
	RefreshTexts();
}

void UAFScoreboardRowWidget::OnScoreChanged(int32 Kills, int32 Deaths, AAFPlayerState* PS)
{
	RefreshTexts();
}