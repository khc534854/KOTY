// Fill out your copyright notice in the Description page of Project Settings.


#include "RedTurtleItem.h"

#include "KotyMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"

ARedTurtleItem::ARedTurtleItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//메시 컴포넌트 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(FName("NoCollision"), false);
	MeshComp->SetupAttachment(GetRootComponent());

	//메시 컴포넌트의 메시와 머터리얼 설정
	const ConstructorHelpers::FObjectFinder<UStaticMesh> TempMesh(
		TEXT("/Game/ItemResource/RedTurtle/Item_RedTurtle.Item_RedTurtle"));
	ConstructorHelpers::FObjectFinder<UMaterial> TempMaterial(TEXT(
		"/Game/ItemResource/RedTurtle/MT_RedTurtle.MT_RedTurtle"));
	if (TempMesh.Succeeded() && TempMaterial.Succeeded())
	{
		MeshComp->SetStaticMesh(TempMesh.Object);
		MeshComp->SetMaterial(0, TempMaterial.Object);
	}
}

// Called when the game starts or when spawned
void ARedTurtleItem::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARedTurtleItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//타깃 액터 유효
	if (TargetActor)
	{
		//타깃 액터와의 거리
		const FVector Diff = TargetActor->GetActorLocation() - GetActorLocation();
		if (const float Length = Diff.Length(); Length < DetectLength)
		{
			//타깃 액터를 향해 회전
			MoveComp->SLerpVelocity(Diff.GetSafeNormal());
			return;
		}
	}

	//트래킹 모드 확인
	if (TrackingMode == ETrackingMode::Inactive)
		return;

	//스플라인 확인
	if (SplineComp == nullptr)
		return;

	//거리에 따른 속도 보간
	const FVector FaceLocation = GetActorLocation() + MoveComp->Velocity.GetSafeNormal();
	const FVector ClosestLocation = SplineComp->FindLocationClosestToWorldLocation(FaceLocation, ESplineCoordinateSpace::World);
	const FVector Diff = ClosestLocation - GetActorLocation();
	if (const float Length = Diff.Length(); Length < 300)
	{
		//거리가 충분히 가까우므로 스플라인의 방향을 따라 속도 보간
		const FVector ClosestDirection = SplineComp->FindDirectionClosestToWorldLocation(FaceLocation, ESplineCoordinateSpace::World);
		MoveComp->SLerpVelocity(ClosestDirection);
	}
	else
	{
		//거리가 아직 있으므로 스플라인을 향해 속도 보간
		MoveComp->SLerpVelocity(Diff.GetSafeNormal());
	}
}

