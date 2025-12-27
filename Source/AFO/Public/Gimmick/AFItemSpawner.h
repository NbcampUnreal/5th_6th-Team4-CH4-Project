// AFItemSpawner.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AFItemSpawner.generated.h"

UCLASS()
class AFO_API AAFItemSpawner : public AActor
{
	GENERATED_BODY()
	
public:
    AAFItemSpawner();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = "Spawn Settings")
    TArray<TSubclassOf<class AAFBuffItem>> ItemClasses;

    UPROPERTY(EditAnywhere, Category = "Spawn Settings")
    float InnerRadius = 500.0f; // 경기장 중심부 (뚫린 공간)

    UPROPERTY(EditAnywhere, Category = "Spawn Settings")
    float OuterRadius = 1500.0f; // 나무 데크 외곽 끝부분

    UPROPERTY(EditAnywhere, Category = "Spawn Settings")
    float SpawnInterval = 10.0f;

    void SpawnRandomItem();

    // 원형 영역 내 랜덤 위치 계산 함수
    FVector GetRandomPointInAnnulus();


    // 에디터에서 영역을 그릴지 여부
    UPROPERTY(EditAnywhere, Category = "Spawn Settings")
    bool bShowDebugVisuals = true;

    // 에디터 상에서 실시간으로 영역을 그리기 위한 함수
    virtual bool ShouldTickIfViewportsOnly() const override { return true; }
    virtual void Tick(float DeltaTime) override;

private:
    FTimerHandle SpawnTimerHandle;
};
