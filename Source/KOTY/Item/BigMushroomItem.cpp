// Fill out your copyright notice in the Description page of Project Settings.


#include "BigMushroomItem.h"

#include "Components/SphereComponent.h"

ABigMushroomItem::ABigMushroomItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//메시 컴포넌트 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(FName("NoCollision"), false);
	MeshComp->SetupAttachment(GetRootComponent());

	//스태틱 메시 로드
	if (const ConstructorHelpers::FObjectFinder<UStaticMesh> Finder(TEXT("/Game/Item/Mushroom/Model/SM_Mushroom.SM_Mushroom"));
		Finder.Succeeded())
	{
		MeshComp->SetStaticMesh(Finder.Object);
	}

	//머터리얼 로드
	if (const ConstructorHelpers::FObjectFinder<UMaterial> Finder(TEXT("/Game/Item/Mushroom/Model/MT_Mushroom.MT_Mushroom"));
		Finder.Succeeded())
	{
		MeshComp->SetMaterial(0, Finder.Object);
	}

	//크기에 맞춰 변경
	MeshComp->SetWorldScale3D(FVector(2, 2, 2));
	SphereComp->SetSphereRadius(60);
	HitComp->SetSphereRadius(80);
}

void ABigMushroomItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABigMushroomItem::ApplyItemEffect(AActor* TargetActor)
{
	Super::ApplyItemEffect(TargetActor);

	UE_LOG(LogTemp, Warning, TEXT("BigMushroom Item Used by %s"), *TargetActor->GetName());
}

