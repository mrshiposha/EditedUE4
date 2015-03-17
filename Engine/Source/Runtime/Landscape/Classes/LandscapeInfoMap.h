// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "LandscapeInfo.h"

#include "LandscapeInfoMap.generated.h"

UCLASS()
class ULandscapeInfoMap : public UObject
{
	GENERATED_BODY()
public:
	ULandscapeInfoMap(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void PostDuplicate(bool bDuplicateForPIE) override;
	void Serialize(FArchive& Ar) override;

	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

	TMap<FGuid, ULandscapeInfo*> Map;
};
