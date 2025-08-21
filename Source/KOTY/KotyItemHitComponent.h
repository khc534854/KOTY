// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KotyItemHitComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KOTY_API UKotyItemHitComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UKotyItemHitComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnRegister() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UBoxComponent> BoxComp;

	UFUNCTION()
	void Hit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
