// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

/**
 * SpectatorPawns are simple pawns that can fly around the world, used by
 * PlayerControllers when in the spectator state.
 */

#pragma once
#include "GameFramework/DefaultPawn.h"
#include "SpectatorPawn.generated.h"


UCLASS(config=Game, Blueprintable, BlueprintType, notplaceable)
class ENGINE_API ASpectatorPawn : public ADefaultPawn
{
	GENERATED_BODY()
public:
	ASpectatorPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Begin Pawn overrides
	/** Overridden to avoid changing network role. If subclasses want networked behavior, call the Pawn::PossessedBy() instead. */
	virtual void PossessedBy(class AController* NewController) override;
	// End Pawn overrides
};
