// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

//=============================================================================
// PrecomputedVisibilityOverrideVolume:  Overrides visibility for a set of actors
//=============================================================================

#pragma once
#include "GameFramework/Volume.h"
#include "PrecomputedVisibilityOverrideVolume.generated.h"

UCLASS(hidecategories=(Collision, Brush, Attachment, Physics, Volume), MinimalAPI)
class APrecomputedVisibilityOverrideVolume : public AVolume
{
	GENERATED_BODY()
public:
	ENGINE_API APrecomputedVisibilityOverrideVolume(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Array of actors that will always be considered visible by Precomputed Visibility when viewed from inside this volume. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=PrecomputedVisibilityOverrideVolume)
	TArray<class AActor*> OverrideVisibleActors;

	/** Array of actors that will always be considered invisible by Precomputed Visibility when viewed from inside this volume. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=PrecomputedVisibilityOverrideVolume)
	TArray<class AActor*> OverrideInvisibleActors;

	/** Array of level names whose actors will always be considered invisible by Precomputed Visibility when viewed from inside this volume. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=PrecomputedVisibilityOverrideVolume)
	TArray<FName> OverrideInvisibleLevels;
};



