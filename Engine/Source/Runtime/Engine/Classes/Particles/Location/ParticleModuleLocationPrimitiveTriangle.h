// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

//=============================================================================
// ParticleModuleLocationPrimitiveTriangle
// Location primitive spawning within a triangle.
//=============================================================================

#pragma once
#include "Particles/Location/ParticleModuleLocationBase.h"
#include "ParticleModuleLocationPrimitiveTriangle.generated.h"

UCLASS(editinlinenew, hidecategories=Object, MinimalAPI, meta=(DisplayName = "Triangle"))
class UParticleModuleLocationPrimitiveTriangle : public UParticleModuleLocationBase
{
	GENERATED_BODY()
public:
	ENGINE_API UParticleModuleLocationPrimitiveTriangle(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	UPROPERTY(EditAnywhere, Category=Location)
	struct FRawDistributionVector StartOffset;

	UPROPERTY(EditAnywhere, Category=Location)
	struct FRawDistributionFloat Height;

	UPROPERTY(EditAnywhere, Category=Location)
	struct FRawDistributionFloat Angle;

	UPROPERTY(EditAnywhere, Category=Location)
	struct FRawDistributionFloat Thickness;
	
	/** Initializes the default values for this property */
	void InitializeDefaults();

	// Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
	virtual void PostInitProperties() override;
	// End UObject Interface

	// Begin UParticleModule Interface
	virtual void	Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;
	virtual void	Render3DPreview(FParticleEmitterInstance* Owner, const FSceneView* View,FPrimitiveDrawInterface* PDI) override;
	// End UParticleModule Interface

	/**
	 *	Extended version of spawn, allows for using a random stream for distribution value retrieval
	 *
	 *	@param	Owner				The particle emitter instance that is spawning
	 *	@param	Offset				The offset to the modules payload data
	 *	@param	SpawnTime			The time of the spawn
	 *	@param	InRandomStream		The random stream to use for retrieving random values
	 */
	void SpawnEx(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, struct FRandomStream* InRandomStream, FBaseParticle* ParticleBase);
};



