// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "GameplayCueInterface.generated.h"


/** Interface for actors that wish to handle GameplayCue events from GameplayEffects. Native only because blueprints can't implement interfaces with native functions */
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGameplayCueInterface: public UInterface
{
	GENERATED_BODY()
public:
	GAMEPLAYABILITIES_API UGameplayCueInterface(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

class GAMEPLAYABILITIES_API IGameplayCueInterface
{
	GENERATED_BODY()
public:

	virtual void HandleGameplayCue(AActor *Self, FGameplayTag GameplayCueTag, EGameplayCueEvent::Type EventType, FGameplayCueParameters Parameters);

	virtual void HandleGameplayCues(AActor *Self, const FGameplayTagContainer& GameplayCueTags, EGameplayCueEvent::Type EventType, FGameplayCueParameters Parameters);

	/** Default native handler, called if no tag matches found */
	virtual void GameplayCueDefaultHandler(EGameplayCueEvent::Type EventType, FGameplayCueParameters Parameters);

	/** Internal function to map ufunctions directly to gameplaycue tags */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = GameplayCue, meta = (BlueprintInternalUseOnly = "true"))
	void BlueprintCustomHandler(EGameplayCueEvent::Type EventType, FGameplayCueParameters Parameters);

	/** Call from a Cue handler event to continue checking for additional, more generic handlers. Called from the ability system blueprint library */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Ability|GameplayCue")
	virtual void ForwardGameplayCueToParent();

	static void DispatchBlueprintCustomHandler(AActor* Actor, UFunction* Func, EGameplayCueEvent::Type EventType, FGameplayCueParameters Parameters);	

	IGameplayCueInterface() : bForwardToParent(false) {}

private:
	/** If true, keep checking for additional handlers */
	bool bForwardToParent;

};


/**
 *	This is meant to provide another way of using GameplayCues without having to go through GameplayEffects.
 *	E.g., it is convenient if GameplayAbilities can issue replicated GameplayCues without having to create
 *	a GameplayEffect.
 *	
 *	Essentially provides bare necessities to replicate GameplayCue Tags.
 */


struct FActiveGameplayCueContainer;

USTRUCT(BlueprintType)
struct FActiveGameplayCue : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag GameplayCueTag;

	UPROPERTY()
	FPredictionKey PredictionKey;

	void PreReplicatedRemove(const struct FActiveGameplayCueContainer &InArray);
	void PostReplicatedAdd(const struct FActiveGameplayCueContainer &InArray);
	void PostReplicatedChange(const struct FActiveGameplayCueContainer &InArray) { }
};

USTRUCT(BlueprintType)
struct FActiveGameplayCueContainer : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray< FActiveGameplayCue >	GameplayCues;

	UPROPERTY()
	class UAbilitySystemComponent*	Owner;

	void AddCue(const FGameplayTag& Tag, const FPredictionKey& PredictionKey);
	void RemoveCue(const FGameplayTag& Tag);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FastArrayDeltaSerialize<FActiveGameplayCue>(GameplayCues, DeltaParms, *this);
	}

private:

	int32 GetGameStateTime(const UWorld* World) const;
};

template<>
struct TStructOpsTypeTraits< FActiveGameplayCueContainer > : public TStructOpsTypeTraitsBase
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
