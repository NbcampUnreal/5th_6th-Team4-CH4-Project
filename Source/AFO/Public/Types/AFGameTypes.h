// AFGameTypes.h

#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AFGameTypes.generated.h" 


USTRUCT(BlueprintType)
struct FAFSkillInfo : public FTableRowBase
{
    GENERATED_BODY()

    // --- 기본 정보 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill | Basic")
    FText SkillName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill | Basic")
    FText SkillDescription;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill | Basic")
    TObjectPtr<UTexture2D> SkillIcon;

    // --- 전투 수치 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill | Stats")
    float Damage = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill | Stats")
    float ManaCost = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Stats")
    float Radius = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Stats")
    float ForwardOffset = 150.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Stats")
    float RotationOffset = 150.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill | Stats")
    float Cooldown = 0.f;
};



USTRUCT(BlueprintType)
struct FAFPlayerCharacterStatRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    FAFPlayerCharacterStatRow() : MaxHp(100.f), MaxMana(100.f), Attack(10.f), MoveSpeed(600.f) {}

    // --- 생존 관련 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat | Vitality")
    float MaxHp;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat | Vitality")
    float MaxMana;

    // --- 전투 관련 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat | Combat")
    float Attack;

    // --- 기동성 관련 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat | Mobility")
    float MoveSpeed;

};
