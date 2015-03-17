// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

//=============================================================================
// UAimOffsetBlendSpaceFactoryNew
//=============================================================================

#pragma once
#include "AimOffsetBlendSpaceFactoryNew.generated.h"

UCLASS(hidecategories=Object, MinimalAPI)
class UAimOffsetBlendSpaceFactoryNew : public UBlendSpaceFactoryNew
{
	GENERATED_BODY()
public:
	UNREALED_API UAimOffsetBlendSpaceFactoryNew(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class,UObject* InParent,FName Name,EObjectFlags Flags,UObject* Context,FFeedbackContext* Warn) override;
	// Begin UFactory Interface
};



