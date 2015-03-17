// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

/**
 *	Bone Mask Filter data holder class for A2Node_PerBone. This has to be in Engine, not Editor class since we need this data to exists in runtime
 *
 */

#include "BoneMaskFilter.generated.h"

USTRUCT()
struct FBranchFilter
{
	GENERATED_BODY()

	/** Bone Name to filter **/
	UPROPERTY(EditAnywhere, Category=Branch)
	FName BoneName;

	/** Blend Depth **/
	UPROPERTY(EditAnywhere, Category=Branch)
	int32 BlendDepth;
};

USTRUCT()
struct FInputBlendPose
{
	GENERATED_BODY()

	/** Bone Name to filter **/
	UPROPERTY(EditAnywhere, Category=Filter)
	TArray<FBranchFilter> BranchFilters;
};

UCLASS(hidecategories=Object, BlueprintType,MinimalAPI)
class UBoneMaskFilter : public UObject
{
	GENERATED_BODY()
public:
	ENGINE_API UBoneMaskFilter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, Category=BoneMask)
	TArray<FInputBlendPose>	BlendPoses;
};



