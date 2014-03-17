// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AnimGraphPrivatePCH.h"

/////////////////////////////////////////////////////
// FPoseLinkMappingRecord

void FPoseLinkMappingRecord::PatchLinkIndex(uint8* DestinationPtr, int32 LinkID, int32 SourceLinkID) const
{
	checkSlow(IsValid());

	DestinationPtr = ChildProperty->ContainerPtrToValuePtr<uint8>(DestinationPtr);
	
	if (ChildPropertyIndex != INDEX_NONE)
	{
		UArrayProperty* ArrayProperty = CastChecked<UArrayProperty>(ChildProperty);

		FScriptArrayHelper ArrayHelper(ArrayProperty, DestinationPtr);
		check(ArrayHelper.IsValidIndex(ChildPropertyIndex));

		DestinationPtr = ArrayHelper.GetRawPtr(ChildPropertyIndex);
	}

	// Check to guard against accidental infinite loops
	check((LinkID == INDEX_NONE) || (LinkID != SourceLinkID));

	// Patch the pose link
	FPoseLinkBase& PoseLink = *((FPoseLinkBase*)DestinationPtr);
	PoseLink.LinkID = LinkID;
	PoseLink.SourceLinkID = SourceLinkID;
}

/////////////////////////////////////////////////////
// UAnimGraphNode_Root

UAnimGraphNode_Root::UAnimGraphNode_Root(const FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
}

FLinearColor UAnimGraphNode_Root::GetNodeTitleColor() const
{
	return FLinearColor(1.0f, 0.65f, 0.4f);
}

FString UAnimGraphNode_Root::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return TEXT("Final Animation Pose");
}

FString UAnimGraphNode_Root::GetTooltip() const
{
	return TEXT("Wire the final animation pose into this node");
}

bool UAnimGraphNode_Root::IsSinkNode() const
{
	return true;
}

void UAnimGraphNode_Root::GetMenuEntries(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	// Intentionally empty.  This node is autogenerated when a new graph is created.
}

FString UAnimGraphNode_Root::GetDocumentationLink() const
{
	return TEXT("Shared/GraphNodes/AnimationStateMachine");
}