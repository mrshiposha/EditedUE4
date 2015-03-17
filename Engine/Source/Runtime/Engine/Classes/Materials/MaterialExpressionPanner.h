// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#pragma once
#include "Materials/MaterialExpression.h"
#include "MaterialExpressionPanner.generated.h"

UCLASS(collapsecategories, hidecategories=Object, MinimalAPI)
class UMaterialExpressionPanner : public UMaterialExpression
{
	GENERATED_BODY()
public:
	ENGINE_API UMaterialExpressionPanner(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(meta = (RequiredInput = "false", ToolTip = "Defaults to 'ConstCoordinate' if not specified"))
	FExpressionInput Coordinate;

	UPROPERTY(meta = (RequiredInput = "false", ToolTip = "Defaults to Game Time if not specified"))
	FExpressionInput Time;

	UPROPERTY(EditAnywhere, Category=MaterialExpressionPanner)
	float SpeedX;

	UPROPERTY(EditAnywhere, Category=MaterialExpressionPanner)
	float SpeedY;

	/** only used if Coordinate is not hooked up */
	UPROPERTY(EditAnywhere, Category = MaterialExpressionPanner)
	uint32 ConstCoordinate;

	// Output only the fractional part of the pan calculation for greater precision.
	// Output is greater than or equal to 0 and less than 1.
	UPROPERTY(EditAnywhere, Category=MaterialExpressionPanner)
	bool bFractionalPart;

	// Begin UMaterialExpression Interface
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex, int32 MultiplexIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	virtual bool NeedsRealtimePreview() override { return Time.Expression==NULL && (SpeedX != 0.f || SpeedY != 0.f); }
	// End UMaterialExpression Interface

};



