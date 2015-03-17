// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Components/PanelSlot.h"
#include "SlateWrapperTypes.h"

#include "VerticalBoxSlot.generated.h"

/** The Slot for the UVerticalBox, contains the widget that is flowed vertically */
UCLASS()
class UMG_API UVerticalBoxSlot : public UPanelSlot
{
	GENERATED_BODY()
public:
	UVerticalBoxSlot(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	
	/** The padding area between the slot and the content it contains. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Layout (Vertical Box Slot)")
	FMargin Padding;

	/** How much space this slot should occupy in the direction of the panel. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Layout (Vertical Box Slot)")
	FSlateChildSize Size;

	/** The alignment of the object horizontally. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Layout (Vertical Box Slot)")
	TEnumAsByte<EHorizontalAlignment> HorizontalAlignment;

	/** The alignment of the object vertically. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Layout (Vertical Box Slot)")
	TEnumAsByte<EVerticalAlignment> VerticalAlignment;

public:

	UFUNCTION(BlueprintCallable, Category="Layout (Vertical Box Slot)")
	void SetPadding(FMargin InPadding);

	UFUNCTION(BlueprintCallable, Category="Layout (Vertical Box Slot)")
	void SetSize(FSlateChildSize InSize);

	UFUNCTION(BlueprintCallable, Category="Layout (Vertical Box Slot)")
	void SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment);

	UFUNCTION(BlueprintCallable, Category="Layout (Vertical Box Slot)")
	void SetVerticalAlignment(EVerticalAlignment InVerticalAlignment);

public:

	// UPanelSlot interface
	virtual void SynchronizeProperties() override;
	// End of UPanelSlot interface

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	/** Builds the underlying FSlot for the Slate layout panel. */
	void BuildSlot(TSharedRef<SVerticalBox> InVerticalBox);

private:
	//TODO UMG Slots should hold weak or shared refs to slots.

	/** A raw pointer to the slot to allow us to adjust the size, padding...etc at runtime. */
	SVerticalBox::FSlot* Slot;
};
