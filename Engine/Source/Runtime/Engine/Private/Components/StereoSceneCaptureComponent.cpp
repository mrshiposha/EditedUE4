#include "Components/StereoSceneCaptureComponent.h"
#include "SceneManagement.h"

UStereoSceneCaptureComponent::UStereoSceneCaptureComponent(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer), ShowFlags(FEngineShowFlags(ESFIM_Game))
{
	InitPostProcessSettings();

	LODDistanceFactor = 1.f;
	MaxViewDistance = -1;
	IPD = 6.2f;

	ShowFlags.TemporalAA = false;
	ShowFlags.MotionBlur = false;

	FrameIndex = 0;
}

void UStereoSceneCaptureComponent::UpdateSceneCaptureContents(class FSceneInterface *Scene)
{
	Scene->UpdateSceneCaptureContents(this);
}

void UStereoSceneCaptureComponent::StereoCapture()
{
	UWorld *World = GetWorld();
	if (World && World->Scene && IsVisible())
	{
		World->SendAllEndOfFrameUpdates();
		UpdateSceneCaptureContents(World->Scene);
		++FrameIndex;
	}
}

void UStereoSceneCaptureComponent::InitPostProcessSettings()
{
	PostProcessSettings.bOverride_SceneColorTint = true;
	PostProcessSettings.SceneColorTint = FLinearColor::White;

	PostProcessSettings.bOverride_VignetteIntensity = true;
	PostProcessSettings.VignetteIntensity = 0;

	PostProcessSettings.bOverride_GrainIntensity = true;
	PostProcessSettings.GrainIntensity = 0;

	PostProcessSettings.bOverride_BloomDirtMask = true;
	PostProcessSettings.BloomDirtMask = nullptr;
	PostProcessSettings.bOverride_BloomDirtMaskIntensity = true;
	PostProcessSettings.BloomDirtMaskIntensity = 0;

	PostProcessSettings.bOverride_SceneFringeIntensity = true;
	PostProcessSettings.SceneFringeIntensity = 0.0;
}