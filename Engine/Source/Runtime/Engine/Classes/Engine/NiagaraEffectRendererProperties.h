// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "NiagaraEffectRendererProperties.generated.h"

#pragma once
/**
* Emitter properties base class
* Each Effectrenderer derives from this with its own class, and returns it in GetProperties; a copy
* of those specific properties is stored on FNiagaraEmitterProperties (on the effect) for serialization 
* and handed back to the effect renderer on load.
*/
UCLASS()
class UNiagaraEffectRendererProperties : public UObject
{
	GENERATED_BODY()
public:
	UNiagaraEffectRendererProperties(const FObjectInitializer& ObjectInitializer);
	UNiagaraEffectRendererProperties()
	{}

	UPROPERTY()
	FName dummy;
};


