// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Interface.h"
#include "RuntimeMovieScenePlayerInterface.generated.h"

UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class URuntimeMovieScenePlayerInterface : public UInterface
{
	GENERATED_BODY()
public:
	ENGINE_API URuntimeMovieScenePlayerInterface(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

class IRuntimeMovieScenePlayerInterface
{
	GENERATED_BODY()
public:

	/**
	 * Ticks this MovieScenePlayer.  Should be called every frame.
	 *
	 * @param	DeltaSeconds	Amount of time that has passed since the last tick
	 */
	virtual void Tick( const float DeltaSeconds ) { }
};

