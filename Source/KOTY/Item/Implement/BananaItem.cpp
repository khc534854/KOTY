// Fill out your copyright notice in the Description page of Project Settings.

#include "BananaItem.h"

#include "C_KartBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Item/Component/KotyItemHoldComponent.h"
#include "Item/Component/KotyMovementComponent.h"
#include "Kismet/GameplayStatics.h"
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

	//스핀 사운드 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_Spin.SW_Spin"));
		Finder.Succeeded())
	{
		SpinSound = Finder.Object;
	}

	//사용 사운드 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_Swoosh.SW_Swoosh"));
		Finder.Succeeded())
	{
		UseSound = Finder.Object;
	}

	//파괴 사운드 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_Destroyed.SW_Destroyed"));
		Finder.Succeeded())
	{
		DestroySound = Finder.Object;
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

	UE_LOG(LogTemp, Log, TEXT("%d"), MoveComp->IsOnSimulate());
	
	if (MoveComp->IsOnSimulate())
	{
		UE_LOG(LogTemp, Log, TEXT("AAA"));

		//충돌 상대가 아이템 충돌체였다
		if (UKotyItemHitComponent* OtherHitComp = OtherActor->GetComponentByClass<UKotyItemHitComponent>())
		{
			//요청
			RequestApplyItemEffectToOtherHitComp(OtherHitComp);
		}	
	}
}

void ABananaItem::OnSimulateBegin()
{
	Super::OnSimulateBegin();

	//사용 사운드 재생
	if (UseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), UseSound, GetActorLocation(), GetActorRotation(), 1, 1, 0, SoundAttenuation);
	}
}

void ABananaItem::ApplyItemEffect(AActor* TargetActor)
{
	Super::ApplyItemEffect(TargetActor);

	UE_LOG(LogTemp, Log, TEXT("Banana Item Applyed To %s"), *TargetActor->GetName());

	//상대가 카트라면 스턴을 먹인다
	if (AC_KartBase* Kart = Cast<AC_KartBase>(TargetActor))
	{
		//스턴
		Kart->Stun();

		//폭발 사운드 재생
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SpinSound, GetActorLocation(), GetActorRotation(), 1, 1, 0, SoundAttenuation);
	}

	//효과를 다한 이 아이템 파괴
	this->Destroy();
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
	FVector Velocity = Shoot * 4000; 

	//합산
	if (AC_KartBase* Temp = Cast<AC_KartBase>(HoldComp->GetOwner()))
	{
		Velocity += Temp->CurVelocity;
	}
	
	//사출
	MoveComp->ThrowLinearDrag(
		true,
		-HoldComp->GetUpVector(), //FVector::DownVector,
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