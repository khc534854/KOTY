// Fill out your copyright notice in the Description page of Project Settings.


#include "GreenTurtleItem.h"

#include "KotyItemHitComponent.h"
#include "KotyMovementComponent.h"
#include "Components/AudioComponent.h"

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
	AudioComp->SetAttenuationSettings(SoundAttenuation);
}

void AGreenTurtleItem::BeginPlay()
{
	Super::BeginPlay();

	//회전 방향 결정
	RotationDir = FMath::RandRange(0, 1);

	//오디오 재생
	AudioComp->Play();
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
	const FVector SlerpDir = FVector::SlerpVectorToDirection(MeshComp->GetForwardVector(), Dir, 0.1);
	const FQuat RotationQuat = FQuat::FindBetweenVectors(MeshComp->GetForwardVector(), SlerpDir);
	MeshComp->AddWorldRotation(RotationQuat * DeltaTime);
}

void AGreenTurtleItem::ApplyItemEffect(AActor* TargetActor)
{
	UE_LOG(LogTemp, Display, TEXT("GreenTurtle Applyed to %s!"), *TargetActor->GetName());
}



