// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#pragma once
#include "Materials/MaterialExpression.h"
#include "MaterialExpressionFmod.generated.h"

UCLASS(collapsecategories, hidecategories=Object)
class UMaterialExpressionFmod : public UMaterialExpression
{
	GENERATED_BODY()
public:
	UMaterialExpressionFmod(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY()
	FExpressionInput A;

	UPROPERTY()
	FExpressionInput B;


	// Begin UMaterialExpression Interface
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex, int32 MultiplexIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
#if WITH_EDITOR
	virtual FString GetKeywords() const {return TEXT("%");}
#endif // WITH_EDITOR
	// End UMaterialExpression Interface
};



