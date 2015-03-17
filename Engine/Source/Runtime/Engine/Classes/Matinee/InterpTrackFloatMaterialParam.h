// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "InterpTrackFloatMaterialParam.generated.h"

UCLASS(meta=( DisplayName = "Float Material Parameter Track" ) )
class UInterpTrackFloatMaterialParam : public UInterpTrackFloatBase
{
	GENERATED_BODY()
public:
	UInterpTrackFloatMaterialParam(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/** Materials whose parameters we want to change and the references to those materials. */
	UPROPERTY(EditAnywhere, Category=InterpTrackFloatMaterialParam)
	TArray<class UMaterialInterface*> TargetMaterials;

	/** Name of parameter in the MaterialInstance which this track will modify over time. */
	UPROPERTY(EditAnywhere, Category=InterpTrackFloatMaterialParam)
	FName ParamName;


	// Begin UObject Interface
#if WITH_EDITOR
	virtual void PreEditChange(UProperty* PropertyThatWillChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
	// End UObject Interface

	// Begin UInterpTrack interface.
	virtual int32 AddKeyframe(float Time, UInterpTrackInst* TrInst, EInterpCurveMode InitInterpMode) override;
	virtual void PreviewUpdateTrack(float NewPosition, UInterpTrackInst* TrInst) override;
	virtual void UpdateTrack(float NewPosition, UInterpTrackInst* TrInst, bool bJump) override;
	// End UInterpTrack interface.
};



