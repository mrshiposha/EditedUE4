// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "AssetData.h"
#include "EdGraphNode_Reference.generated.h"

UCLASS()
class UEdGraphNode_Reference : public UEdGraphNode
{
	GENERATED_BODY()
public:
	UEdGraphNode_Reference(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void SetupReferenceNode(const FIntPoint& NodeLoc, const TArray<FName>& NewPackageNames, const FAssetData& InAssetData);
	virtual void SetReferenceNodeCollapsed(const FIntPoint& NodeLoc, int32 InNumReferencesExceedingMax);
	virtual void AddReferencer(class UEdGraphNode_Reference* ReferencerNode);
	virtual FName GetPackageName() const;
	class UEdGraph_ReferenceViewer* GetReferenceViewerGraph() const;

	// UEdGraphNode implementation
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
	virtual UObject* GetJumpTargetForDoubleClick() const override;
	// End UEdGraphNode implementation

	void CacheAssetData(const FAssetData& AssetData);

	bool UsesThumbnail() const;
	FAssetData GetAssetData() const;
protected:
	virtual UEdGraphPin* GetDependencyPin();
	virtual UEdGraphPin* GetReferencerPin();

	class UEdGraph_ReferenceViewer* GetReferenceGraph() const
	{
		return CastChecked<UEdGraph_ReferenceViewer>(GetOuter());
	}

private:
	TArray<FName> PackageNames;
	FText NodeTitle;

	bool bUsesThumbnail;
	FAssetData CachedAssetData;

	UEdGraphPin* DependencyPin;
	UEdGraphPin* ReferencerPin;
};


