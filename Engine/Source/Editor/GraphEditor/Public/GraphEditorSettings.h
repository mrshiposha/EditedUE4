// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Runtime/SlateCore/Public/Layout/Margin.h"
#include "GraphEditorSettings.generated.h"


/**
 * Implements settings for the graph editor.
 */
UCLASS(config=EditorUserSettings)
class GRAPHEDITOR_API UGraphEditorSettings
	:	public UObject
{
	GENERATED_BODY()
public:
	UGraphEditorSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:

#if WITH_EDITOR
	// UObject interface

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:

	/** The visual styling to use for graph editor pins (in Blueprints, materials, etc...) */
	UPROPERTY(config, EditAnywhere, Category=GeneralStyle)
	TEnumAsByte<EBlueprintPinStyleType> DataPinStyle;

	/** The amount of padding above a pin (defaults to 4) */
	UPROPERTY(/*config, */ EditAnywhere, Category=GeneralStyle, AdvancedDisplay)
	float PaddingAbovePin;

	/** The amount of padding below a pin (defaults to 4) */
	UPROPERTY(/*config, */ EditAnywhere, Category=GeneralStyle, AdvancedDisplay)
	float PaddingBelowPin;

	/** The amount of horizontal padding towards the center of a node on an input pin (defaults to 10) */
	UPROPERTY(/*config, */ EditAnywhere, Category=GeneralStyle, AdvancedDisplay)
	float PaddingRightOfInput;

	/** The amount of horizontal padding towards the center of a node on an output pin (defaults to 10) */
	UPROPERTY(/*config, */ EditAnywhere, Category=GeneralStyle, AdvancedDisplay)
	float PaddingLeftOfOutput;

	/** The amount of padding towards the node edge (defaults to 10, can be negative to make pins overlap or stick out of a node */
	UPROPERTY(/*config, */ EditAnywhere, Category=GeneralStyle, AdvancedDisplay)
	float PaddingTowardsNodeEdge;

	/** Returns the padding to use for an input pin on a graph node */
	FMargin GetInputPinPadding() const;

	/** Returns the padding to use for an output pin on a graph node */
	FMargin GetOutputPinPadding() const;

	/** Returns the padding to use for a node region that won't contain a pin */
	FMargin GetNonPinNodeBodyPadding() const;

	/** Returns the size adjustment from node render geometry to shadow render geometry */
	FVector2D GetShadowDeltaSize() const;

public:

	/** The maximum value to clamp the absolute value of the horizontal distance between endpoints when calculating tangents (when the wire is moving forward) */
	UPROPERTY(config, EditAnywhere, Category=Splines, AdvancedDisplay)
	float ForwardSplineHorizontalDeltaRange;

	/** The maximum value to clamp the absolute value of the vertical distance between endpoints when calculating tangents (when the wire is moving forward) */
	UPROPERTY(config, EditAnywhere, Category=Splines, AdvancedDisplay)
	float ForwardSplineVerticalDeltaRange;

	/** The amount that the horizontal delta affects the generated tangent handle of splines (when the wire is moving forward) */
	UPROPERTY(config, EditAnywhere, Category=Splines, AdvancedDisplay)
	FVector2D ForwardSplineTangentFromHorizontalDelta;

	/** The amount that the vertical delta affects the generated tangent handle of splines (when the wire is moving forward) */
	UPROPERTY(config, EditAnywhere, Category=Splines, AdvancedDisplay)
	FVector2D ForwardSplineTangentFromVerticalDelta;

	/** The maximum value to clamp the absolute value of the horizontal distance between endpoints when calculating tangents (when the wire is moving backwards) */
	UPROPERTY(config, EditAnywhere, Category=Splines, AdvancedDisplay)
	float BackwardSplineHorizontalDeltaRange;

	/** The maximum value to clamp the absolute value of the vertical distance between endpoints when calculating tangents (when the wire is moving backwards) */
	UPROPERTY(config, EditAnywhere, Category=Splines, AdvancedDisplay)
	float BackwardSplineVerticalDeltaRange;

	/** The amount that the horizontal delta affects the generated tangent handle of splines (when the wire is moving backwards) */
	UPROPERTY(config, EditAnywhere, Category=Splines, AdvancedDisplay)
	FVector2D BackwardSplineTangentFromHorizontalDelta;

	/** The amount that the vertical delta affects the generated tangent handle of splines (when the wire is moving backwards) */
	UPROPERTY(config, EditAnywhere, Category=Splines, AdvancedDisplay)
	FVector2D BackwardSplineTangentFromVerticalDelta;

	/** Compute a tangent given the start and end points of the spline */
	FVector2D ComputeSplineTangent(const FVector2D& Start, const FVector2D& End) const;

public:

	/** The default color is used only for types not specifically defined below.  Generally if it's seen, it means another type needs to be defined so that the wire in question can have an appropriate color. */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor DefaultPinTypeColor;

	/** Execution pin type color */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor ExecutionPinTypeColor;

	/** Boolean pin type color */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor BooleanPinTypeColor;

	/** Byte pin type color */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor BytePinTypeColor;

	/** Class pin type color */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor ClassPinTypeColor;

	/** Integer pin type color */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor IntPinTypeColor;

	/** Floating-point pin type color */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor FloatPinTypeColor;

	/** Name pin type color */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor NamePinTypeColor;

	/** Delegate pin type color */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor DelegatePinTypeColor;

	/** Object pin type color */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor ObjectPinTypeColor;

	/** Interface pin type color */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor InterfacePinTypeColor;

	/** String pin type color */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor StringPinTypeColor;

	/** Text pin type color */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor TextPinTypeColor;

	/** Struct pin type color */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor StructPinTypeColor;

	/** Wildcard pin type color */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor WildcardPinTypeColor;

	/** Vector pin type color */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor VectorPinTypeColor;

	/** Rotator pin type color */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor RotatorPinTypeColor;

	/** Transform pin type color */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor TransformPinTypeColor;

	/** Index pin type color */
	UPROPERTY(EditAnywhere,  config, Category=PinColors)
	FLinearColor IndexPinTypeColor;

public:

	/** Event node title color */
	UPROPERTY(EditAnywhere,  config, Category=NodeTitleColors)
	FLinearColor EventNodeTitleColor;

	/** CallFunction node title color */
	UPROPERTY(EditAnywhere,  config, Category=NodeTitleColors)
	FLinearColor FunctionCallNodeTitleColor;

	/** Pure function call node title color */
	UPROPERTY(EditAnywhere,  config, Category=NodeTitleColors)
	FLinearColor PureFunctionCallNodeTitleColor;

	/** Parent class function call node title color */
	UPROPERTY(EditAnywhere,  config, Category=NodeTitleColors)
	FLinearColor ParentFunctionCallNodeTitleColor;

	/** Function Terminator node title color */
	UPROPERTY(EditAnywhere,  config, Category=NodeTitleColors)
	FLinearColor FunctionTerminatorNodeTitleColor;

	/** Exec Branch node title color */
	UPROPERTY(EditAnywhere,  config, Category=NodeTitleColors)
	FLinearColor ExecBranchNodeTitleColor;

	/** Exec Sequence node title color */
	UPROPERTY(EditAnywhere,  config, Category=NodeTitleColors)
	FLinearColor ExecSequenceNodeTitleColor;

	/** Result node title color */
	UPROPERTY(EditAnywhere,  config, Category=NodeTitleColors)
	FLinearColor ResultNodeTitleColor;

	/** Default Comment node title color */
	UPROPERTY(EditAnywhere, config, Category=NodeTitleColors)
	FLinearColor DefaultCommentNodeTitleColor;

public:

	UPROPERTY(EditAnywhere,  config, Category=Tracing)
	FLinearColor TraceAttackColor;

	UPROPERTY(EditAnywhere,  config, Category=Tracing)
	float TraceAttackWireThickness;

	/** How long is the attack color fully visible */
	UPROPERTY()
	float TraceAttackHoldPeriod;

	/** How long does it take to fade from the attack to the sustain color */
	UPROPERTY()
	float TraceDecayPeriod;

	UPROPERTY()
	float TraceDecayExponent;

	UPROPERTY(EditAnywhere,  config, Category=Tracing)
	FLinearColor TraceSustainColor;

	UPROPERTY(EditAnywhere,  config, Category=Tracing)
	float TraceSustainWireThickness;

	/** How long is the sustain color fully visible */
	UPROPERTY()
	float TraceSustainHoldPeriod;

	UPROPERTY(EditAnywhere,  config, Category=Tracing)
	FLinearColor TraceReleaseColor;

	UPROPERTY(EditAnywhere,  config, Category=Tracing)
	float TraceReleaseWireThickness;

	/** How long does it take to fade from the sustain to the release color */
	UPROPERTY()
	float TraceReleasePeriod;

	UPROPERTY()
	float TraceReleaseExponent;

	/** How much of a bonus does an exec get for being near the top of the trace stack, and how does that fall off with position? */
	UPROPERTY()
	float TracePositionBonusPeriod;

	UPROPERTY()
	float TracePositionExponent;
};
