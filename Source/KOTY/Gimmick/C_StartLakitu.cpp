// Fill out your copyright notice in the Description page of Project Settings.


#include "C_StartLakitu.h"


// Sets default values
AC_StartLakitu::AC_StartLakitu()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StartLakitu = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StartLakitu"));
	RootComponent = StartLakitu;
	ConstructorHelpers::FObjectFinder<UStaticMesh> LakituMeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Lakitu/SM_LakituStart.SM_LakituStart'"));

	if (LakituMeshAsset.Succeeded())
	{
		StartLakitu->SetStaticMesh(LakituMeshAsset.Object);
	}
}

// Called when the game starts or when spawned
void AC_StartLakitu::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AC_StartLakitu::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

