// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#pragma once
#include "Particles/ParticleModule.h"
#include "ParticleModuleParameterBase.generated.h"

UCLASS(editinlinenew, hidecategories=Object, abstract, meta=(DisplayName = "Parameter"))
class UParticleModuleParameterBase : public UParticleModule
{
	GENERATED_BODY()
public:
	UParticleModuleParameterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual bool CanTickInAnyThread() override
	{
		return false;
	}

};

