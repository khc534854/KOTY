// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Base/KotyItemBase.h"
#include "SquidItem.generated.h"

UCLASS()
class KOTY_API ASquidItem : public AKotyItemBase
{
	GENERATED_BODY()

public:
	ASquidItem();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UTimelineComponent> TimelineComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialParameterCollection> MaterialParameterCollection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialParameterCollectionInstance> CollectionInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCurveFloat> CurveFloat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SplatterDuration;

public:
	virtual void ApplyItemEffect(AActor* Player) override;

	virtual void OnUseItem(UKotyItemHoldComponent* HoldComp) override;

	virtual void OnLoseItem(UKotyItemHoldComponent* HoldComp) override;

	UFUNCTION()
	void UpdateSplatter(float Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MeshComp;
};
