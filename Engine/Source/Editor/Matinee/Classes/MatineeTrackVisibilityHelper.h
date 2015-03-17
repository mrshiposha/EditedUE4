// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#pragma once
#include "MatineeTrackVisibilityHelper.generated.h"

UCLASS()
class UMatineeTrackVisibilityHelper : public UInterpTrackHelper
{
	GENERATED_BODY()
public:
	UMatineeTrackVisibilityHelper(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void OnAddKeyTextEntry(const FString& ChosenText, IMatineeBase* Matinee, UInterpTrack* Track);

public:

	// UInterpTrackHelper Interface

	virtual	bool PreCreateKeyframe( UInterpTrack *Track, float KeyTime ) const override;
	virtual void  PostCreateKeyframe( UInterpTrack *Track, int32 KeyIndex ) const override;
};

