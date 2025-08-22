// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackBombExplosion.h"

#include "Components/SphereComponent.h"
#include "Components/TimelineComponent.h"


// Sets default values
ABlackBombExplosion::ABlackBombExplosion()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//적중을 감지하는 콜리전 컴포넌트를 루트로 설정
	HitComp = CreateDefaultSubobject<USphereComponent>(TEXT("HitComp"));
	HitComp->SetCollisionProfileName(FName("OverlapAll"), true);
	HitComp->SetSphereRadius(80);
	SetRootComponent(HitComp);

	//메시 컴포넌트 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(FName("NoCollision"), false);
	MeshComp->SetupAttachment(GetRootComponent());

	//메시 컴포넌트의 메시와 머터리얼 설정
	const ConstructorHelpers::FObjectFinder<UStaticMesh> TempMesh(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	ConstructorHelpers::FObjectFinder<UMaterial> TempMaterial(
		TEXT("/Game/Item/BlackBomb/MT_Explosion.MT_Explosion"));
	if (TempMesh.Succeeded() && TempMaterial.Succeeded())
	{
		MeshComp->SetStaticMesh(TempMesh.Object);
	 	MeshComp->SetMaterial(0, TempMaterial.Object);
	}

	//타임라인 컴포넌트
	TimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComp"));

	//커브 플로트 로드
	const ConstructorHelpers::FObjectFinder<UCurveFloat> Temp(
		TEXT("/Game/Item/BlackBomb/ExplosionAlphaCurve.ExplosionAlphaCurve"));
	if (Temp.Succeeded())
	{
		CurveFloat = Temp.Object;
	}
}

void ABlackBombExplosion::BeginPlay()
{
	Super::BeginPlay();

	//머터리얼 인스턴스
	MaterialInst = MeshComp->CreateDynamicMaterialInstance(0);

	//타임라인
	FOnTimelineFloat Callback;
	Callback.BindUFunction(this, FName("UpdateExplosion"));
	TimelineComp->AddInterpFloat(CurveFloat, Callback);
	TimelineComp->SetTimelineLength(1.5);
	TimelineComp->SetLooping(false);
	TimelineComp->PlayFromStart();

	//타이머
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABlackBombExplosion::EndExplosion, 2.0, false);
}

void ABlackBombExplosion::UpdateExplosion(float Value)
{
	MaterialInst->SetScalarParameterValue(FName("Opacity"), Value * 0.75);
	SetActorScale3D(FVector::Zero() + Value * 14);
}

void ABlackBombExplosion::EndExplosion()
{
	this->Destroy();
}

void ABlackBombExplosion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

