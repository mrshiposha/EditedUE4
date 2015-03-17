// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "K2Node_TemporaryVariable.h"
#include "K2Node_LocalVariable.generated.h"

UCLASS(MinimalAPI, deprecated)
class UDEPRECATED_K2Node_LocalVariable : public UK2Node_TemporaryVariable
{
	GENERATED_BODY()
public:
	BLUEPRINTGRAPH_API UDEPRECATED_K2Node_LocalVariable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** If this is not an override, allow user to specify a name for the function created by this entry point */
	UPROPERTY()
	FName CustomVariableName;

	/** The local variable's assigned tooltip */
	UPROPERTY()
	FText VariableTooltip;

	// Begin UEdGraphNode interface.
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void OnRenameNode(const FString& NewName) override;
	virtual TSharedPtr<class INameValidatorInterface> MakeNameValidator() const override;
	virtual void PostPlacedNewNode() override;
	virtual void PostPasteNode() override;
	virtual bool IsCompatibleWithGraph(const UEdGraph* TargetGraph) const override;
	virtual void ReconstructNode() override;
	// End UEdGraphNode interface.

	// Begin UK2Node interface.
	virtual bool NodeCausesStructuralBlueprintChange() const override { return true; }
	virtual bool ShouldShowNodeProperties() const override { return true; }
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override {}
	// End UK2Node interface.

	/**
	 * Assigns the local variable a type
	 *
	 * @param InVariableType		The type to assign this local variable to
	 */
	BLUEPRINTGRAPH_API void ChangeVariableType(const FEdGraphPinType& InVariableType);
};
