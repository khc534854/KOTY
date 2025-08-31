// Fill out your copyright notice in the Description page of Project Settings.


#include "MushroomBigItem.h"

#include "C_KartBase.h"
#include "Components/SphereComponent.h"
#include "Item/Component/KotyItemHitComponent.h"
#include "Item/Component/KotyItemHoldComponent.h"
#include "Item/Component/KotyMovementComponent.h"
#include "Kismet/GameplayStatics.h"

class UKotyItemHitComponent;

AMushroomBigItem::AMushroomBigItem()
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
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_MushroomBigDash.SW_MushroomBigDash"));
		Finder.Succeeded())
	{
		UseSound = Finder.Object;
	}

	//크기에 맞춰 변경
	MeshComp->SetWorldScale3D(FVector(2, 2, 2));
	SphereComp->SetSphereRadius(60);
	HitComp->SetSphereRadius(80);
}

void AMushroomBigItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMushroomBigItem::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (MoveComp->IsOnSimulate())
	{
		//충돌 상대가 아이템 충돌체였다
		if (UKotyItemHitComponent* OtherHitComp = OtherActor->GetComponentByClass<UKotyItemHitComponent>())
		{
			//요청
			RequestApplyItemEffectToOtherHitComp(OtherHitComp);
		}	
	}
}

void AMushroomBigItem::ApplyItemEffect(AActor* TargetActor)
{
	UE_LOG(LogTemp, Warning, TEXT("BigMushroom Item Applyed To %s"), *TargetActor->GetName());
	
	//상대가 카트라면
	if (AC_KartBase* Kart = Cast<AC_KartBase>(TargetActor))
	{
		Kart->StartAddSpeed(4500);
	}

	//아이템 효과 적용 사운드 재생
	if (ApplyedSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ApplyedSound, TargetActor->GetActorLocation(), TargetActor->GetActorRotation(), 1, 1, 0, SoundAttenuation);
	}

	//효과를 다한 이 아이템 파괴
	this->Destroy();
}

void AMushroomBigItem::OnUseItem(UKotyItemHoldComponent* HoldComp)
{
	Super::OnUseItem(HoldComp);

	//부스터 효과 발생
	if (auto* OtherHitComp = HoldComp->GetOwner()->GetComponentByClass<UKotyItemHitComponent>())
	{
		//요청
		RequestApplyItemEffectToOtherHitComp(OtherHitComp);
	}
}

void AMushroomBigItem::OnLoseItem(UKotyItemHoldComponent* HoldComp)
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