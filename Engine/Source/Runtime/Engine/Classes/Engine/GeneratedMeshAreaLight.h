// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/SpotLight.h"
#include "GeneratedMeshAreaLight.generated.h"

/**
 * Implements a light that is created after a lighting build with Lightmass and handles mesh area light influence on dynamic objects.
 */
UCLASS(notplaceable, MinimalAPI)
class AGeneratedMeshAreaLight
	: public ASpotLight
{
	GENERATED_BODY()
public:
	ENGINE_API AGeneratedMeshAreaLight(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
