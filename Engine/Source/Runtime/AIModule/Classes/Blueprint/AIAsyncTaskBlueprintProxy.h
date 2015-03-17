// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "AITypes.h"
#include "AI/Navigation/NavLinkDefinition.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIAsyncTaskBlueprintProxy.generated.h"

class UWorld;
class AAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOAISimpleDelegate, EPathFollowingResult::Type, MovementResult);

UCLASS(MinimalAPI)
class UAIAsyncTaskBlueprintProxy : public UObject
{
	GENERATED_BODY()
public:
	AIMODULE_API UAIAsyncTaskBlueprintProxy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(BlueprintAssignable)
	FOAISimpleDelegate	OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FOAISimpleDelegate	OnFail;

public:
	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MovementResult);

	void OnNoPath();

	// Begin UObject interface
	virtual void BeginDestroy() override;
	// End UObject interface

	TWeakObjectPtr<AAIController> AIController;
	FAIRequestID MoveRequestId;
	TWeakObjectPtr<UWorld> MyWorld;

	FTimerHandle TimerHandle_OnNoPath;
};
