// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AppleARKitTrackable.h"
#include "AppleARKitTextures.h"

UAppleARKitEnvironmentCaptureProbe::UAppleARKitEnvironmentCaptureProbe()
	: Super()
	, ARKitEnvironmentTexture(nullptr)
{
#if SUPPORTS_ARKIT_2_0
	ARKitEnvironmentTexture = NewObject<UAppleARKitEnvironmentCaptureProbeTexture>();
	// Set the base class member since that's what gets used by the non-ARKit specific code
	EnvironmentCaptureTexture = ARKitEnvironmentTexture;
#endif
}

#if SUPPORTS_ARKIT_2_0
void UAppleARKitEnvironmentCaptureProbe::UpdateEnvironmentCapture(const TSharedRef<FARSystemBase, ESPMode::ThreadSafe>& InTrackingSystem, uint32 FrameNumber, double InTimestamp, const FTransform& InLocalToTrackingTransform, const FTransform& InAlignmentTransform, FVector InExtent, id<MTLTexture> InMetalTexture)
{
	Super::UpdateEnvironmentCapture(InTrackingSystem, FrameNumber, InTimestamp, InLocalToTrackingTransform, InAlignmentTransform, InExtent);
	
	check(ARKitEnvironmentTexture != nullptr);
	ARKitEnvironmentTexture->Init(InTimestamp, InMetalTexture);
}
#endif

