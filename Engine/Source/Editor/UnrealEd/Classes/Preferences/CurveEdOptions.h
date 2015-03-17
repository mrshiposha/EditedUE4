// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

//=============================================================================
// CurveEdOptions
//
// A configuration class that holds information for the setup of the CurveEd.
// Supplied so that the editor 'remembers' the last setup the user had.
//=============================================================================

#pragma once
#include "CurveEdOptions.generated.h"

UCLASS(hidecategories=Object, config=EditorUserSettings)
class UNREALED_API UCurveEdOptions : public UObject
{
	GENERATED_BODY()
public:
	UCurveEdOptions(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, config, Category=Options)
	float MinViewRange;

	UPROPERTY(EditAnywhere, config, Category=Options)
	float MaxViewRange;

	UPROPERTY(EditAnywhere, config, Category=Options)
	FLinearColor BackgroundColor;

	UPROPERTY(EditAnywhere, config, Category=Options)
	FLinearColor LabelColor;

	UPROPERTY(EditAnywhere, config, Category=Options)
	FLinearColor SelectedLabelColor;

	UPROPERTY(EditAnywhere, config, Category=Options)
	FLinearColor GridColor;

	UPROPERTY(EditAnywhere, config, Category=Options)
	FLinearColor GridTextColor;

	UPROPERTY(EditAnywhere, config, Category=Options)
	FLinearColor LabelBlockBkgColor;

	UPROPERTY(EditAnywhere, config, Category=Options)
	FLinearColor SelectedKeyColor;

};

