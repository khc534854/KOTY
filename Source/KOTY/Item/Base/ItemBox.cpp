// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemBox.h"

#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Item/Component/KotyItemHitComponent.h"
#include "Item/Component/KotyItemHoldComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AItemBox::AItemBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//충돌체를 루트 컴포넌트로 설정
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetEnableGravity(false);
	SphereComp->SetCollisionProfileName(FName("OverlapAll"), true);
	SphereComp->SetSphereRadius(150);
	SphereComp->ShapeColor = FColor::Blue;
	SetRootComponent(SphereComp);

	//메시 컴포넌트를 루트에 부착
	BoxMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMeshComp"));
	BoxMeshComp->SetCollisionProfileName(FName("NoCollision"), false);
	BoxMeshComp->SetupAttachment(GetRootComponent());

	//메시 컴포넌트를 루트에 부착
	FontMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FontMeshComp"));
	FontMeshComp->SetCollisionProfileName(FName("NoCollision"), false);
	FontMeshComp->SetupAttachment(GetRootComponent());

	//아이템 사운드 감쇠 로드
	if (const ConstructorHelpers::FObjectFinder<USoundAttenuation> Finder(TEXT("/Game/Item/Sound/SA_Item.SA_Item"));
		Finder.Succeeded())
	{
		SoundAttenuation = Finder.Object;
	}
	
	//오디오 컴포넌트 부착
	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComp->SetAutoActivate(false);
	AudioComp->SetAttenuationSettings(SoundAttenuation);
	AudioComp->SetupAttachment(RootComponent);

	//스태틱 메시 로드
	if (const ConstructorHelpers::FObjectFinder<UStaticMesh> Finder(TEXT("/Game/Item/ItemBox/Model/SM_ItemBox.SM_ItemBox"));
		Finder.Succeeded())
	{
		BoxMeshComp->SetStaticMesh(Finder.Object);
	}

	//머터리얼 로드
	if (const ConstructorHelpers::FObjectFinder<UMaterial> Finder(TEXT("/Game/Item/ItemBox/Model/MT_ItemBox.MT_ItemBox"));
		Finder.Succeeded())
	{
		BoxMeshComp->SetMaterial(0, Finder.Object);
	}

	//스태틱 메시 로드
	if (const ConstructorHelpers::FObjectFinder<UStaticMesh> Finder(TEXT("/Game/Item/ItemBox/Model/SM_ItemBoxFont.SM_ItemBoxFont"));
		Finder.Succeeded())
	{
		FontMeshComp->SetStaticMesh(Finder.Object);
	}

	//머터리얼 로드
	if (const ConstructorHelpers::FObjectFinder<UMaterial> Finder(TEXT("/Game/Item/ItemBox/Model/MT_ItemBoxFont.MT_ItemBoxFont"));
		Finder.Succeeded())
	{
		FontMeshComp->SetMaterial(0, Finder.Object);
	}

	//아이템 박스 파괴 사운드 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_ItemBoxCrash.SW_ItemBoxCrash"));
		Finder.Succeeded())
	{
		DestroySound = Finder.Object;
	}

	//아이템 선택 사운드 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_ItemSelect.SW_ItemSelect"));
		Finder.Succeeded())
	{
		UseSound = Finder.Object;
	}

	//아이템 사운드 감쇠 로드
	if (const ConstructorHelpers::FObjectFinder<USoundAttenuation> Finder(TEXT("/Game/Item/Sound/SA_Item.SA_Item"));
		Finder.Succeeded())
	{
		SoundAttenuation = Finder.Object;
	}
}

void AItemBox::BeginPlay()
{
	Super::BeginPlay();

	//플레이어 폰 획득
	if (GetWorld()->GetFirstPlayerController())
	{
		PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	}
	
	RotateAxis = FVector(0.f, 0.f, 1.f);
}

void AItemBox::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (this->IsHidden())
		return;

	//충돌 상대가 아이템 충돌체였다
	if (const UKotyItemHitComponent* OtherHitComp = OtherActor->GetComponentByClass<UKotyItemHitComponent>())
	{
		//아이템 획득
		UE_LOG(LogTemp, Log, TEXT("ItemBox Hit with Kart!"));

		//플레이어의 경우
		if (UKotyItemHoldComponent* OtherHoldComp = OtherActor->GetComponentByClass<UKotyItemHoldComponent>())
		{
			//아이템 선택 사운드 재생
			if (UseSound)
			{
				UGameplayStatics::PlaySound2D(OtherActor, UseSound, 1, 1, 0);	
			}

			//아이템 코드
			switch (ItemCode)
			{
			case EItemList::Start:
			case EItemList::End:
			case EItemList::None:
				{
					//랜덤 아이템 부여
					OtherHoldComp->GetRandomItem();
					break;
				}
			default:
				{
					//특정 아이템 부여
					OtherHoldComp->GetSpecifiedItem(ItemCode);
					break;
				};
			}
		}

		//아이템 박스 파괴 사운드 재생
		if (DestroySound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), DestroySound, GetActorLocation(), GetActorRotation(), 1, 1, 0, SoundAttenuation);
		}

		//이 아이템 박스 파괴(안보이게)
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &AItemBox::Respawn, 5.f, false);

		//this->Destroy();
	}
}

void AItemBox::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	//플레이어 방향을 바라보도록 고정
	if (PlayerPawn)
	{
		const FRotator LookPlayerRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerPawn->GetActorLocation());
		FontMeshComp->SetWorldRotation(LookPlayerRotation);
	}

	// 시간에 따라 회전 축을 동적으로 변경
	const float Time = GetWorld()->GetTimeSeconds();
	RotateAxis = FVector(FMath::Sin(Time * 0.5f), FMath::Cos(Time * 0.5f), FMath::Sin(Time * 0.3f)).GetSafeNormal();

	//아이템 박스는 동적 축을 기준으로 회전
	const FQuat RotationQuat(RotateAxis, FMath::DegreesToRadians(60.0f) * DeltaTime);
	BoxMeshComp->AddLocalRotation(RotationQuat, false);
}

void AItemBox::Respawn()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}
