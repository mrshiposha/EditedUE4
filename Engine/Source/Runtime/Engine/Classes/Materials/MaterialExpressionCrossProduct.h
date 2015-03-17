// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#pragma once
#include "Materials/MaterialExpression.h"
#include "MaterialExpressionCrossProduct.generated.h"

UCLASS()
class UMaterialExpressionCrossProduct : public UMaterialExpression
{
	GENERATED_BODY()
public:
	UMaterialExpressionCrossProduct(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY()
	FExpressionInput A;

	UPROPERTY()
	FExpressionInput B;


	// Begin UMaterialExpression Interface
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex, int32 MultiplexIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	// End UMaterialExpression Interface
};



