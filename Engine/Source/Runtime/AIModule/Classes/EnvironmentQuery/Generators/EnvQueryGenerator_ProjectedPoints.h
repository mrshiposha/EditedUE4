// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "AI/Navigation/NavigationData.h"
#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvQueryGenerator_ProjectedPoints.generated.h"

class ANavigationData;

UCLASS(Abstract)
class AIMODULE_API UEnvQueryGenerator_ProjectedPoints : public UEnvQueryGenerator
{
	GENERATED_BODY()
public:
	UEnvQueryGenerator_ProjectedPoints(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** trace params */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FEnvTraceData ProjectionData;

	struct FSortByHeight
	{
		float OriginalZ;

		FSortByHeight(const FVector& OriginalPt) : OriginalZ(OriginalPt.Z) {}

		FORCEINLINE bool operator()(const FNavLocation& A, const FNavLocation& B) const
		{
			return FMath::Abs(A.Location.Z - OriginalZ) < FMath::Abs(B.Location.Z - OriginalZ);
		}
	};

	/** project all points in array and remove those outside navmesh */
	virtual void ProjectAndFilterNavPoints(TArray<FNavLocation>& Points, FEnvQueryInstance& QueryInstance) const;

	/** store points as generator's result */
	virtual void StoreNavPoints(const TArray<FNavLocation>& Points, FEnvQueryInstance& QueryInstance) const;
};
