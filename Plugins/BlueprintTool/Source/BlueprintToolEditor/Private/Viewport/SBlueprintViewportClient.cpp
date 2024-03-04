#include "Viewport/SBlueprintViewportClient.h"
#include "Components/SkyLightComponent.h"
#include "AdvancedPreviewScene.h"
#include "Components/DirectionalLightComponent.h"
#include "Atmosphere/AtmosphericFogComponent.h"
#include "BlueprintViewportClient.h"

void SBlueprintPreviewViewport::Construct(const FArguments& InArgs)
{
	BPEditorPtr = InArgs._BPEditorPtr;
	ObjectToEdit = InArgs._ObjectToEdit;

	{
		FAdvancedPreviewScene::ConstructionValues ViewConstructionValues;
		ViewConstructionValues.bCreatePhysicsScene = false;
		ViewConstructionValues.LightBrightness = 3;
		ViewConstructionValues.SkyBrightness = 1;

		PreviewScene = MakeShareable(new FAdvancedPreviewScene(ViewConstructionValues));
		PreviewScene->SetFloorVisibility(false);

		PreviewScene->SetFloorOffset(-100000);
	}

	Skylight = NewObject<USkyLightComponent>();
	PreviewScene->AddComponent(Skylight, FTransform::Identity);

	AtmosphericFog = NewObject<UAtmosphericFogComponent>();
	PreviewScene->AddComponent(AtmosphericFog, FTransform::Identity);

	PreviewScene->DirectionalLight->SetMobility(EComponentMobility::Movable);
	PreviewScene->DirectionalLight->CastShadows = true;
	PreviewScene->DirectionalLight->CastStaticShadows = true;
	PreviewScene->DirectionalLight->CastDynamicShadows = true;
	PreviewScene->DirectionalLight->SetIntensity(3);

	SEditorViewport::Construct(SEditorViewport::FArguments());
}

TSharedRef<FEditorViewportClient> SBlueprintPreviewViewport::MakeEditorViewportClient()
{
	EditorViewportClient = MakeShareable(new FBPTEditorViewportClient(BPEditorPtr, SharedThis(this), *PreviewScene, ObjectToEdit));
	EditorViewportClient->SetRealtime(true);	

	return EditorViewportClient.ToSharedRef();
}

