// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#pragma once
#include "InterpTrackInstBoolProp.generated.h"

UCLASS()
class UInterpTrackInstBoolProp : public UInterpTrackInstProperty
{
	GENERATED_BODY()
public:
	UInterpTrackInstBoolProp(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Pointer to boolean property in TrackObject. */
	void* BoolPropertyAddress;

	/** Mask that indicates which bit the boolean property actually uses of the value pointed to by BoolProp. */
	UPROPERTY(transient)
	UBoolProperty* BoolProperty;

	/** Saved value for restoring state when exiting Matinee. */
	UPROPERTY()
	bool ResetBool;


	// Begin UInterpTrackInst Instance
	virtual void InitTrackInst(UInterpTrack* Track) override;
	virtual void SaveActorState(UInterpTrack* Track) override;
	virtual void RestoreActorState(UInterpTrack* Track) override;
	// End UInterpTrackInst Instance
};

