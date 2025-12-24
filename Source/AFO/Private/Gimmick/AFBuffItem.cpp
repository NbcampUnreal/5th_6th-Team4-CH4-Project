// AFBuffItem.cpp

#include "Gimmick/AFBuffItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"


AAFBuffItem::AAFBuffItem()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetSphereRadius(50.f);

	// 서버에서만 Overlap 이벤트를 실행하도록 설정
	CollisionComponent->SetCollisionProfileName(TEXT("Trigger"));
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AAFBuffItem::OnOverlapBegin);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BuffEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BuffEffect"));
	BuffEffect->SetupAttachment(RootComponent);
}

void AAFBuffItem::BeginPlay()
{
	Super::BeginPlay();

	// 나이아가라 시스템 안의 'Color' 파라미터 이름을 맞춰야 함 (예: "UserColor")
	if (BuffEffect)
	{
		BuffEffect->SetNiagaraVariableLinearColor(TEXT("User.BaseColor"), BuffColor);
	}
}

void AAFBuffItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return; // 서버에서만 처리

	// 플레이어인지 확인 (인터페이스나 캐스팅 사용)
	// 예: IAFCharacterItemInterface* ItemCharacter = Cast<IAFCharacterItemInterface>(OtherActor);
	// if (ItemCharacter) { ItemCharacter->ApplyBuff(BuffType, BuffValue); }

	UE_LOG(LogTemp, Log, TEXT("Item Picked Up: %s"), *OtherActor->GetName());

	Destroy(); // 아이템 제거 (서버에서 지우면 클라에서도 사라짐)
}

void AAFBuffItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 아이템이 둥둥 떠있거나 회전하는 연출 (클라에서도 돌아감)
	AddActorLocalRotation(FRotator(0.f, 100.f * DeltaTime, 0.f));
}
