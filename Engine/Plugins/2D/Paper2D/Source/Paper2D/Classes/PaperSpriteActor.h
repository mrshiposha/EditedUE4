// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PaperSpriteComponent.h"
#include "PaperSpriteActor.generated.h"

/**
 * An instance of a UPaperSprite in a level.
 *
 * This actor is created when you drag a sprite asset from the content browser into the level, and
 * it is just a thin wrapper around a UPaperSpriteComponent that actually references the asset.
 */
UCLASS(ComponentWrapperClass, meta = (ChildCanTick))
class PAPER2D_API APaperSpriteActor : public AActor
{
	GENERATED_BODY()
public:
	APaperSpriteActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	UPROPERTY(Category=Sprite, VisibleAnywhere, BlueprintReadOnly, meta=(ExposeFunctionCategories="Sprite,Rendering,Physics,Components|Sprite", AllowPrivateAccess="true"))
	class UPaperSpriteComponent* RenderComponent;
public:

	// AActor interface
#if WITH_EDITOR
	virtual bool GetReferencedContentObjects(TArray<UObject*>& Objects) const override;
#endif
	// End of AActor interface

	/** Returns RenderComponent subobject **/
	FORCEINLINE class UPaperSpriteComponent* GetRenderComponent() const { return RenderComponent; }
};

// Allow the old name to continue to work for one release
DEPRECATED(4.3, "APaperRenderActor has been renamed to APaperSpriteActor")
typedef APaperSpriteActor APaperRenderActor;
