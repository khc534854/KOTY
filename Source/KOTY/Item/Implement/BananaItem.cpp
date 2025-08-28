// Fill out your copyright notice in the Description page of Project Settings.

#include "BananaItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Item/Component/KotyItemHoldComponent.h"
#include "Item/Component/KotyMovementComponent.h"
#include "UObject/ConstructorHelpers.h"

ABananaItem::ABananaItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//메시 컴포넌트 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(FName("NoCollision"), false);
	MeshComp->SetupAttachment(GetRootComponent());
	
	if (const ConstructorHelpers::FObjectFinder<UStaticMesh> Finder(TEXT("/Game/Item/Banana/Model/SM_Banana.SM_Banana"));
		Finder.Succeeded())
	{
		MeshComp->SetStaticMesh(Finder.Object);
	}
	
	if (const ConstructorHelpers::FObjectFinder<UMaterial> Finder(TEXT("/Game/Item/Banana/Model/MT_Banana.MT_Banana"));
		Finder.Succeeded())
	{
		MeshComp->SetMaterial(0, Finder.Object);
	}

	//크기에 맞춰 변경
	SphereComp->SetSphereRadius(30);
	HitComp->SetSphereRadius(60);
}

void ABananaItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABananaItem::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	//미끄러짐 효과 발생
	if (const auto* OtherHitComp = OtherActor->GetComponentByClass<UKotyItemHitComponent>())
	{
		//요청
		RequestApplyItemEffectToOtherHitComp(OtherHitComp);
	}
}

void ABananaItem::ApplyItemEffect(AActor* TargetActor)
{
	Super::ApplyItemEffect(TargetActor);

	UE_LOG(LogTemp, Warning, TEXT("Banana Item Used by %s"), *TargetActor->GetName());
}

void ABananaItem::OnUseItem(UKotyItemHoldComponent* HoldComp)
{
	Super::OnUseItem(HoldComp);

	//떼어내기
	this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	this->SetActorLocation(HoldComp->GetShootLocation());

	//사출 속도
	const FVector Forward = HoldComp->GetOwner()->GetActorForwardVector();
	const FVector Right = HoldComp->GetOwner()->GetActorRightVector();
	const FVector Shoot = Forward.RotateAngleAxis(-45, Right);
	const FVector Velocity = Shoot * 4500;

	//사출
	MoveComp->ThrowLinearDrag(
		true,
		FVector::DownVector,
		6000,
		5.0,
		4.0,
		Velocity,
		0.25);
}

void ABananaItem::OnLoseItem(UKotyItemHoldComponent* HoldComp)
{
	Super::OnLoseItem(HoldComp);

	//떼어내기
	this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	this->SetActorLocation(HoldComp->GetShootLocation());

	//사출 속도
	const FVector Forward = HoldComp->GetOwner()->GetActorForwardVector();
	const FVector Right = HoldComp->GetOwner()->GetActorRightVector();
	const FVector Shoot = Forward.RotateAngleAxis(-45, Right);
	const FVector Velocity = Shoot * 1500;

	//사출
	MoveComp->ThrowLinearDrag(
		true,
		FVector::DownVector,
		6000,
		5.0,
		4.0,
		Velocity,
		0.25);
}