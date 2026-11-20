#pragma once
#include "HLSLTextBox.h"

class SHLSLTextBox;
class UMaterialGraphNode_Custom;

class FBetterHLSLEditorWindow
{
public:
	/// Create and open a new HLSL window with a node attached
	FBetterHLSLEditorWindow();
	~FBetterHLSLEditorWindow();
	
	// Sets the node that the window is inspecting
	void SetNode(TObjectPtr<UMaterialGraphNode_Custom> Node);
	
	/// Bring the window to focus
	void Focus();
	
private:
	// These classes need access to the error array
	friend class FBetterHLSLTokenizer;
	friend class FHLSLSyntaxTree;
	friend class FBetterHLSLHighlighter;
	
	// Called when the window opens
	TSharedRef<SDockTab> Create(const FSpawnTabArgs&);
		
	void RefreshErrorList();
	static TSharedRef<ITableRow> MakeErrorListItem(TSharedPtr<FError> Error, const TSharedRef<STableViewBase>& TableViewBase);
	
	FReply ReloadHLSL()
	{
		return HLSLBox->Reload();
	}
	
	TSharedPtr<SHLSLTextBox> HLSLBox;
	
	FDelegateHandle GraphChangedHandle;
	
	TSharedPtr<SListView<TSharedPtr<FError>>> ErrorBox;
	TObjectPtr<UMaterialGraphNode_Custom> CustomNode = nullptr;
};
