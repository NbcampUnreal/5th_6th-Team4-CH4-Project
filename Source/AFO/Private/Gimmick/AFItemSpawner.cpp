// AFItemSpawner.cpp


#include "Gimmick/AFItemSpawner.h"
#include "Gimmick/AFBuffItem.h"


AAFItemSpawner::AAFItemSpawner()
{
}

void AAFItemSpawner::BeginPlay()
{
    Super::BeginPlay();
    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AAFItemSpawner::SpawnRandomItem, SpawnInterval, true);
}

FVector AAFItemSpawner::GetRandomPointInAnnulus()
{
    float Angle = FMath::FRandRange(0.0f, 2.0f * PI);
    float R = FMath::Sqrt(FMath::FRandRange(FMath::Square(InnerRadius), FMath::Square(OuterRadius)));

    float X = R * FMath::Cos(Angle);
    float Y = R * FMath::Sin(Angle);

    FVector StartLocation = GetActorLocation() + FVector(X, Y, 500.0f); // 위에서 아래로 빔을 쏩니다.
    FVector EndLocation = StartLocation - FVector(0.f, 0.f, 1000.f);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    // 라인트레이스를 쏴서 실제 나무 데크 바닥 높이를 찾습니다.
    if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params))
    {
        return HitResult.Location + FVector(0.f, 0.f, 50.f); // 바닥에서 50cm 띄움
    }

    return StartLocation - FVector(0.f, 0.f, 480.f); // 실패 시 기본 높이
}

void AAFItemSpawner::SpawnRandomItem()
{
    if (ItemClasses.Num() == 0) return;

    int32 RandomIndex = FMath::RandRange(0, ItemClasses.Num() - 1);
    FVector SpawnLocation = GetRandomPointInAnnulus();

    GetWorld()->SpawnActor<AAFBuffItem>(ItemClasses[RandomIndex], SpawnLocation, FRotator::ZeroRotator);
}


void AAFItemSpawner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 에디터 뷰포트에서 영역 시각화
    if (bShowDebugVisuals)
    {
        FVector Center = GetActorLocation();

        // 안쪽 원 (경기장 중앙 뚫린 영역) - 빨간색
        DrawDebugCircle(GetWorld(), Center, InnerRadius, 100, FColor::Red, false, -1.f, 0, 5.f, FVector(0, 1, 0), FVector(1, 0, 0));

        // 바깥쪽 원 (나무 데크 외곽) - 초록색
        DrawDebugCircle(GetWorld(), Center, OuterRadius, 100, FColor::Green, false, -1.f, 0, 5.f, FVector(0, 1, 0), FVector(1, 0, 0));
    }
}