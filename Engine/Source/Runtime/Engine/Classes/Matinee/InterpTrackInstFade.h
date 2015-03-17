// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#pragma once
#include "InterpTrackInstFade.generated.h"

UCLASS()
class UInterpTrackInstFade : public UInterpTrackInst
{
	GENERATED_BODY()
public:
	UInterpTrackInstFade(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	// Begin UInterpTrackInst Instance
	virtual void TermTrackInst(UInterpTrack* Track) override;
	// End UInterpTrackInst Instance
};

