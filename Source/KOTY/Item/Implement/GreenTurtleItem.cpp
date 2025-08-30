// Fill out your copyright notice in the Description page of Project Settings.

#include "GreenTurtleItem.h"

#include "C_KartBase.h"
#include "Components/AudioComponent.h"
#include "Item/Component/KotyItemHitComponent.h"
#include "Item/Component/KotyItemHoldComponent.h"
#include "Item/Component/KotyMovementComponent.h"
#include "Kismet/GameplayStatics.h"

class UKotyItemHitComponent;

AGreenTurtleItem::AGreenTurtleItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//메시 컴포넌트 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(FName("NoCollision"), false);
	MeshComp->SetupAttachment(GetRootComponent());

	//스태틱 메시 로드
	if (const ConstructorHelpers::FObjectFinder<UStaticMesh> Finder(TEXT("/Game/Item/GreenTurtle/Model/SM_GreenTurtle.SM_GreenTurtle"));
		Finder.Succeeded())
	{
		MeshComp->SetStaticMesh(Finder.Object);
	}

	//머터리얼 로드
	if (const ConstructorHelpers::FObjectFinder<UMaterial> Finder(TEXT("/Game/Item/GreenTurtle/Model/MT_GreenTurtle.MT_GreenTurtle"));
		Finder.Succeeded())
	{
		MeshComp->SetMaterial(0, Finder.Object);
	}

	//사용 사운드 베이스 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_Swoosh.SW_Swoosh"));
		Finder.Succeeded())
	{
		UseSound = Finder.Object;
	}

	//이동 사운드 베이스 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_GreenTurtle.SW_GreenTurtle"));
		Finder.Succeeded())
	{
		MovingSound = Finder.Object;
	}

	//파괴 사운드 베이스 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_Destroyed.SW_Destroyed"));
		Finder.Succeeded())
	{
		DestroySound = Finder.Object;
	}

	//오디오 컴포넌트 초기화
	AudioComp->SetSound(MovingSound);
}

void AGreenTurtleItem::BeginPlay()
{
	Super::BeginPlay();

	//델리게이트 등록
	MoveComp->OnSimulateBeginEventDispatcher.AddUFunction(this, FName("OnSimulateBegin"));
}

void AGreenTurtleItem::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (MoveComp->IsOnSimulate())
	{
		//충돌 상대가 다른 아이템이었다
		if (AKotyItemBase* OtherItem = Cast<AKotyItemBase>(OtherActor))
		{
			this->Destroy();
			UE_LOG(LogTemp, Log, TEXT("Item Hit with OtherItem!"));
		}

		//충돌 상대가 아이템 충돌체였다
		if (UKotyItemHitComponent* OtherHitComp = OtherActor->GetComponentByClass<UKotyItemHitComponent>())
		{
			//요청
			RequestApplyItemEffectToOtherHitComp(OtherHitComp);
		}	
	}
}

void AGreenTurtleItem::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MoveComp->IsOnSimulate())
	{
		//메시 컴포넌트가 머리 방향을 축으로 회전하도록 업데이트
		const FVector Dir = RotationDir ? MeshComp->GetRightVector() : MeshComp->GetRightVector() * -1;
		const FVector SlerpDir = FVector::SlerpVectorToDirection(MeshComp->GetForwardVector(), Dir, 0.1);
		const FQuat RotationQuat = FQuat::FindBetweenVectors(MeshComp->GetForwardVector(), SlerpDir);
		MeshComp->AddWorldRotation(RotationQuat * DeltaTime);	
	}
}

void AGreenTurtleItem::ApplyItemEffect(AActor* TargetActor)
{
	UE_LOG(LogTemp, Display, TEXT("GreenTurtle Applyed To %s!"), *TargetActor->GetName());

	//상대가 카트라면
	if (AC_KartBase* Kart = Cast<AC_KartBase>(TargetActor))
	{
		//스턴
		Kart->Stun();
	}

	//아이템 효과 적용 사운드 재생
	if (ApplyedSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ApplyedSound, TargetActor->GetActorLocation(), TargetActor->GetActorRotation(), 1, 1, 0, SoundAttenuation);
	}

	//효과를 다한 이 아이템 파괴
	this->Destroy();
}

void AGreenTurtleItem::OnSimulateBegin()
{
	Super::OnSimulateBegin();

	UE_LOG(LogTemp, Log, TEXT("OnSimulateBegin!"));

	//사용 사운드 재생
	if (UseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), UseSound, GetActorLocation(), GetActorRotation(), 1, 1, 0, SoundAttenuation);
	}
	
	//회전 방향 결정
	RotationDir = FMath::RandRange(0, 1);

	//오디오 재생
	AudioComp->Play();
}

void AGreenTurtleItem::OnUseItem(UKotyItemHoldComponent* HoldComp)
{
	Super::OnUseItem(HoldComp);

	//떼어내기
	this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	this->SetActorLocation(HoldComp->GetShootLocation());
	
	//사출 속도
	const FVector Forward = HoldComp->GetOwner()->GetActorForwardVector();
	const FVector Right = HoldComp->GetOwner()->GetActorRightVector();
	const FVector Shoot = Forward.RotateAngleAxis(-30, Right);
	FVector Velocity = Shoot * 1000;
	
	//사출
	MoveComp->ThrowConstantHorizon(
		true,
		FVector::DownVector,
		6000,
		6000,
		4.0,
		Velocity,
		0.25);
}

void AGreenTurtleItem::OnLoseItem(UKotyItemHoldComponent* HoldComp)
{
	Super::OnLoseItem(HoldComp);

	//파괴
	this->Destroy();
}