// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackBombItem.h"

ABlackBombItem::ABlackBombItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//메시 컴포넌트 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(FName("NoCollision"), false);
	MeshComp->SetupAttachment(GetRootComponent());

	//메시 컴포넌트의 메시와 머터리얼 설정
	const ConstructorHelpers::FObjectFinder<UStaticMesh> TempMesh(
		TEXT("/Game/ItemResource/Bomb/ItemBomhei.ItemBomhei"));
	ConstructorHelpers::FObjectFinder<UMaterial> TempMaterial(TEXT(
		"/Game/ItemResource/Bomb/MT_Bomb.MT_Bomb"));
	if (TempMesh.Succeeded() && TempMaterial.Succeeded())
	{
		MeshComp->SetStaticMesh(TempMesh.Object);
		MeshComp->SetMaterial(0, TempMaterial.Object);
	}
}

void ABlackBombItem::BeginPlay()
{
	Super::BeginPlay();
}

void ABlackBombItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

