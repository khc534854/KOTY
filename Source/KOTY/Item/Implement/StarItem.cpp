// Fill out your copyright notice in the Description page of Project Settings.

#include "StarItem.h"

#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Item/Component/KotyItemHitComponent.h"
#include "Item/Component/KotyItemHoldComponent.h"
#include "Item/Component/KotyMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AStarItem::AStarItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//메시 컴포넌트 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(FName("NoCollision"), false);
	MeshComp->SetupAttachment(GetRootComponent());

	//스태틱 메시 로드
	if (const ConstructorHelpers::FObjectFinder<UStaticMesh> Finder(TEXT("/Game/Item/Star/Model/SM_Star.SM_Star"));
		Finder.Succeeded())
	{
		MeshComp->SetStaticMesh(Finder.Object);
	}

	//머터리얼 로드
	if (const ConstructorHelpers::FObjectFinder<UMaterial> Finder(TEXT("/Game/Item/Star/Model/MT_Star.MT_Star"));
		Finder.Succeeded())
	{
		MeshComp->SetMaterial(0, Finder.Object);
	}

	//사용 사운드 베이스 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_StarRunning.SW_StarRunning"));
		Finder.Succeeded())
	{
		UseSound = Finder.Object;
	}

	//크기에 맞춰 변경
	SphereComp->SetSphereRadius(70);
	HitComp->SetSphereRadius(100);
}

void AStarItem::BeginPlay()
{
	Super::BeginPlay();
}

void AStarItem::OnSimulateBegin()
{
	Super::OnSimulateBegin();
}

void AStarItem::ApplyItemEffect(AActor* TargetActor)
{
	Super::ApplyItemEffect(TargetActor);

	if (UseSound)
	{
		UGameplayStatics::SpawnSoundAttached(
			UseSound,
			TargetActor->GetRootComponent(),
			NAME_None,
			FVector(ForceInit),
			FRotator(ForceInit),
			EAttachLocation::Type::KeepRelativeOffset,
			false,
			1,
			1,
			0,
			SoundAttenuation);	
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Star Item Used by %s"), *TargetActor->GetName());

	this->Destroy();
}

void AStarItem::OnUseItem(UKotyItemHoldComponent* HoldComp)
{
	Super::OnUseItem(HoldComp);

	//무적 효과 발생
	if (const auto* OtherHitComp = HoldComp->GetOwner()->GetComponentByClass<UKotyItemHitComponent>())
	{
		FItemEffect ItemEffectDelegate;
		ItemEffectDelegate.BindDynamic(this, &AStarItem::ApplyItemEffect);
		OtherHitComp->OnRequestApplyEffectFromItem(ItemEffectDelegate, this);
	}
}

void AStarItem::OnLoseItem(UKotyItemHoldComponent* HoldComp)
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