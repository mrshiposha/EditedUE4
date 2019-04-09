// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	
=============================================================================*/

#include "CoreMinimal.h"
#include "Containers/ArrayView.h"
#include "Misc/MemStack.h"
#include "EngineDefines.h"
#include "RHIDefinitions.h"
#include "RHI.h"
#include "RenderingThread.h"
#include "Engine/Scene.h"
#include "SceneInterface.h"
#include "LegacyScreenPercentageDriver.h"
#include "GameFramework/Actor.h"
#include "GameFramework/WorldSettings.h"
#include "RHIStaticStates.h"
#include "SceneView.h"
#include "Shader.h"
#include "TextureResource.h"
#include "StaticBoundShaderState.h"
#include "SceneUtils.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneCaptureComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SceneCaptureComponentCube.h"
#include "Components/StereoSceneCaptureComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/TextureRenderTargetCube.h"
#include "Engine/Public/HighResScreenshot.h"
#include "ImageWriteQueue/Public/ImageWriteTask.h"
#include "ImageWriteQueue/Public/ImageWriteQueue.h"
#include "PostProcess/SceneRenderTargets.h"
#include "GlobalShader.h"
#include "SceneRenderTargetParameters.h"
#include "SceneRendering.h"
#include "DeferredShadingRenderer.h"
#include "ScenePrivate.h"
#include "PostProcess/SceneFilterRendering.h"
#include "ScreenRendering.h"
#include "MobileSceneCaptureRendering.h"
#include "ClearQuad.h"
#include "PipelineStateCache.h"
#include "RendererModule.h"

#include <sstream>

const TCHAR* GShaderSourceModeDefineName[] =
{
	TEXT("SOURCE_MODE_SCENE_COLOR_AND_OPACITY"),
	TEXT("SOURCE_MODE_SCENE_COLOR_NO_ALPHA"),
	nullptr,
	TEXT("SOURCE_MODE_SCENE_COLOR_SCENE_DEPTH"),
	TEXT("SOURCE_MODE_SCENE_DEPTH"),
	TEXT("SOURCE_MODE_DEVICE_DEPTH"),
	TEXT("SOURCE_MODE_NORMAL"),
	TEXT("SOURCE_MODE_BASE_COLOR")
};

/**
 * A pixel shader for capturing a component of the rendered scene for a scene capture.
 */
template<ESceneCaptureSource CaptureSource>
class TSceneCapturePS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(TSceneCapturePS,Global);
public:

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) 
	{ 
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM4);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		const TCHAR* DefineName = GShaderSourceModeDefineName[CaptureSource];
		if (DefineName)
		{
			OutEnvironment.SetDefine(DefineName, 1);
		}
	}

	TSceneCapturePS(const ShaderMetaType::CompiledShaderInitializerType& Initializer):
		FGlobalShader(Initializer)
	{
		SceneTextureParameters.Bind(Initializer);
	}
	TSceneCapturePS() {}

	void SetParameters(FRHICommandList& RHICmdList, const FSceneView& View)
	{
		FGlobalShader::SetParameters<FViewUniformShaderParameters>(RHICmdList, GetPixelShader(), View.ViewUniformBuffer);
		SceneTextureParameters.Set(RHICmdList, GetPixelShader(), View.FeatureLevel, ESceneTextureSetupMode::All);
	}

	virtual bool Serialize(FArchive& Ar) override
	{
		bool bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
		Ar << SceneTextureParameters;
		return bShaderHasOutdatedParameters;
	}

private:
	FSceneTextureShaderParameters SceneTextureParameters;
};

IMPLEMENT_SHADER_TYPE(template<>, TSceneCapturePS<SCS_SceneColorHDR>, TEXT("/Engine/Private/SceneCapturePixelShader.usf"), TEXT("Main"), SF_Pixel);
IMPLEMENT_SHADER_TYPE(template<>, TSceneCapturePS<SCS_SceneColorHDRNoAlpha>, TEXT("/Engine/Private/SceneCapturePixelShader.usf"), TEXT("Main"), SF_Pixel);
IMPLEMENT_SHADER_TYPE(template<>,TSceneCapturePS<SCS_SceneColorSceneDepth>,TEXT("/Engine/Private/SceneCapturePixelShader.usf"),TEXT("Main"),SF_Pixel);
IMPLEMENT_SHADER_TYPE(template<>,TSceneCapturePS<SCS_SceneDepth>,TEXT("/Engine/Private/SceneCapturePixelShader.usf"),TEXT("Main"),SF_Pixel);
IMPLEMENT_SHADER_TYPE(template<>, TSceneCapturePS<SCS_DeviceDepth>, TEXT("/Engine/Private/SceneCapturePixelShader.usf"), TEXT("Main"), SF_Pixel);
IMPLEMENT_SHADER_TYPE(template<>,TSceneCapturePS<SCS_Normal>,TEXT("/Engine/Private/SceneCapturePixelShader.usf"),TEXT("Main"),SF_Pixel);
IMPLEMENT_SHADER_TYPE(template<>,TSceneCapturePS<SCS_BaseColor>,TEXT("/Engine/Private/SceneCapturePixelShader.usf"),TEXT("Main"),SF_Pixel);

class FODSCapturePS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FODSCapturePS, Global);
public:

	static bool ShouldCache(EShaderPlatform Platform)
	{
		return true;
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	FODSCapturePS(const ShaderMetaType::CompiledShaderInitializerType& Initializer) :
		FGlobalShader(Initializer)
	{
		LeftEyeTexture.Bind(Initializer.ParameterMap, TEXT("LeftEyeTexture"));
		RightEyeTexture.Bind(Initializer.ParameterMap, TEXT("RightEyeTexture"));
		LeftEyeTextureSampler.Bind(Initializer.ParameterMap, TEXT("LeftEyeTextureSampler"));
		RightEyeTextureSampler.Bind(Initializer.ParameterMap, TEXT("RightEyeTextureSampler"));
	}

	FODSCapturePS() {}

	void SetParameters(FRHICommandList& RHICmdList, const FTextureRHIRef InLeftEyeTexture, const FTextureRHIRef InRightEyeTexture)
	{
		const FPixelShaderRHIParamRef ShaderRHI = GetPixelShader();
		
		SetTextureParameter(
			RHICmdList,
			ShaderRHI,
			LeftEyeTexture,
			LeftEyeTextureSampler,
			TStaticSamplerState<SF_Bilinear>::GetRHI(),
			InLeftEyeTexture);

		SetTextureParameter(
			RHICmdList,
			ShaderRHI,
			RightEyeTexture,
			RightEyeTextureSampler,
			TStaticSamplerState<SF_Bilinear>::GetRHI(),
			InRightEyeTexture);
	}

	virtual bool Serialize(FArchive& Ar) override
	{
		const bool bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
		Ar << LeftEyeTexture;
		Ar << RightEyeTexture;
		Ar << LeftEyeTextureSampler;
		Ar << RightEyeTextureSampler;
		return bShaderHasOutdatedParameters;
	}

	FShaderResourceParameter LeftEyeTexture;
	FShaderResourceParameter RightEyeTexture;
	FShaderResourceParameter LeftEyeTextureSampler;
	FShaderResourceParameter RightEyeTextureSampler;
};

IMPLEMENT_SHADER_TYPE(, FODSCapturePS, TEXT("/Engine/Private/ODSCapture.usf"), TEXT("MainPS"), SF_Pixel);


class FStereoCapturePS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FStereoCapturePS, Global);
public:

	static bool ShouldCache(EShaderPlatform Platform)
	{
		return true;
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	FStereoCapturePS(const ShaderMetaType::CompiledShaderInitializerType& Initializer) :
		FGlobalShader(Initializer)
	{
		LeftEyeTexture.Bind(Initializer.ParameterMap, TEXT("LeftEyeTexture"));
		RightEyeTexture.Bind(Initializer.ParameterMap, TEXT("RightEyeTexture"));
		LeftEyeTextureSampler.Bind(Initializer.ParameterMap, TEXT("LeftEyeTextureSampler"));
		RightEyeTextureSampler.Bind(Initializer.ParameterMap, TEXT("RightEyeTextureSampler"));
	}

	FStereoCapturePS() {}

	void SetParameters(FRHICommandList& RHICmdList, const FTextureRHIRef InLeftEyeTexture, const FTextureRHIRef InRightEyeTexture)
	{
		const FPixelShaderRHIParamRef ShaderRHI = GetPixelShader();

		SetTextureParameter(
			RHICmdList,
			ShaderRHI,
			LeftEyeTexture,
			LeftEyeTextureSampler,
			TStaticSamplerState<SF_Bilinear, AM_Mirror, AM_Mirror, AM_Mirror, 0, 1, 0, SCF_Less>::GetRHI(),
			InLeftEyeTexture);

		SetTextureParameter(
			RHICmdList,
			ShaderRHI,
			RightEyeTexture,
			RightEyeTextureSampler,
			TStaticSamplerState<SF_Bilinear, AM_Mirror, AM_Mirror, AM_Mirror, 0, 1, 0, SCF_Less>::GetRHI(),
			InRightEyeTexture);
	}

	virtual bool Serialize(FArchive& Ar) override
	{
		const bool bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
		Ar << LeftEyeTexture;
		Ar << RightEyeTexture;
		Ar << LeftEyeTextureSampler;
		Ar << RightEyeTextureSampler;
		return bShaderHasOutdatedParameters;
	}

	FShaderResourceParameter LeftEyeTexture;
	FShaderResourceParameter RightEyeTexture;
	FShaderResourceParameter LeftEyeTextureSampler;
	FShaderResourceParameter RightEyeTextureSampler;
};

IMPLEMENT_SHADER_TYPE(, FStereoCapturePS, TEXT("/Engine/Private/ODSCapture.usf"), TEXT("MainPS"), SF_Pixel);


void FDeferredShadingSceneRenderer::CopySceneCaptureComponentToTarget(FRHICommandListImmediate& RHICmdList)
{
	ESceneCaptureSource SceneCaptureSource = ViewFamily.SceneCaptureSource;

	if (IsAnyForwardShadingEnabled(ViewFamily.GetShaderPlatform()) && (SceneCaptureSource == SCS_Normal || SceneCaptureSource == SCS_BaseColor))
	{
		SceneCaptureSource = SCS_SceneColorHDR;
	}

	if (SceneCaptureSource != SCS_FinalColorLDR)
	{
		SCOPED_DRAW_EVENT(RHICmdList, CaptureSceneComponent);

		FGraphicsPipelineStateInitializer GraphicsPSOInit;
		GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
		GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			FViewInfo& View = Views[ViewIndex];
			FRHIRenderTargetView ColorView(ViewFamily.RenderTarget->GetRenderTargetTexture(), 0, -1, ERenderTargetLoadAction::ENoAction, ERenderTargetStoreAction::EStore);
			FRHISetRenderTargetsInfo Info(1, &ColorView, FRHIDepthRenderTargetView());
			RHICmdList.SetRenderTargetsAndClear(Info);
			RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

			if (SceneCaptureSource == SCS_SceneColorHDR && ViewFamily.SceneCaptureCompositeMode == SCCM_Composite)
			{
				// Blend with existing render target color. Scene capture color is already pre-multiplied by alpha.
				GraphicsPSOInit.BlendState = TStaticBlendState<CW_RGBA, BO_Add, BF_One, BF_SourceAlpha, BO_Add, BF_Zero, BF_SourceAlpha>::GetRHI();
			}
			else if (SceneCaptureSource == SCS_SceneColorHDR && ViewFamily.SceneCaptureCompositeMode == SCCM_Additive)
			{
				// Add to existing render target color. Scene capture color is already pre-multiplied by alpha.
				GraphicsPSOInit.BlendState = TStaticBlendState<CW_RGBA, BO_Add, BF_One, BF_One, BO_Add, BF_Zero, BF_SourceAlpha>::GetRHI();
			}
			else
			{
				GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
			}

			TShaderMapRef<FScreenVS> VertexShader(View.ShaderMap);
			GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
			GraphicsPSOInit.BoundShaderState.VertexShaderRHI = GETSAFERHISHADER_VERTEX(*VertexShader);
			GraphicsPSOInit.PrimitiveType = PT_TriangleList;

			if (SceneCaptureSource == SCS_SceneColorHDR)
			{
				TShaderMapRef<TSceneCapturePS<SCS_SceneColorHDR> > PixelShader(View.ShaderMap);
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);
				SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

				PixelShader->SetParameters(RHICmdList, View);
			}
			else if (SceneCaptureSource == SCS_SceneColorHDRNoAlpha)
			{
				TShaderMapRef<TSceneCapturePS<SCS_SceneColorHDRNoAlpha> > PixelShader(View.ShaderMap);
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);
				SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

				PixelShader->SetParameters(RHICmdList, View);
			}
			else if (SceneCaptureSource == SCS_SceneColorSceneDepth)
			{
				TShaderMapRef<TSceneCapturePS<SCS_SceneColorSceneDepth> > PixelShader(View.ShaderMap);
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);
				SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

				PixelShader->SetParameters(RHICmdList, View);
			}
			else if (SceneCaptureSource == SCS_SceneDepth)
			{
				TShaderMapRef<TSceneCapturePS<SCS_SceneDepth> > PixelShader(View.ShaderMap);
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);
				SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

				PixelShader->SetParameters(RHICmdList, View);
			}
			else if (ViewFamily.SceneCaptureSource == SCS_DeviceDepth)
			{
				TShaderMapRef<TSceneCapturePS<SCS_DeviceDepth> > PixelShader(View.ShaderMap);
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);
				SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

				PixelShader->SetParameters(RHICmdList, View);
			}
			else if (SceneCaptureSource == SCS_Normal)
			{
				TShaderMapRef<TSceneCapturePS<SCS_Normal> > PixelShader(View.ShaderMap);
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);
				SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

				PixelShader->SetParameters(RHICmdList, View);
			}
			else if (SceneCaptureSource == SCS_BaseColor)
			{
				TShaderMapRef<TSceneCapturePS<SCS_BaseColor> > PixelShader(View.ShaderMap);
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);
				SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

				PixelShader->SetParameters(RHICmdList, View);
			}
			else
			{
				check(0);
			}

			VertexShader->SetParameters(RHICmdList, View.ViewUniformBuffer);

			DrawRectangle(
				RHICmdList,
				View.ViewRect.Min.X, View.ViewRect.Min.Y,
				View.ViewRect.Width(), View.ViewRect.Height(),
				View.ViewRect.Min.X, View.ViewRect.Min.Y,
				View.ViewRect.Width(), View.ViewRect.Height(),
				View.UnconstrainedViewRect.Size(),
				FSceneRenderTargets::Get(RHICmdList).GetBufferSizeXY(),
				*VertexShader,
				EDRF_UseTriangleOptimization);
		}
	}
}

static void UpdateSceneCaptureContentDeferred_RenderThread(
	FRHICommandListImmediate& RHICmdList, 
	FSceneRenderer* SceneRenderer, 
	FRenderTarget* RenderTarget, 
	FTexture* RenderTargetTexture, 
	const FString& EventName, 
	const FResolveParams& ResolveParams)
{
	FMemMark MemStackMark(FMemStack::Get());

	// update any resources that needed a deferred update
	FDeferredUpdateResource::UpdateResources(RHICmdList);
	{
#if WANTS_DRAW_MESH_EVENTS
		SCOPED_DRAW_EVENTF(RHICmdList, SceneCapture, TEXT("SceneCapture %s"), *EventName);
#else
		SCOPED_DRAW_EVENT(RHICmdList, UpdateSceneCaptureContent_RenderThread);
#endif

		const FRenderTarget* Target = SceneRenderer->ViewFamily.RenderTarget;

		// TODO: Could avoid the clear by replacing with dummy black system texture.
		FViewInfo& View = SceneRenderer->Views[0];
		SetRenderTarget(RHICmdList, Target->GetRenderTargetTexture(), nullptr, true);
		DrawClearQuad(RHICmdList, true, FLinearColor::Black, false, 0, false, 0, Target->GetSizeXY(), View.UnscaledViewRect);

		// Render the scene normally
		{
			SCOPED_DRAW_EVENT(RHICmdList, RenderScene);
			SceneRenderer->Render(RHICmdList);
		}

		// Note: When the ViewFamily.SceneCaptureSource requires scene textures (i.e. SceneCaptureSource != SCS_FinalColorLDR), the copy to RenderTarget 
		// will be done in CopySceneCaptureComponentToTarget while the GBuffers are still alive for the frame.
		RHICmdList.CopyToResolveTarget(RenderTarget->GetRenderTargetTexture(), RenderTargetTexture->TextureRHI, ResolveParams);
	}

	FSceneRenderer::WaitForTasksClearSnapshotsAndDeleteSceneRenderer(RHICmdList, SceneRenderer);
}

static void ODSCapture_RenderThread(
	FRHICommandListImmediate& RHICmdList,
	const FTexture* const LeftEyeTexture,
	const FTexture* const RightEyeTexture,
	FRenderTarget* const RenderTarget, 
	const ERHIFeatureLevel::Type FeatureLevel)
{
	SetRenderTarget(RHICmdList, RenderTarget->GetRenderTargetTexture(), nullptr, ESimpleRenderTargetMode::EExistingColorAndDepth, FExclusiveDepthStencil::DepthNop_StencilNop, true);

	FGraphicsPipelineStateInitializer GraphicsPSOInit;
	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
	GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();

	const auto ShaderMap = GetGlobalShaderMap(FeatureLevel);	
	TShaderMapRef<FScreenVS> VertexShader(ShaderMap);
	TShaderMapRef<FODSCapturePS> PixelShader(ShaderMap);
	extern TGlobalResource<FFilterVertexDeclaration> GFilterVertexDeclaration;

	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = GETSAFERHISHADER_VERTEX(*VertexShader);
	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);
	GraphicsPSOInit.PrimitiveType = PT_TriangleList;

	SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

	PixelShader->SetParameters(RHICmdList, LeftEyeTexture->TextureRHI->GetTextureCube(), RightEyeTexture->TextureRHI->GetTextureCube());

	const FIntPoint& TargetSize = RenderTarget->GetSizeXY();
	RHICmdList.SetViewport(0, 0, 0.0f, TargetSize.X, TargetSize.Y, 1.0f);

	DrawRectangle(
		RHICmdList,
		0, 0,
		static_cast<float>(TargetSize.X), static_cast<float>(TargetSize.Y),
		0, 0,
		TargetSize.X, TargetSize.Y,
		TargetSize,
		TargetSize,
		*VertexShader,
		EDRF_UseTriangleOptimization);
}

static void StereoCapture_RenderThread
(
	FRHICommandListImmediate& RHICmdList,
	const FTexture* const LeftEyeTexture,
	const FTexture* const RightEyeTexture,
	FRenderTarget* const RenderTarget,
	const ERHIFeatureLevel::Type FeatureLevel
)
{
	SetRenderTarget(RHICmdList, RenderTarget->GetRenderTargetTexture(), nullptr, ESimpleRenderTargetMode::EExistingColorAndDepth, FExclusiveDepthStencil::DepthNop_StencilNop, true);

	FGraphicsPipelineStateInitializer GraphicsPSOInit;
	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
	GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();

	const auto ShaderMap = GetGlobalShaderMap(FeatureLevel);
	TShaderMapRef<FScreenVS> VertexShader(ShaderMap);
	TShaderMapRef<FStereoCapturePS> PixelShader(ShaderMap);
	extern TGlobalResource<FFilterVertexDeclaration> GFilterVertexDeclaration;

	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = GETSAFERHISHADER_VERTEX(*VertexShader);
	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);
	GraphicsPSOInit.PrimitiveType = PT_TriangleList;

	SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

	PixelShader->SetParameters(RHICmdList, LeftEyeTexture->TextureRHI->GetTextureCube(), RightEyeTexture->TextureRHI->GetTextureCube());

	const FIntPoint& TargetSize = RenderTarget->GetSizeXY();
	RHICmdList.SetViewport(0, 0, 0.0f, TargetSize.X, TargetSize.Y, 1.0f);

	DrawRectangle
	(
		RHICmdList,
		0, 0,
		static_cast<float>(TargetSize.X), static_cast<float>(TargetSize.Y),
		0, 0,
		TargetSize.X, TargetSize.Y,
		TargetSize,
		TargetSize,
		*VertexShader,
		EDRF_UseTriangleOptimization
	);
}

static void UpdateSceneCaptureContent_RenderThread(
	FRHICommandListImmediate& RHICmdList,
	FSceneRenderer* SceneRenderer,
	FRenderTarget* RenderTarget,
	FTexture* RenderTargetTexture,
	const FString& EventName,
	const FResolveParams& ResolveParams)
{
	FMaterialRenderProxy::UpdateDeferredCachedUniformExpressions();

	switch (SceneRenderer->Scene->GetShadingPath())
	{
		case EShadingPath::Mobile:
		{
			UpdateSceneCaptureContentMobile_RenderThread(
				RHICmdList,
				SceneRenderer,
				RenderTarget,
				RenderTargetTexture,
				EventName,
				ResolveParams);
			break;
		}
		case EShadingPath::Deferred:
		{
			UpdateSceneCaptureContentDeferred_RenderThread(
				RHICmdList,
				SceneRenderer,
				RenderTarget,
				RenderTargetTexture,
				EventName,
				ResolveParams);
			break;
		}
		default:
			checkNoEntry();
			break;
		}
}

void BuildProjectionMatrix(FIntPoint RenderTargetSize, ECameraProjectionMode::Type ProjectionType, float FOV, float InOrthoWidth, FMatrix& ProjectionMatrix)
{
	float const XAxisMultiplier = 1.0f;
	float const YAxisMultiplier = RenderTargetSize.X / (float)RenderTargetSize.Y;

	if (ProjectionType == ECameraProjectionMode::Orthographic)
	{
		check((int32)ERHIZBuffer::IsInverted);
		const float OrthoWidth = InOrthoWidth / 2.0f;
		const float OrthoHeight = InOrthoWidth / 2.0f * XAxisMultiplier / YAxisMultiplier;

		const float NearPlane = 0;
		const float FarPlane = WORLD_MAX / 8.0f;

		const float ZScale = 1.0f / (FarPlane - NearPlane);
		const float ZOffset = -NearPlane;

		ProjectionMatrix = FReversedZOrthoMatrix(
			OrthoWidth,
			OrthoHeight,
			ZScale,
			ZOffset
			);
	}
	else
	{
		if ((int32)ERHIZBuffer::IsInverted)
		{
			ProjectionMatrix = FReversedZPerspectiveMatrix(
				FOV,
				FOV,
				XAxisMultiplier,
				YAxisMultiplier,
				GNearClippingPlane,
				GNearClippingPlane
				);
		}
		else
		{
			ProjectionMatrix = FPerspectiveMatrix(
				FOV,
				FOV,
				XAxisMultiplier,
				YAxisMultiplier,
				GNearClippingPlane,
				GNearClippingPlane
				);
		}
	}
}

void SetupViewVamilyForSceneCapture(
	FSceneViewFamily& ViewFamily,
	USceneCaptureComponent* SceneCaptureComponent,
	const TArrayView<const FSceneCaptureViewInfo> Views,
	float MaxViewDistance,
	bool bCaptureSceneColor,
	bool bIsPlanarReflection,
	FPostProcessSettings* PostProcessSettings,
	float PostProcessBlendWeight,
	const AActor* ViewActor)
{
	check(!ViewFamily.GetScreenPercentageInterface());

	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ++ViewIndex)
	{
		const FSceneCaptureViewInfo& SceneCaptureViewInfo = Views[ViewIndex];

		FSceneViewInitOptions ViewInitOptions;
		ViewInitOptions.SetViewRectangle(SceneCaptureViewInfo.ViewRect);
		ViewInitOptions.ViewFamily = &ViewFamily;
		ViewInitOptions.ViewActor = ViewActor;
		ViewInitOptions.ViewOrigin = SceneCaptureViewInfo.ViewLocation;
		ViewInitOptions.ViewRotationMatrix = SceneCaptureViewInfo.ViewRotationMatrix;
		ViewInitOptions.BackgroundColor = FLinearColor::Black;
		ViewInitOptions.OverrideFarClippingPlaneDistance = MaxViewDistance;
		ViewInitOptions.StereoPass = SceneCaptureViewInfo.StereoPass;
		ViewInitOptions.SceneViewStateInterface = SceneCaptureComponent->GetViewState(ViewIndex);
		ViewInitOptions.ProjectionMatrix = SceneCaptureViewInfo.ProjectionMatrix;
		ViewInitOptions.LODDistanceFactor = FMath::Clamp(SceneCaptureComponent->LODDistanceFactor, .01f, 100.0f);

		if (ViewFamily.Scene->GetWorld() != nullptr && ViewFamily.Scene->GetWorld()->GetWorldSettings() != nullptr)
		{
			ViewInitOptions.WorldToMetersScale = ViewFamily.Scene->GetWorld()->GetWorldSettings()->WorldToMeters;
		}
		ViewInitOptions.StereoIPD = SceneCaptureViewInfo.StereoIPD * (ViewInitOptions.WorldToMetersScale / 100.0f);

		if (bCaptureSceneColor)
		{
			ViewFamily.EngineShowFlags.PostProcessing = 0;
			ViewInitOptions.OverlayColor = FLinearColor::Black;
		}

		FSceneView* View = new FSceneView(ViewInitOptions);

		View->bIsSceneCapture = true;
		// Note: this has to be set before EndFinalPostprocessSettings
		View->bIsPlanarReflection = bIsPlanarReflection;

		check(SceneCaptureComponent);
		for (auto It = SceneCaptureComponent->HiddenComponents.CreateConstIterator(); It; ++It)
		{
			// If the primitive component was destroyed, the weak pointer will return NULL.
			UPrimitiveComponent* PrimitiveComponent = It->Get();
			if (PrimitiveComponent)
			{
				View->HiddenPrimitives.Add(PrimitiveComponent->ComponentId);
			}
		}

		for (auto It = SceneCaptureComponent->HiddenActors.CreateConstIterator(); It; ++It)
		{
			AActor* Actor = *It;

			if (Actor)
			{
				for (UActorComponent* Component : Actor->GetComponents())
				{
					if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Component))
					{
						View->HiddenPrimitives.Add(PrimComp->ComponentId);
					}
				}
			}
		}

		if (SceneCaptureComponent->PrimitiveRenderMode == ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList)
		{
			View->ShowOnlyPrimitives.Emplace();

			for (auto It = SceneCaptureComponent->ShowOnlyComponents.CreateConstIterator(); It; ++It)
			{
				// If the primitive component was destroyed, the weak pointer will return NULL.
				UPrimitiveComponent* PrimitiveComponent = It->Get();
				if (PrimitiveComponent)
				{
					View->ShowOnlyPrimitives->Add(PrimitiveComponent->ComponentId);
				}
			}

			for (auto It = SceneCaptureComponent->ShowOnlyActors.CreateConstIterator(); It; ++It)
			{
				AActor* Actor = *It;

				if (Actor)
				{
					for (UActorComponent* Component : Actor->GetComponents())
					{
						if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Component))
						{
							View->ShowOnlyPrimitives->Add(PrimComp->ComponentId);
						}
					}
				}
			}
		}
		else if (SceneCaptureComponent->ShowOnlyComponents.Num() > 0 || SceneCaptureComponent->ShowOnlyActors.Num() > 0)
		{
			static bool bWarned = false;

			if (!bWarned)
			{
				UE_LOG(LogRenderer, Log, TEXT("Scene Capture has ShowOnlyComponents or ShowOnlyActors ignored by the PrimitiveRenderMode setting! %s"), *SceneCaptureComponent->GetPathName());
				bWarned = true;
			}
		}

		ViewFamily.Views.Add(View);

		View->StartFinalPostprocessSettings(SceneCaptureViewInfo.ViewLocation);
		View->OverridePostProcessSettings(*PostProcessSettings, PostProcessBlendWeight);
		View->EndFinalPostprocessSettings(ViewInitOptions);
	}
}

static FSceneRenderer* CreateSceneRendererForSceneCapture(
	FScene* Scene,
	USceneCaptureComponent* SceneCaptureComponent,
	FRenderTarget* RenderTarget,
	FIntPoint RenderTargetSize,
	const FMatrix& ViewRotationMatrix,
	const FVector& ViewLocation,
	const FMatrix& ProjectionMatrix,
	float MaxViewDistance,
	bool bCaptureSceneColor,
	FPostProcessSettings* PostProcessSettings,
	float PostProcessBlendWeight,
	const AActor* ViewActor, 
	const float StereoIPD = 0.0f)
{
	FSceneCaptureViewInfo SceneCaptureViewInfo;
	SceneCaptureViewInfo.ViewRotationMatrix = ViewRotationMatrix;
	SceneCaptureViewInfo.ViewLocation = ViewLocation;
	SceneCaptureViewInfo.ProjectionMatrix = ProjectionMatrix;
	SceneCaptureViewInfo.StereoPass = EStereoscopicPass::eSSP_FULL;
	SceneCaptureViewInfo.StereoIPD = StereoIPD;
	SceneCaptureViewInfo.ViewRect = FIntRect(0, 0, RenderTargetSize.X, RenderTargetSize.Y);

	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		RenderTarget,
		Scene,
		SceneCaptureComponent->ShowFlags)
		.SetResolveScene(!bCaptureSceneColor)
		.SetRealtimeUpdate(SceneCaptureComponent->bCaptureEveryFrame || SceneCaptureComponent->bAlwaysPersistRenderingState));

	SetupViewVamilyForSceneCapture(
		ViewFamily,
		SceneCaptureComponent,
		{ SceneCaptureViewInfo },
		MaxViewDistance, 
		bCaptureSceneColor,
		/* bIsPlanarReflection = */ false,
		PostProcessSettings, 
		PostProcessBlendWeight,
		ViewActor);

	// Screen percentage is still not supported in scene capture.
	ViewFamily.EngineShowFlags.ScreenPercentage = false;
	ViewFamily.SetScreenPercentageInterface(new FLegacyScreenPercentageDriver(
		ViewFamily, /* GlobalResolutionFraction = */ 1.0f, /* AllowPostProcessSettingsScreenPercentage = */ false));

	return FSceneRenderer::CreateSceneRenderer(&ViewFamily, nullptr);
}

void SetupViewVamilyForSceneCapture
(
	FSceneViewFamily& ViewFamily,
	UStereoSceneCaptureComponent* SceneCaptureComponent,
	FSceneCaptureViewInfo ViewInfo
)
{
	FSceneViewInitOptions ViewInitOptions;
	ViewInitOptions.SetViewRectangle(ViewInfo.ViewRect);
	ViewInitOptions.ViewFamily = &ViewFamily;
	ViewInitOptions.ViewActor = SceneCaptureComponent->GetOwner();
	ViewInitOptions.ViewOrigin = ViewInfo.ViewLocation;
	ViewInitOptions.ViewRotationMatrix = ViewInfo.ViewRotationMatrix;
	ViewInitOptions.BackgroundColor = FLinearColor::Black;
	ViewInitOptions.OverrideFarClippingPlaneDistance = SceneCaptureComponent->MaxViewDistance;
	ViewInitOptions.StereoPass = ViewInfo.StereoPass;
	ViewInitOptions.SceneViewStateInterface = nullptr;
	ViewInitOptions.ProjectionMatrix = ViewInfo.ProjectionMatrix;
	ViewInitOptions.LODDistanceFactor = FMath::Clamp(SceneCaptureComponent->LODDistanceFactor, .01f, 100.0f);

	if (ViewFamily.Scene->GetWorld() != nullptr && ViewFamily.Scene->GetWorld()->GetWorldSettings() != nullptr)
		ViewInitOptions.WorldToMetersScale = ViewFamily.Scene->GetWorld()->GetWorldSettings()->WorldToMeters;

	ViewInitOptions.StereoIPD = ViewInfo.StereoIPD * (ViewInitOptions.WorldToMetersScale / 100.0f);

	FSceneView* View = new FSceneView(ViewInitOptions);

	View->bIsSceneCapture = true;
	View->bIsPlanarReflection = false;

	ViewFamily.Views.Add(View);

	View->StartFinalPostprocessSettings(ViewInfo.ViewLocation);
	View->OverridePostProcessSettings(SceneCaptureComponent->PostProcessSettings, SceneCaptureComponent->PostProcessBlendWeight);
	View->EndFinalPostprocessSettings(ViewInitOptions);
}

static FSceneRenderer* CreateSceneRendererForSceneCapture
(
	FScene* Scene,
	UStereoSceneCaptureComponent* SceneCaptureComponent,
	FRenderTarget* RenderTarget,
	FIntPoint RenderTargetSize,
	const FMatrix& ViewRotationMatrix,
	const FVector& ViewLocation,
	const FMatrix& ProjectionMatrix
)
{
	FSceneCaptureViewInfo SceneCaptureViewInfo;
	SceneCaptureViewInfo.ViewRotationMatrix = ViewRotationMatrix;
	SceneCaptureViewInfo.ViewLocation = ViewLocation;
	SceneCaptureViewInfo.ProjectionMatrix = ProjectionMatrix;
	SceneCaptureViewInfo.StereoPass = EStereoscopicPass::eSSP_FULL;
	SceneCaptureViewInfo.StereoIPD = SceneCaptureComponent->IPD;
	SceneCaptureViewInfo.ViewRect = FIntRect(0, 0, RenderTargetSize.X, RenderTargetSize.Y);

	FSceneViewFamilyContext ViewFamily
	(
		FSceneViewFamily::ConstructionValues
		(
			RenderTarget,
			Scene,
			SceneCaptureComponent->ShowFlags
		).SetResolveScene(true)
		 .SetRealtimeUpdate(false)
	);

	SetupViewVamilyForSceneCapture
	(
		ViewFamily,
		SceneCaptureComponent,
		SceneCaptureViewInfo
	);

	ViewFamily.EngineShowFlags.ScreenPercentage = false;
	ViewFamily.SetScreenPercentageInterface
	(
		new FLegacyScreenPercentageDriver
		(
			ViewFamily, 
			/* GlobalResolutionFraction = */ 1.0f, 
			/* AllowPostProcessSettingsScreenPercentage = */ false
		)
	);

	FSceneRenderer *SceneRenderer = FSceneRenderer::CreateSceneRenderer(&ViewFamily, nullptr);

	SceneRenderer->ViewFamily.SceneCaptureSource = SCS_FinalColorLDR;

	return SceneRenderer;
}


void FScene::UpdateSceneCaptureContents(USceneCaptureComponent2D* CaptureComponent)
{
	check(CaptureComponent);

	if (CaptureComponent->TextureTarget)
	{
		// Only ensure motion blur cache is up to date when doing USceneCaptureComponent2D::CaptureScene(),
		// but only when bAlwaysPersistRenderingState == true for backward compatibility.
		if (!CaptureComponent->bCaptureEveryFrame && CaptureComponent->bAlwaysPersistRenderingState)
		{
			// We assume the world is not paused since the CaptureScene() has manually been called.
			EnsureMotionBlurCacheIsUpToDate(/* bWorldIsPaused = */ false);
		}

		FTransform Transform = CaptureComponent->GetComponentToWorld();
		FVector ViewLocation = Transform.GetTranslation();

		// Remove the translation from Transform because we only need rotation.
		Transform.SetTranslation(FVector::ZeroVector);
		Transform.SetScale3D(FVector::OneVector);
		FMatrix ViewRotationMatrix = Transform.ToInverseMatrixWithScale();

		// swap axis st. x=z,y=x,z=y (unreal coord space) so that z is up
		ViewRotationMatrix = ViewRotationMatrix * FMatrix(
			FPlane(0, 0, 1, 0),
			FPlane(1, 0, 0, 0),
			FPlane(0, 1, 0, 0),
			FPlane(0, 0, 0, 1));
		const float FOV = CaptureComponent->FOVAngle * (float)PI / 360.0f;
		FIntPoint CaptureSize(CaptureComponent->TextureTarget->GetSurfaceWidth(), CaptureComponent->TextureTarget->GetSurfaceHeight());

		FMatrix ProjectionMatrix;
		if (CaptureComponent->bUseCustomProjectionMatrix)
		{
			ProjectionMatrix = CaptureComponent->CustomProjectionMatrix;
		}
		else
		{
			BuildProjectionMatrix(CaptureSize, CaptureComponent->ProjectionType, FOV, CaptureComponent->OrthoWidth, ProjectionMatrix);
		}

		const bool bUseSceneColorTexture = CaptureComponent->CaptureSource != SCS_FinalColorLDR;

		FSceneRenderer* SceneRenderer = CreateSceneRendererForSceneCapture(
			this, 
			CaptureComponent, 
			CaptureComponent->TextureTarget->GameThread_GetRenderTargetResource(), 
			CaptureSize, 
			ViewRotationMatrix, 
			ViewLocation, 
			ProjectionMatrix, 
			CaptureComponent->MaxViewDistanceOverride, 
			bUseSceneColorTexture,
			&CaptureComponent->PostProcessSettings, 
			CaptureComponent->PostProcessBlendWeight,
			CaptureComponent->GetViewOwner());

		SceneRenderer->ViewFamily.SceneCaptureSource = CaptureComponent->CaptureSource;
		SceneRenderer->ViewFamily.SceneCaptureCompositeMode = CaptureComponent->CompositeMode;

		{
			FPlane ClipPlane = FPlane(CaptureComponent->ClipPlaneBase, CaptureComponent->ClipPlaneNormal.GetSafeNormal());

			for (FSceneView& View : SceneRenderer->Views)
			{
				View.bCameraCut = CaptureComponent->bCameraCutThisFrame;

				if (CaptureComponent->bEnableClipPlane)
				{
					View.GlobalClippingPlane = ClipPlane;
					// Jitter can't be removed completely due to the clipping plane
					View.bAllowTemporalJitter = false;
				}
			}
		}

		// Reset scene capture's camera cut.
		CaptureComponent->bCameraCutThisFrame = false;

		FTextureRenderTargetResource* TextureRenderTarget = CaptureComponent->TextureTarget->GameThread_GetRenderTargetResource();

		FString EventName;
		if (!CaptureComponent->ProfilingEventName.IsEmpty())
		{
			EventName = CaptureComponent->ProfilingEventName;
		}
		else if (CaptureComponent->GetOwner())
		{
			CaptureComponent->GetOwner()->GetFName().ToString(EventName);
		}

		ENQUEUE_RENDER_COMMAND(CaptureCommand)(
			[SceneRenderer, TextureRenderTarget, EventName](FRHICommandListImmediate& RHICmdList)
			{
				UpdateSceneCaptureContent_RenderThread(RHICmdList, SceneRenderer, TextureRenderTarget, TextureRenderTarget, EventName, FResolveParams());
			}
		);
	}
}

void FScene::UpdateSceneCaptureContents(USceneCaptureComponentCube* CaptureComponent)
{
	struct FLocal
	{
		/** Creates a transformation for a cubemap face, following the D3D cubemap layout. */
		static FMatrix CalcCubeFaceTransform(ECubeFace Face)
		{
			static const FVector XAxis(1.f, 0.f, 0.f);
			static const FVector YAxis(0.f, 1.f, 0.f);
			static const FVector ZAxis(0.f, 0.f, 1.f);

			// vectors we will need for our basis
			FVector vUp(YAxis);
			FVector vDir;
			switch (Face)
			{
			case CubeFace_PosX:
				vDir = XAxis;
				break;
			case CubeFace_NegX:
				vDir = -XAxis;
				break;
			case CubeFace_PosY:
				vUp = -ZAxis;
				vDir = YAxis;
				break;
			case CubeFace_NegY:
				vUp = ZAxis;
				vDir = -YAxis;
				break;
			case CubeFace_PosZ:
				vDir = ZAxis;
				break;
			case CubeFace_NegZ:
				vDir = -ZAxis;
				break;
			}
			// derive right vector
			FVector vRight(vUp ^ vDir);
			// create matrix from the 3 axes
			return FBasisVectorMatrix(vRight, vUp, vDir, FVector::ZeroVector);
		}
	};

	check(CaptureComponent);

	const bool bIsODS = CaptureComponent->TextureTargetLeft && CaptureComponent->TextureTargetRight && CaptureComponent->TextureTargetODS;
	const uint32 StartIndex = (bIsODS) ? 1 : 0;
	const uint32 EndIndex = (bIsODS) ? 3 : 1;

	UTextureRenderTargetCube* const TextureTargets[] = {
		CaptureComponent->TextureTarget,
		CaptureComponent->TextureTargetLeft,
		CaptureComponent->TextureTargetRight
	};

	for (uint32 CaptureIter = StartIndex; CaptureIter < EndIndex; ++CaptureIter)
	{
		UTextureRenderTargetCube* const TextureTarget = TextureTargets[CaptureIter];

		if (GetFeatureLevel() >= ERHIFeatureLevel::SM4 && TextureTarget)
		{
			const float FOV = 90 * (float)PI / 360.0f;
			for (int32 faceidx = 0; faceidx < (int32)ECubeFace::CubeFace_MAX; faceidx++)
			{
				const ECubeFace TargetFace = (ECubeFace)faceidx;
				const FVector Location = CaptureComponent->GetComponentToWorld().GetTranslation();
				const FMatrix ViewRotationMatrix = FLocal::CalcCubeFaceTransform(TargetFace);
				FIntPoint CaptureSize(TextureTarget->GetSurfaceWidth(), TextureTarget->GetSurfaceHeight());
				FMatrix ProjectionMatrix;
				BuildProjectionMatrix(CaptureSize, ECameraProjectionMode::Perspective, FOV, 1.0f, ProjectionMatrix);
				FPostProcessSettings PostProcessSettings;

				float StereoIPD = 0.0f;
				if (bIsODS)
				{
					StereoIPD = (CaptureIter == 1) ? CaptureComponent->IPD * -0.5f : CaptureComponent->IPD * 0.5f;
				}

				FSceneRenderer* SceneRenderer = CreateSceneRendererForSceneCapture(this, CaptureComponent, TextureTarget->GameThread_GetRenderTargetResource(), CaptureSize, ViewRotationMatrix, Location, ProjectionMatrix, CaptureComponent->MaxViewDistanceOverride, true, &PostProcessSettings, 0, CaptureComponent->GetViewOwner(), StereoIPD);
				SceneRenderer->ViewFamily.SceneCaptureSource = SCS_SceneColorHDR;

				FTextureRenderTargetCubeResource* TextureRenderTarget = static_cast<FTextureRenderTargetCubeResource*>(TextureTarget->GameThread_GetRenderTargetResource());
				FString EventName;
				if (!CaptureComponent->ProfilingEventName.IsEmpty())
				{
					EventName = CaptureComponent->ProfilingEventName;
				}
				else if (CaptureComponent->GetOwner())
				{
					CaptureComponent->GetOwner()->GetFName().ToString(EventName);
				}
				ENQUEUE_RENDER_COMMAND(CaptureCommand)(
					[SceneRenderer, TextureRenderTarget, EventName, TargetFace](FRHICommandListImmediate& RHICmdList)
				{
					UpdateSceneCaptureContent_RenderThread(RHICmdList, SceneRenderer, TextureRenderTarget, TextureRenderTarget, EventName, FResolveParams(FResolveRect(), TargetFace));
				}
				);
			}
		}
	}

	if (bIsODS)
	{
		const FTextureRenderTargetCubeResource* const LeftEye = static_cast<FTextureRenderTargetCubeResource*>(CaptureComponent->TextureTargetLeft->GameThread_GetRenderTargetResource());
		const FTextureRenderTargetCubeResource* const RightEye = static_cast<FTextureRenderTargetCubeResource*>(CaptureComponent->TextureTargetRight->GameThread_GetRenderTargetResource());
		FTextureRenderTargetResource* const RenderTarget = CaptureComponent->TextureTargetODS->GameThread_GetRenderTargetResource();
		const ERHIFeatureLevel::Type InFeatureLevel = FeatureLevel;

		ENQUEUE_RENDER_COMMAND(ODSCaptureCommand)(
			[LeftEye, RightEye, RenderTarget, InFeatureLevel](FRHICommandListImmediate& RHICmdList)
		{
			ODSCapture_RenderThread(RHICmdList, LeftEye, RightEye, RenderTarget, InFeatureLevel);
		}
		);
	}
}

static FMatrix GetBasisFromCubeFace(ECubeFace Face)
{
	static const FVector XAxis(1.f, 0.f, 0.f);
	static const FVector YAxis(0.f, 1.f, 0.f);
	static const FVector ZAxis(0.f, 0.f, 1.f);

	// vectors we will need for our basis
	FVector vUp(YAxis);
	FVector vDir;

	switch (Face)
	{
	case CubeFace_PosX:
		vDir = XAxis;
		break;
	case CubeFace_NegX:
		vDir = -XAxis;
		break;
	case CubeFace_PosY:
		vUp = -ZAxis;
		vDir = YAxis;
		break;
	case CubeFace_NegY:
		vUp = ZAxis;
		vDir = -YAxis;
		break;
	case CubeFace_PosZ:
		vDir = ZAxis;
		break;
	case CubeFace_NegZ:
		vDir = -ZAxis;
		break;
	}

	// derive right vector
	FVector vRight(vUp ^ vDir);
	
	// create matrix from the 3 axes
	return FBasisVectorMatrix(vRight, vUp, vDir, FVector::ZeroVector);
}

void SaveRenderTextureToFile
(
	FTextureRenderTargetResource *RenderTexture,
	const FIntPoint &DestSize,
	const FString &Directory, 
	size_t FrameIndex
)
{
	std::stringstream stream;
	stream << FrameIndex;

	const FString FilePath = Directory / ("frame_original_" + stream.str()).c_str();

	TUniquePtr<TImagePixelData<FColor>> PixelData = MakeUnique<TImagePixelData<FColor>>(DestSize);
	FReadSurfaceDataFlags ReadPixelFlags(RCM_UNorm);
	ReadPixelFlags.SetLinearToGamma(true);
	RenderTexture->ReadPixels(PixelData->Pixels, ReadPixelFlags);

	TUniquePtr<FImageWriteTask> ImageTask = MakeUnique<FImageWriteTask>();
	ImageTask->PixelData = MoveTemp(PixelData);
	ImageTask->Filename = FilePath;
	ImageTask->Format = EImageFormat::JPEG; /* >>> FORMAT <<< */
	ImageTask->CompressionQuality = (int32)EImageCompressionQuality::Default;
	ImageTask->bOverwriteFile = true;
	ImageTask->PixelPreProcessors.Add(TAsyncAlphaWrite<FColor>(255));

	FHighResScreenshotConfig &HighResScreenshotConfig = GetHighResScreenshotConfig();
	HighResScreenshotConfig.ImageWriteQueue->Enqueue(MoveTemp(ImageTask));//.Get();
}

void FScene::UpdateSceneCaptureContents(class UStereoSceneCaptureComponent *CaptureComponent)
{
	bool IsAllPresent = CaptureComponent->TextureTargetLeft
					 && CaptureComponent->TextureTargetRight
				  	 && CaptureComponent->TextureTargetODS;

	if (!IsAllPresent)
	{
		UE_LOG(LogTemp, Log, TEXT(">>> Missing texture for StereoSceneCapture <<<"));
		return;
	}

	if (GetFeatureLevel() < ERHIFeatureLevel::SM4)
	{
		UE_LOG(LogTemp, Log, TEXT(">>> Feature Level < ERHIFeatureLevel::SM4 <<<"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT(">>> CAPTURE %d <<<"), CaptureComponent->FrameIndex);

	UTextureRenderTargetCube *Targets[] = 
	{
		CaptureComponent->TextureTargetLeft,
		CaptureComponent->TextureTargetRight
	};

	constexpr float FOV = 90 * (float)PI / 360.0f;

	constexpr int TARGETS_NUM = 2, FACES_NUM = 6;
	constexpr int LEFT_TEXTURE = 0;

	constexpr bool USE_SCENE_COLOR = false;

	FSceneRenderer* SceneRenderer = nullptr;

	ECubeFace face;
	FVector Location;
	FIntPoint CaptureSize;
	FMatrix ProjectionMatrix;

	float StereoIPD = 0.f;
	for (int i = 0, face_i = 0; i < TARGETS_NUM; ++i)
	{
		for (face_i = 0; face_i < FACES_NUM; ++face_i)
		{
			face = static_cast<ECubeFace>(face_i);
			Location = CaptureComponent->GetComponentToWorld().GetTranslation();
			CaptureSize = FIntPoint(Targets[i]->GetSurfaceWidth(), Targets[i]->GetSurfaceHeight());

			BuildProjectionMatrix(CaptureSize, ECameraProjectionMode::Perspective, FOV, 1.0f, ProjectionMatrix);

			if (i == LEFT_TEXTURE)
				StereoIPD = CaptureComponent->IPD * -0.5f;
			else
				StereoIPD = CaptureComponent->IPD * 0.5f;

			SceneRenderer = CreateSceneRendererForSceneCapture
			(
				this,
				CaptureComponent,
				Targets[i]->GameThread_GetRenderTargetResource(),
				CaptureSize,
				GetBasisFromCubeFace(face),
				Location,
				ProjectionMatrix
			);

			FTextureRenderTargetCubeResource* TextureRenderTarget = static_cast<FTextureRenderTargetCubeResource*>(Targets[i]->GameThread_GetRenderTargetResource());
			FString EventName;
			CaptureComponent->GetOwner()->GetFName().ToString(EventName);
			
			ENQUEUE_RENDER_COMMAND(CaptureCommand)
			([SceneRenderer, TextureRenderTarget, EventName, face](FRHICommandListImmediate& RHICmdList)
			{
				UpdateSceneCaptureContent_RenderThread(RHICmdList, SceneRenderer, TextureRenderTarget, TextureRenderTarget, EventName, FResolveParams(FResolveRect(), face));
			});
		}
	}

	const FTextureRenderTargetCubeResource* const LeftEye = static_cast<FTextureRenderTargetCubeResource*>(CaptureComponent->TextureTargetLeft->GameThread_GetRenderTargetResource());
	const FTextureRenderTargetCubeResource* const RightEye = static_cast<FTextureRenderTargetCubeResource*>(CaptureComponent->TextureTargetRight->GameThread_GetRenderTargetResource());
	FTextureRenderTargetResource* const RenderTarget = CaptureComponent->TextureTargetODS->GameThread_GetRenderTargetResource();
	const ERHIFeatureLevel::Type InFeatureLevel = FeatureLevel;

	ENQUEUE_RENDER_COMMAND(ODSCaptureCommand)
	([LeftEye, RightEye, RenderTarget, InFeatureLevel](FRHICommandListImmediate& RHICmdList)
	{
		StereoCapture_RenderThread(RHICmdList, LeftEye, RightEye, RenderTarget, InFeatureLevel);
	});

	SaveRenderTextureToFile
	(
		RenderTarget,
		FIntPoint(CaptureComponent->TextureTargetODS->GetSurfaceWidth(), CaptureComponent->TextureTargetODS->GetSurfaceHeight()),
		CaptureComponent->CaptureDirectory.Path,
		CaptureComponent->FrameIndex
	);
}