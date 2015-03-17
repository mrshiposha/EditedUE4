// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#pragma once
#include "GameFramework/Volume.h"
#include "ProceduralFoliageBlockingVolume.generated.h"

class AProceduralFoliageActor;

/** An invisible volume used to block ProceduralFoliage instances from being spawned. */
UCLASS(MinimalAPI)
class AProceduralFoliageBlockingVolume : public AVolume
{
	GENERATED_BODY()
public:
	FOLIAGE_API AProceduralFoliageBlockingVolume(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(Category = ProceduralFoliage, EditAnywhere)
	AProceduralFoliageActor* ProceduralFoliageActor;
};



