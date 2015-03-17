// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#pragma once
#include "ActorFactoryCharacter.generated.h"

UCLASS(MinimalAPI,config=Editor)
class UActorFactoryCharacter : public UActorFactory
{
	GENERATED_BODY()
public:
	UNREALED_API UActorFactoryCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual bool CanCreateActorFrom( const FAssetData& AssetData, FText& OutErrorMsg ) override;

protected:
	virtual AActor* SpawnActor( UObject* Asset, ULevel* InLevel, const FVector& Location, const FRotator& Rotation, EObjectFlags ObjectFlags, const FName& Name ) override;
};
