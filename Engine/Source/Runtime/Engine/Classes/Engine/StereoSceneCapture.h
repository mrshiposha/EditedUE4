// <CGF> Stereo Capturer Actor

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/SceneCapture.h"
#include "StereoSceneCapture.generated.h"

UCLASS()
class AStereoSceneCapture : public ASceneCapture
{
	GENERATED_UCLASS_BODY()

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	class UDrawFrustumComponent *DrawFrustum;
#endif // WITH_EDITORONLY_DATA

	UPROPERTY(Category = DecalActor, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStereoSceneCaptureComponent *StereoSceneCaptureComponent;

public:

#if WITH_EDITOR
	ENGINE_API virtual void PostActorCreated() override;

	ENGINE_API class UDrawFrustumComponent* GetDrawFrustum() const { return DrawFrustum; }
#endif // WITH_EDITOR

private:
};