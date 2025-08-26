// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/SquidItem.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ASquidItem::ASquidItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//메시 컴포넌트 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(FName("NoCollision"), false);
	MeshComp->SetupAttachment(GetRootComponent());

	if (const ConstructorHelpers::FObjectFinder<UStaticMesh> Finder(TEXT("/Game/Item/Squid/Model/SM_Squid.SM_Squid")); Finder.Succeeded())
	{
		MeshComp->SetStaticMesh(Finder.Object);
	}
	
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

	UE_LOG(LogTemp, Warning, TEXT("Star Item Used by %s"), *TargetActor->GetName());
}
