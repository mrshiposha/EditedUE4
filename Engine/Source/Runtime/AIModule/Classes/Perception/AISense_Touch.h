// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense.h"
#include "AISense_Touch.generated.h"

class UAISense_Touch;

USTRUCT()
struct AIMODULE_API FAITouchEvent
{	
	GENERATED_BODY()

	typedef UAISense_Touch FSenseClass;

	FVector Location;
	
	UPROPERTY()
	AActor* TouchReceiver;

	UPROPERTY()
	AActor* OtherActor;
		
	FAITouchEvent(){}
	
	FAITouchEvent(AActor* InTouchReceiver, AActor* InOtherActor, const FVector& EventLocation)
		: Location(EventLocation), TouchReceiver(InTouchReceiver), OtherActor(InOtherActor)
	{
	}
};

UCLASS(ClassGroup=AI)
class AIMODULE_API UAISense_Touch : public UAISense
{
	GENERATED_BODY()
public:
	UAISense_Touch(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY()
	TArray<FAITouchEvent> RegisteredEvents;

public:		
	void RegisterEvent(const FAITouchEvent& Event);	

protected:
	virtual float Update() override;
};
