// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MatineeTrackSoundHelper.generated.h"


UCLASS()
class UMatineeTrackSoundHelper : public UInterpTrackHelper
{
	GENERATED_BODY()
public:
	UMatineeTrackSoundHelper(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:

	// UInterpTrackHelper interface

	virtual	bool PreCreateKeyframe( UInterpTrack *Track, float KeyTime ) const override;
	virtual void  PostCreateKeyframe( UInterpTrack *Track, int32 KeyIndex ) const override;
};

