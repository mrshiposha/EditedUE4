#include "Engine/StereoSceneCapture.h"
#include "Engine/StaticMesh.h"
#include "Components/DrawFrustumComponent.h"
#include "Components/StereoSceneCaptureComponent.h"


AStereoSceneCapture::AStereoSceneCapture(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	DrawFrustum = CreateDefaultSubobject<UDrawFrustumComponent>(TEXT("DrawFrustum"));
	DrawFrustum->SetIsVisualizationComponent(true);
	DrawFrustum->SetupAttachment(GetMeshComp());
#endif // WITH_EDITOR

	StereoSceneCaptureComponent = CreateDefaultSubobject<UStereoSceneCaptureComponent>(TEXT("StereoSceneCapture"));
	StereoSceneCaptureComponent->SetupAttachment(GetMeshComp());
	StereoSceneCaptureComponent->SetVisibility(true);
}

#if WITH_EDITOR
void AStereoSceneCapture::PostActorCreated()
{
	Super::PostActorCreated();

	if (GetMeshComp())
	{
		if (!IsRunningCommandlet())
		{
			if (!GetMeshComp()->GetStaticMesh())
			{
				UStaticMesh *CameraMesh = LoadObject<UStaticMesh>
				(
					nullptr, 
					TEXT("/Engine/EditorMeshes/MatineeCam_SM.MatineeCam_SM"), 
					nullptr, 
					LOAD_None, 
					nullptr
				);
				GetMeshComp()->SetStaticMesh(CameraMesh);
			}
		}
	}
}
#endif // WITH_EDITOR
