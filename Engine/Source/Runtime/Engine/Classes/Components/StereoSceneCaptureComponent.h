#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Components/SceneCaptureComponent.h"
#include "Engine/Scene.h"
#include "StereoSceneCaptureComponent.generated.h"

UCLASS()
class ENGINE_API UStereoSceneCaptureComponent : public USceneComponent
{
	GENERATED_UCLASS_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StereoSceneCapture")
	class UTextureRenderTargetCube* TextureTargetLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StereoSceneCapture")
	class UTextureRenderTargetCube* TextureTargetRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StereoSceneCapture")
	class UTextureRenderTarget2D* TextureTargetODS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StereoSceneCapture", meta = (DisplayName = "Interpupillary Distance (cm)", UIMin = "0.0", UIMax = "16.0", ClampMin = "0.0", ClampMax = "16.0"))
	float IPD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StereoSceneCapture, meta = (UIMin = "100", UIMax = "10000"))
	float MaxViewDistance;

	UPROPERTY(interp, Category = PostProcessVolume, BlueprintReadWrite, meta = (UIMin = "0.0", UIMax = "1.0"))
	float PostProcessBlendWeight;

	UPROPERTY(interp, Category = PostProcessVolume, meta = (ShowOnlyInnerProperties))
	struct FPostProcessSettings PostProcessSettings;

	UPROPERTY(EditAnywhere, Category = PlanarReflection, meta = (UIMin = ".1", UIMax = "10"), AdvancedDisplay)
	float LODDistanceFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StereoSceneCapture", meta = (RelativePath))
	FDirectoryPath CaptureDirectory;

	size_t FrameIndex;

	FEngineShowFlags ShowFlags;


	void UpdateSceneCaptureContents(class FSceneInterface *);

	UFUNCTION(BlueprintCallable, Category = "Rendering|Stereo Capture")
	void StereoCapture();

	void InitPostProcessSettings();


private:
};