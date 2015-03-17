// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#pragma once
#include "Particles/ParticleModule.h"
#include "ParticleModuleCameraBase.generated.h"

UCLASS(editinlinenew, hidecategories=Object, abstract, meta=(DisplayName = "Camera"))
class UParticleModuleCameraBase : public UParticleModule
{
	GENERATED_BODY()
public:
	UParticleModuleCameraBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool CanTickInAnyThread() override
	{
		return false;
	}
};

