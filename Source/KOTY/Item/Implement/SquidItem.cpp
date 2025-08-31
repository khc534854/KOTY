// Fill out your copyright notice in the Description page of Project Settings.

#include "SquidItem.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ASquidItem::ASquidItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	const auto SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComp);
	
	//메시 컴포넌트 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(FName("NoCollision"), false);
	MeshComp->SetupAttachment(GetRootComponent());

	//스태틱 메시 로드
	if (const ConstructorHelpers::FObjectFinder<UStaticMesh> Finder(TEXT("/Game/Item/Squid/Model/SM_Squid.SM_Squid")); Finder.Succeeded())
	{
		MeshComp->SetStaticMesh(Finder.Object);
	}

	//머터리얼 로드
	if (const ConstructorHelpers::FObjectFinder<UMaterial> Finder(TEXT("/Game/Item/Squid/Model/MT_Squid.MT_Squid")); Finder.Succeeded())
	{
		MeshComp->SetMaterial(0, Finder.Object);
	}
}

void ASquidItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASquidItem::ApplyItemEffect(AActor* TargetActor)
{
	Super::ApplyItemEffect(TargetActor);

	UE_LOG(LogTemp, Warning, TEXT("Squid Item Used To %s"), *TargetActor->GetName());

	//플레이어의 경우
	if (TargetActor == Cast<AActor>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
	{
		//포스트 프로세싱으로 먹물 효과 적용
	}
	
	this->Destroy();
}

void ASquidItem::OnUseItem(UKotyItemHoldComponent* HoldComp)
{
	Super::OnUseItem(HoldComp);

	//모든 카트 검색
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("HasHitComp"), OutActors);
	for (const auto Actor : OutActors)
	{
		//자신 이외의 모든 카트에 대해
		if (Actor != ItemOwningActor)
		{
			if (const auto OtherHitComp = Actor->GetComponentByClass<UKotyItemHitComponent>())
			{
				//아이템 효과 적용 요청
				RequestApplyItemEffectToOtherHitComp(OtherHitComp);
			}
		}
	}
}

void ASquidItem::OnLoseItem(UKotyItemHoldComponent* HoldComp)
{
	Super::OnLoseItem(HoldComp);

	//파괴
	this->Destroy();
}
