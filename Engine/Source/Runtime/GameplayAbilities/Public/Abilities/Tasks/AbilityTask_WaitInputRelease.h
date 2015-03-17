// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "AbilityTask.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemComponent.h"
#include "AbilityTask_WaitInputRelease.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputReleaseDelegate, float, TimeHeld);

/**
 *	Waits until the input is released from activating an ability. Clients will replicate a 'release input' event to the server, but not the exact time it was held locally.
 *	We expect server to execute this task in parallel and keep its own time.
 */
UCLASS(MinimalAPI)
class UAbilityTask_WaitInputRelease : public UAbilityTask
{
	GENERATED_BODY()
public:
	GAMEPLAYABILITIES_API UAbilityTask_WaitInputRelease(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(BlueprintAssignable)
	FInputReleaseDelegate	OnRelease;

	UFUNCTION()
	void OnReleaseCallback();

	virtual void Activate() override;

	/** Wait until the user releases the input button for this ability's activation. Returns time from hitting this node, till release. Will return 0 if input was already released. */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_WaitInputRelease* WaitInputRelease(UObject* WorldContextObject, bool bTestAlreadyReleased=false);

protected:

	virtual void OnDestroy(bool AbilityEnded) override;

	float StartTime;
	bool bTestInitialState;
	FDelegateHandle DelegateHandle;
};
