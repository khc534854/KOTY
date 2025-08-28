// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicItemBase.h"
#include <Components/SphereComponent.h>
#include "Components/AudioComponent.h"
#include "Item/Component/KotyMovementComponent.h"
#include "Kismet/GameplayStatics.h"

APhysicItemBase::APhysicItemBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//충돌체를 루트 컴포넌트로 설정
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionProfileName(FName("BlockAll"), true);
	SphereComp->SetSphereRadius(70);
	SphereComp->ShapeColor = FColor::Blue;
	SetRootComponent(SphereComp);

	//적중을 감지하는 콜리전 컴포넌트 부착
	HitComp = CreateDefaultSubobject<USphereComponent>(TEXT("HitComp"));
	HitComp->SetCollisionProfileName(FName("OverlapAll"), true);
	HitComp->SetSphereRadius(80);
	SphereComp->ShapeColor = FColor::Red;
	HitComp->SetupAttachment(RootComponent);

	//오디오 컴포넌트의 위치 재조정
	AudioComp->SetupAttachment(SphereComp);

	//아이템 무브먼트 컴포넌트 부착
	MoveComp = CreateDefaultSubobject<UKotyMovementComponent>(TEXT("KotyMovement"));

	//지면 충돌 사운드 큐 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SC_Bounce.SC_Bounce"));
		Finder.Succeeded())
	{
		GroundHitSoundCue = Finder.Object;
	}

	//지면 충돌 사운드를 재생하는 람다식을 무브먼트 컴포넌트에 바인드
	MoveComp->OnBounceEventDispatcher.AddLambda([this]()
	{
		if (GroundHitSoundCue)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), GroundHitSoundCue, GetActorLocation(), GetActorRotation(), 1, 1, 0, SoundAttenuation);
		}
	});
}

void APhysicItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MoveComp->IsOnSimulate())
	{
		//메시 컴포넌트의 머리가 중력의 반대 방향을 향하도록 업데이트
		MeshComp->AddWorldRotation(FQuat::FindBetweenVectors(MeshComp->GetUpVector(), -MoveComp->GetGravityDir()) * DeltaTime);
	}
}

void APhysicItemBase::OnSimulateBegin()
{
	
}
