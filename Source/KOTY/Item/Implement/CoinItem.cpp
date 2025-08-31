// Fill out your copyright notice in the Description page of Project Settings.


#include "CoinItem.h"

#include "C_KartBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Item/Component/KotyItemHitComponent.h"
#include "Item/Component/KotyItemHoldComponent.h"
#include "Item/Component/KotyMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "PlayerKart/C_PlayerKart.h"
#include "PlayerKart/C_PlayerController.h"
#include "UI/C_RaceWidget.h"

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

	//사용 사운드 베이스 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_UseCoin.SW_UseCoin"));
		Finder.Succeeded())
	{
		UseSound = Finder.Object;
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

	if (MoveComp->IsOnSimulate())
	{
		//충돌 상대가 아이템 충돌체였다
		if (UKotyItemHitComponent* OtherHitComp = OtherActor->GetComponentByClass<UKotyItemHitComponent>())
		{
			//요청
			RequestApplyItemEffectToOtherHitComp(OtherHitComp);
		}
	}

	if (UKotyItemHoldComponent* OtherHoldComp = OtherActor->GetComponentByClass<UKotyItemHoldComponent>())
	{
		OnUseItem(OtherHoldComp);
	}
}

void ACoinItem::ApplyItemEffect(AActor* TargetActor)
{
	UE_LOG(LogTemp, Log, TEXT("Coin Item Used To %s"), *TargetActor->GetName());
	
	//상대가 카트라면
	if (AC_KartBase* Kart = Cast<AC_KartBase>(TargetActor))
	{
		//최대 속도 상승
		Kart->CoinCount = FMath::Clamp(Kart->CoinCount + 1, 0, 10);
		
		if (TargetActor->IsA<AC_PlayerKart>())
		{
			auto* pc = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController());
			pc->CurrentHUD->ChangeCoinNum();
		}
	}

	//아이템 효과 적용 사운드 재생
	if (ApplyedSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ApplyedSound, TargetActor->GetActorLocation(), TargetActor->GetActorRotation(), 50, 1, 0, SoundAttenuation);
	}

	//효과를 다한 이 아이템 파괴
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ACoinItem::Respawn, 5.f, false);
	//this->Destroy();
}

void ACoinItem::OnUseItem(UKotyItemHoldComponent* HoldComp)
{
	Super::OnUseItem(HoldComp);

	//최고 속도 상승 효과 발생
	if (auto* OtherHitComp = HoldComp->GetOwner()->GetComponentByClass<UKotyItemHitComponent>())
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

void ACoinItem::Respawn()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}
