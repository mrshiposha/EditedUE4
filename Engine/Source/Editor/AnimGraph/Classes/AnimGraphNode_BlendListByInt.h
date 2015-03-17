// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "AnimGraphNode_BlendListBase.h"
#include "Animation/AnimNode_BlendListByInt.h"
#include "AnimGraphNode_BlendListByInt.generated.h"

UCLASS(MinimalAPI)
class UAnimGraphNode_BlendListByInt : public UAnimGraphNode_BlendListBase
{
	GENERATED_BODY()
public:
	ANIMGRAPH_API UAnimGraphNode_BlendListByInt(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, Category=Settings)
	FAnimNode_BlendListByInt Node;

	// Adds a new pose pin
	//@TODO: Generalize this behavior (returning a list of actions/delegates maybe?)
	ANIMGRAPH_API virtual void AddPinToBlendList();
	ANIMGRAPH_API virtual void RemovePinFromBlendList(UEdGraphPin* Pin);

	// UEdGraphNode interface
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PostPlacedNewNode() override;
	// End of UEdGraphNode interface

	// UK2Node interface
	virtual void GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const override;
	// End of UK2Node interface
};
