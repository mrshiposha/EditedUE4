// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

/**
 * Welds selected objects.
 */

#pragma once
#include "GeomModifier_Weld.generated.h"

UCLASS()
class UGeomModifier_Weld : public UGeomModifier_Edit
{
	GENERATED_BODY()
public:
	UGeomModifier_Weld(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	// Begin UGeomModifier Interface
	virtual bool Supports() override;
protected:
	virtual bool OnApply() override;
	// End UGeomModifier Interface
};



