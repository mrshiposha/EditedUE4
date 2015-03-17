// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#pragma once
#include "Materials/MaterialExpression.h"
#include "MaterialExpressionMultiply.generated.h"

UCLASS(MinimalAPI)
class UMaterialExpressionMultiply : public UMaterialExpression
{
	GENERATED_BODY()
public:
	ENGINE_API UMaterialExpressionMultiply(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(meta = (RequiredInput = "false", ToolTip = "Defaults to 'ConstA' if not specified"))
	FExpressionInput A;

	UPROPERTY(meta = (RequiredInput = "false", ToolTip = "Defaults to 'ConstB' if not specified"))
	FExpressionInput B;

	/** only used if A is not hooked up */
	UPROPERTY(EditAnywhere, Category=MaterialExpressionMultiply, meta=(OverridingInputProperty = "A"))
	float ConstA;

	/** only used if B is not hooked up */
	UPROPERTY(EditAnywhere, Category=MaterialExpressionMultiply, meta=(OverridingInputProperty = "B"))
	float ConstB;


	// Begin UMaterialExpression Interface
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex, int32 MultiplexIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
#if WITH_EDITOR
	virtual FString GetKeywords() const {return TEXT("*");}
#endif // WITH_EDITOR
	// End UMaterialExpression Interface
};



