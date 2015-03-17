// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ScriptBlueprint.h"
#include "ScriptFactory.generated.h"

/**
* Script Blueprint factory
*/
UCLASS(collapsecategories, hidecategories = Object, EarlyAccessPreview)
class SCRIPTEDITORPLUGIN_API UScriptFactory : public UFactory
{
	GENERATED_BODY()
public:
	UScriptFactory(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:

	UPROPERTY(EditAnywhere, Category = ScriptFactory, meta = (AllowAbstract = "", BlueprintBaseOnly = ""))
	TSubclassOf<class UObject> ParentClass;

	// Begin UFactory Interface
	virtual bool ConfigureProperties() override;
	virtual bool DoesSupportClass(UClass* Class) override;
	virtual UObject* FactoryCreateText(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const TCHAR*& Buffer, const TCHAR* BufferEnd, FFeedbackContext* Warn);
	// End UFactory Interface
};
