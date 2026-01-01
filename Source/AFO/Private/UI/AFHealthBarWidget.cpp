// AFHealthBarWidget.cpp


#include "UI/AFHealthBarWidget.h"
#include "Components/AFAttributeComponent.h"
#include "Player/AFPlayerState.h"
#include "GameFramework/Pawn.h"
#include "UI/AFSkillMainWidget.h"  
#include "Character/AFPlayerCharacter.h" 
#include "Components/AFSkillComponent.h"
#include "Types/AFGameTypes.h"

void UAFHealthBarWidget::BindToCharacter(AAFPlayerCharacter* NewCharacter)
{
    if (!NewCharacter) return;

    // 1. 캐릭터 참조 저장
    TargetCharacter = NewCharacter;

    // 2. 캐릭터가 이미 데이터를 들고 있다면 즉시 UI 업데이트
    // 만약 SkillMainWidget을 이 위젯이 들고 있다면 아래와 같이 호출합니다.
    if (SkillMainWidget)
    {
        TArray<FAFSkillInfo> Skills;
        TArray<FName> Names;

        // 캐릭터에 캐싱된 데이터들을 긁어모읍니다.
        // (캐릭터 헤더에 Public Getter 함수들이 있다고 가정하거나 변수가 Public이어야 합니다)
        if (NewCharacter->GetCachedAttackData())
        {
            Skills.Add(*(NewCharacter->GetCachedAttackData()));
            Names.Add(NewCharacter->GetCachedAttackKey());
        }
        if (NewCharacter->GetCachedHeavyAttackData())
        {
            Skills.Add(*(NewCharacter->GetCachedHeavyAttackData()));
            Names.Add(NewCharacter->GetCachedHeavyAttackKey());
        }
        if (NewCharacter->GetCachedSkillQData())
        {
            Skills.Add(*(NewCharacter->GetCachedSkillQData()));
            Names.Add(NewCharacter->GetCachedSkillQKey());
        }
        if (NewCharacter->GetCachedSkillEData())
        {
            Skills.Add(*(NewCharacter->GetCachedSkillEData()));
            Names.Add(NewCharacter->GetCachedSkillEKey());
        }

        // UI에 데이터 주입! (이 함수가 불려야 아이콘이 나타납니다)
        SkillMainWidget->UpdateSkillSlots(Skills, Names);

        // 스킬 컴포넌트 연결 (쿨타임 계산용)
        UAFSkillComponent* SkillComp = NewCharacter->FindComponentByClass<UAFSkillComponent>();
        SkillMainWidget->UpdateAllSlotsComponent(SkillComp);

        UE_LOG(LogTemp, Warning, TEXT("@@@ [UI] BindToCharacter - Skill Data Injected!"));
    }
}


void UAFHealthBarWidget::HandleHealthChanged(float CurrentHealth, float MaxHealth)
{
    if (MaxHealth <= 0.f) return;

    float Percent = CurrentHealth / MaxHealth;

    // [핵심] C++에서 직접 프로그레스 바 업데이트
    if (HealthPB)
    {
        HealthPB->SetPercent(Percent);
        UE_LOG(LogTemp, Log, TEXT("[HealthBar] Direct UI Update: %.2f%%"), Percent * 100.f);
    }

    // 기존 이벤트는 연출용(애니메이션 등)으로 남겨두고 호출만 해줍니다.
    OnUpdateHealthVisual(Percent);
}


void UAFHealthBarWidget::UpdateInitialState()
{
	AActor* Actor = TargetActor.Get();
	if (!Actor)
	{
		GetWorld()->GetTimerManager().ClearTimer(InitializationTimerHandle);
		return;
	}

	APawn* OwningPawn = Cast<APawn>(Actor);
	if (!OwningPawn) return;

	// 내 PlayerState와 상대방 PlayerState를 모두 가져옵니다.
    UAFAttributeComponent* AttrComp = Actor->FindComponentByClass<UAFAttributeComponent>();
	APlayerController* LocalPC = GetWorld()->GetFirstPlayerController();
	AAFPlayerState* LocalPS = LocalPC ? LocalPC->GetPlayerState<AAFPlayerState>() : nullptr;
	AAFPlayerState* TargetPS = OwningPawn->GetPlayerState<AAFPlayerState>();

	// ★ 핵심: 둘 다 존재해야 하며, 팀 ID가 초기값(보통 255)이 아닐 때만 실행합니다.
    if (AttrComp && TargetPS && LocalPS && TargetPS->GetTeamID() != 255)
    {
        // [중요] AttributeComponent의 델리게이트에 바인딩
        AttrComp->OnHealthChanged.RemoveDynamic(this, &UAFHealthBarWidget::HandleHealthChanged);
        AttrComp->OnHealthChanged.AddDynamic(this, &UAFHealthBarWidget::HandleHealthChanged);

        // 초기 값 즉시 반영
        float CurrHP = AttrComp->GetHealth();
        float MaxHP = AttrComp->GetMaxHealth();
        HandleHealthChanged(CurrHP, MaxHP);

        // 팀 색상 설정 (내 팀과 같으면 파랑, 다르면 빨강)
        FLinearColor TeamColor = (LocalPS->GetTeamID() == TargetPS->GetTeamID()) ? FLinearColor::Blue : FLinearColor::Red;

        if (HealthPB)
        {
            HealthPB->SetFillColorAndOpacity(TeamColor);
        }
        OnUpdateTeamVisual(TeamColor);

        // 성공했으므로 타이머 중지
        GetWorld()->GetTimerManager().ClearTimer(InitializationTimerHandle);

		UE_LOG(LogTemp, Log, TEXT("HealthBar Successfully Bound to: %s (Team: %d)"), *Actor->GetName(), TargetPS->GetTeamID());
	}
}


void UAFHealthBarWidget::AttemptBind()
{
    if (bIsInitialized)
    {
        GetWorld()->GetTimerManager().ClearTimer(InitializationTimerHandle);
        return;
    }

    AActor* Actor = TargetActor.Get();
    if (!Actor) return;

    // 1. 컴포넌트 및 PlayerState 확보

    APawn* OwningPawn = Cast<APawn>(Actor);
    if (!OwningPawn) return;
    UAFAttributeComponent* AttrComp = Actor->FindComponentByClass<UAFAttributeComponent>();
    AAFPlayerState* TargetPS = OwningPawn->GetPlayerState<AAFPlayerState>();
    APlayerController* LocalPC = GetWorld()->GetFirstPlayerController();
    AAFPlayerState* LocalPS = LocalPC ? LocalPC->GetPlayerState<AAFPlayerState>() : nullptr;

    // 2. 모든 필수 데이터가 서버로부터 복제되었는지 확인
    if (TargetPS && LocalPS && TargetPS->GetTeamID() != 255 && LocalPS->GetTeamID() != 255 && AttrComp->GetMaxHealth() > 0.f)
    {
        AttrComp->OnHealthChanged.RemoveDynamic(this, &UAFHealthBarWidget::HandleHealthChanged);
        AttrComp->OnHealthChanged.AddDynamic(this, &UAFHealthBarWidget::HandleHealthChanged);

        // 4. 초기 UI 셋팅
        float CurrHP = AttrComp->GetHealth();
        float MaxHP = AttrComp->GetMaxHealth();
        float Percent = CurrHP / MaxHP;

        // 팀 색상 결정
        FLinearColor TeamColor = (LocalPS->GetTeamID() == TargetPS->GetTeamID()) ? FLinearColor::Blue : FLinearColor::Red;

        if (HealthPB)
        {
            HealthPB->SetPercent(Percent);
            HealthPB->SetFillColorAndOpacity(TeamColor);
        }

        // 블루프린트 연출 호출 (애니메이션 등)
        OnUpdateTeamVisual(TeamColor);
        OnUpdateHealthVisual(Percent);

        bIsInitialized = true;
        GetWorld()->GetTimerManager().ClearTimer(InitializationTimerHandle);
        }
    }
