// Fill out your copyright notice in the Description page of Project Settings.


#include "GreenTurtleItem.h"

#include "KotyItemHitComponent.h"
#include "KotyMovementComponent.h"

class UKotyItemHitComponent;

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

void AGreenTurtleItem::BeginPlay()
{
	Super::BeginPlay();

	//회전 방향 결정
	RotationDir = FMath::RandRange(0, 1);
}

void AGreenTurtleItem::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	//충돌 상대가 다른 아이템이었다
	if (AKotyItemBase* OtherItem = Cast<AKotyItemBase>(OtherActor))
	{
		this->Destroy();
		UE_LOG(LogTemp, Log, TEXT("Item Hit with OtherItem!"));
	}

	//충돌 상대가 아이템 충돌체였다
	if (const UKotyItemHitComponent* OtherHitComp = OtherActor->GetComponentByClass<UKotyItemHitComponent>())
	{
		//오너를 대상으로 아이템 효과 적용
		UE_LOG(LogTemp, Log, TEXT("Apply Item Effect to OtherKart!"));

		//델리게이트 전달
		FItemEffect ItemEffectDelegate;
		ItemEffectDelegate.BindDynamic(this, &AKotyItemBase::ApplyItemEffect);
		OtherHitComp->OnRequestApplyEffectFromItem(ItemEffectDelegate, this);
		
		//파괴
		this->Destroy();
	}
}

void AGreenTurtleItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//메시 컴포넌트의 머리가 중력의 반대 방향을 향하도록 업데이트
	MeshComp->AddWorldRotation(FQuat::FindBetweenVectors(MeshComp->GetUpVector(), -MoveComp->GetGravityDir()) * DeltaTime);

	//메시 컴포넌트가 머리 방향을 축으로 회전하도록 업데이트
	const FVector Dir = RotationDir ? MeshComp->GetRightVector() : MeshComp->GetRightVector() * -1;
	const FVector SlerpDir = FVector::SlerpVectorToDirection(MeshComp->GetForwardVector(), Dir, 0.05);
	const FQuat RotationQuat = FQuat::FindBetweenVectors(MeshComp->GetForwardVector(), SlerpDir);
	MeshComp->AddWorldRotation(RotationQuat * DeltaTime);
}

void AGreenTurtleItem::ApplyItemEffect(AActor* TargetActor)
{
	Super::ApplyItemEffect(TargetActor);

	UE_LOG(LogTemp, Display, TEXT("GreenTurtle Applyed to %s!"), *TargetActor->GetName());
}



