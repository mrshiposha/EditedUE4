// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#pragma once
#include "Particles/Velocity/ParticleModuleVelocityBase.h"
#include "ParticleModuleVelocityInheritParent.generated.h"

UCLASS(editinlinenew, hidecategories=Object, MinimalAPI, meta=(DisplayName = "Inherit Parent Velocity"))
class UParticleModuleVelocityInheritParent : public UParticleModuleVelocityBase
{
	GENERATED_BODY()
public:
	ENGINE_API UParticleModuleVelocityInheritParent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 *	The scale to apply tot he parent velocity prior to adding it to the particle velocity during spawn.
	 *	Value is retrieved using the EmitterTime of the emitter.
	 */
	UPROPERTY(EditAnywhere, Category=Velocity)
	struct FRawDistributionVector Scale;

	/** Initializes the default values for this property */
	void InitializeDefaults();

	//Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
	virtual void PostInitProperties() override;
	// End UObject Interface

	// Begin UParticleModule Interface
	virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;
	// Begin UParticleModule Interface
};



