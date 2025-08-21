// Fill out your copyright notice in the Description page of Project Settings.


#include "KotyItemHitComponent.h"

#include "KotyItemBase.h"
#include "Components/BoxComponent.h"


// Sets default values for this component's properties
UKotyItemHitComponent::UKotyItemHitComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//오너의 루트 컴포넌트에 부착
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	//충돌 이벤트 바인드
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &UKotyItemHitComponent::Hit);
}


// Called when the game starts
void UKotyItemHitComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UKotyItemHitComponent::OnRegister()
{
	Super::OnRegister();
}

void UKotyItemHitComponent::Hit(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//충돌 상대를 캐스팅
	if (AKotyItemBase* OtherItem = Cast<AKotyItemBase>(OtherActor))
	{
		UE_LOG(LogTemp, Log, TEXT("Item Hit!"));
	}
}

// Called every frame
void UKotyItemHitComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

