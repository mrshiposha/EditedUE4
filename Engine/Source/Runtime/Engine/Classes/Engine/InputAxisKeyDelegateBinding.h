// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Engine/InputDelegateBinding.h"
#include "InputAxisKeyDelegateBinding.generated.h"

USTRUCT()
struct ENGINE_API FBlueprintInputAxisKeyDelegateBinding : public FBlueprintInputDelegateBinding
{
	GENERATED_BODY()

	UPROPERTY()
	FKey AxisKey;

	UPROPERTY()
	FName FunctionNameToBind;

	FBlueprintInputAxisKeyDelegateBinding()
		: FBlueprintInputDelegateBinding()
		, FunctionNameToBind(NAME_None)
	{
	}
};

UCLASS()
class ENGINE_API UInputAxisKeyDelegateBinding : public UInputDelegateBinding
{
	GENERATED_BODY()
public:
	UInputAxisKeyDelegateBinding(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY()
	TArray<FBlueprintInputAxisKeyDelegateBinding> InputAxisKeyDelegateBindings;

	// Begin UInputDelegateBinding interface
	virtual void BindToInputComponent(UInputComponent* InputComponent) const override;
	// End UInputDelegateBinding interface
};
