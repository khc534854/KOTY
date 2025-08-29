// Fill out your copyright notice in the Description page of Project Settings.

#include "MushroomSmallItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Item/Component/KotyItemHitComponent.h"
#include "Item/Component/KotyItemHoldComponent.h"
#include "Item/Component/KotyMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

class UKotyItemHitComponent;

AMushroomSmallItem::AMushroomSmallItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//메시 컴포넌트 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(FName("NoCollision"), false);
	MeshComp->SetupAttachment(GetRootComponent());

	//스태틱 메시 로드
	if (const ConstructorHelpers::FObjectFinder<UStaticMesh> Finder(TEXT("/Game/Item/Mushroom/Model/SM_Mushroom.SM_Mushroom"));
		Finder.Succeeded())
	{
		MeshComp->SetStaticMesh(Finder.Object);
	}

	//머터리얼 로드
	if (const ConstructorHelpers::FObjectFinder<UMaterial> Finder(TEXT("/Game/Item/Mushroom/Model/MT_Mushroom.MT_Mushroom"));
		Finder.Succeeded())
	{
		MeshComp->SetMaterial(0, Finder.Object);
	}

	//사용 사운드 베이스 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_MushroomSmallDash.SW_MushroomSmallDash"));
		Finder.Succeeded())
	{
		UseSound = Finder.Object;
	}

	//크기에 맞춰 변경
	SphereComp->SetSphereRadius(30);
	HitComp->SetSphereRadius(60);
}

void AMushroomSmallItem::BeginPlay()
{
	Super::BeginPlay();
}

void AMushroomSmallItem::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (MoveComp->IsOnSimulate())
	{
		//충돌 상대가 아이템 충돌체였다
		if (const UKotyItemHitComponent* OtherHitComp = OtherActor->GetComponentByClass<UKotyItemHitComponent>())
		{
			//요청
			RequestApplyItemEffectToOtherHitComp(OtherHitComp);
		}	
	}
}

void AMushroomSmallItem::OnSimulateBegin()
{
	Super::OnSimulateBegin();
}

void AMushroomSmallItem::ApplyItemEffect(AActor* TargetActor)
{
	Super::ApplyItemEffect(TargetActor);

	if (UseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), UseSound, GetActorLocation());
	}

	UE_LOG(LogTemp, Warning, TEXT("SmallMushroom Item Used by %s"), *TargetActor->GetName());
	
	this->Destroy();
}

void AMushroomSmallItem::OnUseItem(UKotyItemHoldComponent* HoldComp)
{
	Super::OnUseItem(HoldComp);

	//가속 효과 발생
	if (const auto* OtherHitComp = HoldComp->GetOwner()->GetComponentByClass<UKotyItemHitComponent>())
	{
		FItemEffect ItemEffectDelegate;
		ItemEffectDelegate.BindDynamic(this, &AMushroomSmallItem::ApplyItemEffect);
		OtherHitComp->OnRequestApplyEffectFromItem(ItemEffectDelegate, this);
	}
}

void AMushroomSmallItem::OnLoseItem(UKotyItemHoldComponent* HoldComp)
{
	Super::OnLoseItem(HoldComp);
	
	//떼어내기
	this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	this->SetActorLocation(HoldComp->GetShootLocation());

	//사출 속도
	const FVector Forward = GetOwner()->GetActorForwardVector();
	const FVector Right = GetOwner()->GetActorRightVector();
	const FVector Shoot = Forward.RotateAngleAxis(45, Right);
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