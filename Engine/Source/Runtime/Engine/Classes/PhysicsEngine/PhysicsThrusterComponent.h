// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#pragma once
#include "PhysicsThrusterComponent.generated.h"

/** 
 *	Used with objects that have physics to apply a force down the negative-X direction
 *	ie. point X in the direction you want the thrust in.
 */
UCLASS(hidecategories=(Object, Mobility, LOD), ClassGroup=Physics, showcategories=Trigger, MinimalAPI, meta=(BlueprintSpawnableComponent))
class UPhysicsThrusterComponent : public USceneComponent
{
	GENERATED_BODY()
public:
	ENGINE_API UPhysicsThrusterComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Strength of thrust force applied to the base object. */
	UPROPERTY(BlueprintReadWrite, interp, Category=Physics)
	float ThrustStrength;

	// Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	// End UActorComponent Interface
};



