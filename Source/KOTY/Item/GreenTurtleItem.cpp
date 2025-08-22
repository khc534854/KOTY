// Fill out your copyright notice in the Description page of Project Settings.


#include "GreenTurtleItem.h"

#include "KotyMovementComponent.h"


// Sets default values
AGreenTurtleItem::AGreenTurtleItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//메시 컴포넌트 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(FName("NoCollision"), false);
	MeshComp->SetupAttachment(GetRootComponent());

	//메시 컴포넌트의 메시와 머터리얼 설정
	const ConstructorHelpers::FObjectFinder<UStaticMesh> TempMesh(
		TEXT("/Game/ItemResource/GreenTurtle/Item_GreenTurtle.Item_GreenTurtle"));
	ConstructorHelpers::FObjectFinder<UMaterial> TempMaterial(TEXT(
		"/Game/ItemResource/GreenTurtle/MT_GreenTurtle.MT_GreenTurtle"));
	if (TempMesh.Succeeded() && TempMaterial.Succeeded())
	{
		MeshComp->SetStaticMesh(TempMesh.Object);
		MeshComp->SetMaterial(0, TempMaterial.Object);
	}
}

// Called when the game starts or when spawned
void AGreenTurtleItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGreenTurtleItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MeshComp->AddRelativeRotation(FQuat::FindBetweenVectors(MeshComp->GetUpVector(), -MoveComp->GetGravityDir()) * DeltaTime);
}



