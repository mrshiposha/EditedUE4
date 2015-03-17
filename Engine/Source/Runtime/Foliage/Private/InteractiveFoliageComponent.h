// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#pragma once
#include "InteractiveFoliageComponent.generated.h"

UCLASS(hidecategories=Object)
class UInteractiveFoliageComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
public:
	UInteractiveFoliageComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


public:
	class FInteractiveFoliageSceneProxy* FoliageSceneProxy;

	// Begin UPrimitiveComponent Interface
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	// End UPrimitiveComponent Interface

	// Begin UActorComponent Interface
	virtual void DestroyRenderState_Concurrent() override;
	// End UActorComponent Interface

	friend class AInteractiveFoliageActor;
};

