// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "HudSettings.generated.h"


UCLASS(config=Game)
class ENGINESETTINGS_API UHudSettings
	: public UObject
{
	GENERATED_BODY()
public:
	UHudSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Whether the HUD is visible at all.	 */
	UPROPERTY(config, EditAnywhere, Category=HudSettings)
	uint32 bShowHUD:1;

	/** Collection of names specifying what debug info to display for ViewTarget actor.	 */
	UPROPERTY(globalconfig, EditAnywhere, Category=HudSettings)
	TArray<FName> DebugDisplay;
};
