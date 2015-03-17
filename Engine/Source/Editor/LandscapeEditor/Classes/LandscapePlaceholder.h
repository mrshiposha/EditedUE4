// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "LandscapePlaceholder.generated.h"

UCLASS(NotPlaceable)
class ALandscapePlaceholder : public AActor
{
	GENERATED_BODY()
public:
	ALandscapePlaceholder(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//virtual void PostEditMove(bool bFinished) override;
	virtual bool TeleportTo(const FVector& DestLocation, const FRotator& DestRotation, bool bIsATest = false, bool bNoCheck = false) override;

	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
};
