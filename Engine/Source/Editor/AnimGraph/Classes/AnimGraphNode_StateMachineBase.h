// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "AnimGraphNode_Base.h"
#include "Animation/AnimNode_StateMachine.h"
#include "EdGraph/EdGraphNodeUtils.h" // for FNodeTextCache
#include "AnimGraphNode_StateMachineBase.generated.h"

UCLASS(Abstract)
class ANIMGRAPH_API UAnimGraphNode_StateMachineBase : public UAnimGraphNode_Base
{
	GENERATED_BODY()
public:
	UAnimGraphNode_StateMachineBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Editor state machine representation
	UPROPERTY()
	class UAnimationStateMachineGraph* EditorStateMachineGraph;

	// UEdGraphNode interface
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PostPlacedNewNode() override;
	virtual UObject* GetJumpTargetForDoubleClick() const override;
	virtual void DestroyNode() override;
	virtual void PostPasteNode() override;
	virtual TSharedPtr<class INameValidatorInterface> MakeNameValidator() const override;
	virtual FString GetDocumentationLink() const override;
	virtual void OnRenameNode(const FString& NewName) override;
	// End of UEdGraphNode interface

	// UAnimGraphNode_Base interface
	virtual FString GetNodeCategory() const override;
	virtual void GetMenuEntries(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	// End of UAnimGraphNode_Base interface

	//  @return the name of this state machine
	FString GetStateMachineName();

	// Interface for derived classes to implement
	virtual FAnimNode_StateMachine& GetNode() PURE_VIRTUAL(UAnimGraphNode_StateMachineBase::GetNode, static FAnimNode_StateMachine Dummy; return Dummy;);
	// End of my interface

private:
	/** Constructing FText strings can be costly, so we cache the node's title */
	FNodeTextCache CachedFullTitle;
};
