// Fill out your copyright notice in the Description page of Project Settings.


#include "KOTY/AI_Player/AAiPlayerKartPawn.h"

// Sets default values
AAAiPlayerKartPawn::AAAiPlayerKartPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAAiPlayerKartPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAAiPlayerKartPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAAiPlayerKartPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

