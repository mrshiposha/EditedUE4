// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

/**
 *
 */

#pragma once
#include "AnimSequenceFactory.generated.h"

UCLASS(HideCategories=Object,MinimalAPI)
class UAnimSequenceFactory : public UFactory
{
	GENERATED_BODY()
public:
	UNREALED_API UAnimSequenceFactory(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY()
	class USkeleton* TargetSkeleton;

	// Begin UFactory Interface
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class,UObject* InParent,FName Name,EObjectFlags Flags,UObject* Context,FFeedbackContext* Warn) override;
	// Begin UFactory Interface	

private:
	void OnTargetSkeletonSelected(const FAssetData& SelectedAsset);

private:
	TSharedPtr<SWindow> PickerWindow;
};

