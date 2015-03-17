// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AI/Navigation/NavRelevantComponent.h"
#include "NavModifierComponent.generated.h"

class UNavArea;
struct FCompositeNavModifier;

UCLASS(ClassGroup = (Navigation), meta = (BlueprintSpawnableComponent), hidecategories = (Activation))
class UNavModifierComponent : public UNavRelevantComponent
{
	GENERATED_BODY()
public:
	UNavModifierComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, Category = Navigation)
	TSubclassOf<UNavArea> AreaClass;

	virtual void CalcBounds() override;
	virtual void GetNavigationData(FNavigationRelevantData& Data) const override;

protected:
	FVector ObstacleExtent;
};
