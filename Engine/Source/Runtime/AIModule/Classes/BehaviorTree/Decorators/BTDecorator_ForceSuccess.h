// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once 
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_ForceSuccess.generated.h"

/** 
 * Change node result to Success
 * useful for creating optional branches in sequence
 *
 * Forcing failed result was not implemented, because it doesn't make sense in both basic composites:
 * - sequence = child nodes behind it will be never run
 * - selector = would allow executing multiple nodes, turning it into a sequence...
 */

UCLASS(HideCategories=(Condition))
class AIMODULE_API UBTDecorator_ForceSuccess : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTDecorator_ForceSuccess(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void OnNodeProcessed(struct FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type& NodeResult);

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif // WITH_EDITOR
};
