// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Engine/InputDelegateBinding.h"
#include "InputAxisDelegateBinding.generated.h"

USTRUCT()
struct ENGINE_API FBlueprintInputAxisDelegateBinding : public FBlueprintInputDelegateBinding
{
	GENERATED_BODY()

	UPROPERTY()
	FName InputAxisName;

	UPROPERTY()
	FName FunctionNameToBind;

	FBlueprintInputAxisDelegateBinding()
		: FBlueprintInputDelegateBinding()
		, InputAxisName(NAME_None)
		, FunctionNameToBind(NAME_None)
	{
	}
};

UCLASS()
class ENGINE_API UInputAxisDelegateBinding : public UInputDelegateBinding
{
	GENERATED_BODY()
public:
	UInputAxisDelegateBinding(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY()
	TArray<FBlueprintInputAxisDelegateBinding> InputAxisDelegateBindings;

	// Begin UInputDelegateBinding interface
	virtual void BindToInputComponent(UInputComponent* InputComponent) const override;
	// End UInputDelegateBinding interface
};
