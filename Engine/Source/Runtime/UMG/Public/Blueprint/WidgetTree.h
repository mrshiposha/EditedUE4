// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UserWidget.h"

#include "WidgetTree.generated.h"

/** The widget tree manages the collection of widgets in a blueprint widget. */
UCLASS()
class UMG_API UWidgetTree : public UObject
{
	GENERATED_BODY()
public:
	UWidgetTree(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:

	// Begin UObject
	virtual UWorld* GetWorld() const override;
	// End UObject

	/** Finds the widget in the tree by name. */
	UWidget* FindWidget(const FName& Name) const;

	/** Finds a widget in the tree using the native widget as the key. */
	UWidget* FindWidget(TSharedRef<SWidget> InWidget) const;

	/** Finds the widget in the tree by name and casts the return to the desired type. */
	template<class WidgetClass>
	FORCEINLINE WidgetClass* FindWidget(const FName& Name) const
	{
		return Cast<WidgetClass>(FindWidget(Name));
	}

	/** Removes the widget from the hierarchy and all sub widgets. */
	bool RemoveWidget(UWidget* Widget);

	/** Gets the parent widget of a given widget, and potentially the child index. */
	class UPanelWidget* FindWidgetParent(UWidget* Widget, int32& OutChildIndex);

	/** Gathers all the widgets in the tree recursively */
	void GetAllWidgets(TArray<UWidget*>& Widgets) const;

	/** Gathers descendant child widgets of a parent widget. */
	void GetChildWidgets(UWidget* Parent, TArray<UWidget*>& Widgets) const;

	/**
	 * Iterates through all widgets including widgets contained in named slots, other than
	 * investigating named slots, this code does not dive into foreign WidgetTrees, as would exist
	 * inside another user widget.
	 */
	template <typename Predicate>
	FORCEINLINE void ForEachWidget(Predicate Pred) const
	{
		if ( RootWidget )
		{
			Pred(RootWidget);
			
			ForWidgetAndChildren(RootWidget, Pred);
		}
	}

	/**
	 * Iterates through all child widgets including widgets contained in named slots, other than
	 * investigating named slots, this code does not dive into foreign WidgetTrees, as would exist
	 * inside another user widget.
	 */
	template <typename Predicate>
	FORCEINLINE void ForWidgetAndChildren(UWidget* Widget, Predicate Pred) const
	{
		// Search for any named slot with content that we need to dive into.
		if ( INamedSlotInterface* NamedSlotHost = Cast<INamedSlotInterface>(Widget) )
		{
			TArray<FName> SlotNames;
			NamedSlotHost->GetSlotNames(SlotNames);

			for ( FName SlotName : SlotNames )
			{
				if ( UWidget* SlotContent = NamedSlotHost->GetContentForSlot(SlotName) )
				{
					Pred(SlotContent);

					ForWidgetAndChildren(SlotContent, Pred);
				}
			}
		}

		// Search standard children.
		if ( UPanelWidget* PanelParent = Cast<UPanelWidget>(Widget) )
		{
			for ( int32 ChildIndex = 0; ChildIndex < PanelParent->GetChildrenCount(); ChildIndex++ )
			{
				if ( UWidget* ChildWidget = PanelParent->GetChildAt(ChildIndex) )
				{
					Pred(ChildWidget);

					ForWidgetAndChildren(ChildWidget, Pred);
				}
			}
		}
	}

	/** Constructs the widget, and adds it to the tree. */
	template< class T >
	FORCEINLINE T* ConstructWidget(TSubclassOf<UWidget> WidgetType)
	{
		if ( WidgetType->IsChildOf(UUserWidget::StaticClass()) )
		{
			UUserWidget* Widget = NewObject<UUserWidget>(this, WidgetType);
			Widget->Initialize();
			Widget->SetFlags(RF_Transactional);
			return (T*)Widget;
		}
		else
		{
			UWidget* Widget = (UWidget*)NewObject<UWidget>(this, WidgetType);
			Widget->SetFlags(RF_Transactional);
			return (T*)Widget;
		}
	}

	// UObject interface
	virtual void PreSave() override
	{
		AllWidgets.Empty();

		GetAllWidgets(AllWidgets);

		Super::PreSave();
	}
	// End of UObject interface

public:
	/** The root widget of the tree */
	UPROPERTY()
	UWidget* RootWidget;

protected:

	UPROPERTY()
	TArray< UWidget* > AllWidgets;
};
