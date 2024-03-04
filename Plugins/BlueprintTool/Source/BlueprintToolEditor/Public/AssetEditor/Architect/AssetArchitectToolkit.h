#pragma once
#include "Toolkits/AssetEditorToolkit.h"
#include "Tickable.h"

class UBlueprintData;
class FBlueprintToolEditorToolkit : public FAssetEditorToolkit, public FNotifyHook, public FTickableGameObject
{
	typedef FAssetEditorToolkit Super;

public:
	~FBlueprintToolEditorToolkit();
	// FTickableGameObject Interface
	virtual bool IsTickableInEditor() const override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;

	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

	void Initialize(UBlueprintData* InTextAsset, const EToolkitMode::Type InMode, const TSharedPtr<IToolkitHost>& InToolkitHost);

	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	FORCEINLINE TSharedPtr<SGraphEditor> GetGraphEditor() const { return GraphEditor; }
	//FORCEINLINE void CallEditorUpdate() { bGraphChanged = true; }
	
	//Commands
private:
	void BindCommands();

	// Extend
	void ExtendToolbar();

	//Handle 
	void Compile();
	void Help();
	void Run();

	//Save
	virtual void SaveAsset_Execute()override;

	//Data
	virtual UBlueprintData* GetBlueprintData() const;
private:
	TSharedRef<SDockTab> SpawnByPreviewTab(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnByContentBrowserTab(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnByBlueprintViewTab(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnByPaletteTab(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnByDetailsTab(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnByPreviewSettingsTab(const FSpawnTabArgs& Args);

	TSharedRef<SGraphEditor> CreateBPGraphEditor(UEdGraph* InGraph);

	void OnSelectedBPNodesChanged(const TSet<class UObject*>& SelectionNode);

private:
	void OnGraphChanged(const FEdGraphEditAction& Action);

	void GraphChanged();

	//����ӳ��
private:
	void DeleteSelectedNodes();
	bool CanDeleteNode() const;

	void CopySelectedNodes();
	bool CanCopyNodes() const;

	void PasteNodes();
	bool CanPasteNodes() const;

	void CutSelectedNodes();
	bool CanCutNodes() const;

	void DuplicateNodes();
	bool CanDuplicateNodes() const;

	void SelectAllNodes();
	bool CanSelectAllNodes() const;

	void UndoGraphAction();
	void RedoGraphAction();

private:
	UBlueprintData* BlueprintData;									//����

private:
	TSharedPtr<FUICommandList> GraphEditorCommands;
	TSharedPtr<IDetailsView> PropertyEditor;						//ϸ�����ʵ��
	TSharedPtr<class SBlueprintNodeListPalette> ActionPalette;		//��ͼ�б�
	TSharedPtr<class SGraphEditor> GraphEditor;						//��ͼ��
	TSharedPtr<class SBlueprintPreviewViewport> PreviewViewport;	//����ӿ�

private:
	uint8 bGraphChanged : 1;

	FDelegateHandle OnGraphChangedHandle;
};