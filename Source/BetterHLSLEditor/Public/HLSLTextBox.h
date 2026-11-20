#pragma once
#include "BetterHLSLHighlighter.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "MaterialGraph/MaterialGraphNode_Custom.h"

class SHLSLTextBox : public SMultiLineEditableText
{	
public:
	SLATE_BEGIN_ARGS(SHLSLTextBox)
		: _CustomNode(nullptr)
		, _LineNumberBox(nullptr)
		{}
		SLATE_ARGUMENT(TObjectPtr<UMaterialGraphNode_Custom>, CustomNode)
		SLATE_ARGUMENT(TSharedPtr<STextBlock>, LineNumberBox)
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs);
	
	void SetNode(TObjectPtr<UMaterialGraphNode_Custom> InNode);
	
	// Reloads all the data from the custom node
	FReply Reload();
	
	TSharedPtr<FBetterHLSLHighlighter> GetHighlighter() const { return Highlighter; }
	
private:
	
	void RefreshLineNumbers(const FText& Text);
	
	void OnTextUpdated(const FText& Text, ETextCommit::Type CommitType) const;
	
	TObjectPtr<UMaterialGraphNode_Custom> CustomNode;
	
	TSharedPtr<STextBlock> LineNumberBox;
	TSharedPtr<FBetterHLSLHighlighter> Highlighter;
};
