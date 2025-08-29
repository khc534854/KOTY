// Fill out your copyright notice in the Description page of Project Settings.


#include "CoinItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Item/Component/KotyItemHitComponent.h"
#include "Item/Component/KotyItemHoldComponent.h"
#include "Item/Component/KotyMovementComponent.h"
#include "UObject/ConstructorHelpers.h"

ACoinItem::ACoinItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//메시 컴포넌트 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(FName("NoCollision"), false);
	MeshComp->SetupAttachment(GetRootComponent());

	//스태틱 메시 로드
	if (const ConstructorHelpers::FObjectFinder<UStaticMesh> Finder(TEXT("/Game/Item/Coin/Model/SM_Coin.SM_Coin"));
		Finder.Succeeded())
	{
		MeshComp->SetStaticMesh(Finder.Object);
	}

	//머터리얼 로드
	if (const ConstructorHelpers::FObjectFinder<UMaterial> Finder(TEXT("/Game/Item/Coin/Model/MT_Coin.MT_Coin"));
		Finder.Succeeded())
	{
		MeshComp->SetMaterial(0, Finder.Object);
	}

	//크기에 맞춰 변경
	SphereComp->SetSphereRadius(40);
	HitComp->SetSphereRadius(50);
}

void ACoinItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACoinItem::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	//최고속도 상승 효과 발생
	if (const auto* OtherHitComp = OtherActor->GetComponentByClass<UKotyItemHitComponent>())
	{
		//요청
		RequestApplyItemEffectToOtherHitComp(OtherHitComp);
	}
}

void ACoinItem::ApplyItemEffect(AActor* TargetActor)
{
	Super::ApplyItemEffect(TargetActor);
	
	UE_LOG(LogTemp, Log, TEXT("Coin Item Used by %s"), *TargetActor->GetName());

	this->Destroy();
}

void ACoinItem::OnUseItem(UKotyItemHoldComponent* HoldComp)
{
	Super::OnUseItem(HoldComp);

	//최고 속도 상승 효과 발생
	if (const auto* OtherHitComp = HoldComp->GetOwner()->GetComponentByClass<UKotyItemHitComponent>())
	{
		FItemEffect ItemEffectDelegate;
		ItemEffectDelegate.BindDynamic(this, &ACoinItem::ApplyItemEffect);
		OtherHitComp->OnRequestApplyEffectFromItem(ItemEffectDelegate, this);
	}
}

void ACoinItem::OnLoseItem(UKotyItemHoldComponent* HoldComp)
{
	Super::OnLoseItem(HoldComp);

	//떼어내기
	this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	this->SetActorLocation(HoldComp->GetShootLocation());

	//사출 속도
	const FVector Forward = GetOwner()->GetActorForwardVector();
	const FVector Right = GetOwner()->GetActorRightVector();
	const FVector Shoot = Forward.RotateAngleAxis(45, Right);
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