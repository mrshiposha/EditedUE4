// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SourceCodeAccessSettings.generated.h"

UCLASS(config=EditorUserSettings)
class USourceCodeAccessSettings : public UObject
{
	GENERATED_BODY()
public:
	USourceCodeAccessSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** The source code editor we prefer to use. */
	UPROPERTY(Config, EditAnywhere, Category="Source Code Editor", meta=(DisplayName="Source Code Editor"))
	FString PreferredAccessor;
};
