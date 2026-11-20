#include "BetterHLSLEditorWindow.h"

#include "BetterHLSLEditorStyle.h"
#include "SSimpleButton.h"

#define LOCTEXT_NAMESPACE "FBetterHLSLEditorModule"

FBetterHLSLEditorWindow::FBetterHLSLEditorWindow()
{	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName(TEXT("FBetterHLSLEditorWindow")), FOnSpawnTab::CreateRaw(this, &FBetterHLSLEditorWindow::Create)).SetDisplayName(FText::FromString(TEXT("Better HLSL Editor")));
}

FBetterHLSLEditorWindow::~FBetterHLSLEditorWindow()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName(TEXT("FBetterHLSLEditorWindow")));
	GraphChangedHandle.Reset();
}

void FBetterHLSLEditorWindow::SetNode(TObjectPtr<UMaterialGraphNode_Custom> Node)
{
	check(Node)
	
	GraphChangedHandle.Reset();
	CustomNode = Node;
	if (HLSLBox)
	{
		HLSLBox->SetNode(CustomNode);
	}
}

TSharedRef<SDockTab> FBetterHLSLEditorWindow::Create(const FSpawnTabArgs&)
{
	check(CustomNode)
	
	TSharedRef<STextBlock> LineNumberBox = SNew(STextBlock).Font(FBetterHLSLEditorStyle::GetCodeFont());
	
	HLSLBox = SNew(SHLSLTextBox).CustomNode(CustomNode).LineNumberBox(LineNumberBox);
	
	ErrorBox = SNew(SListView<TSharedPtr<FError>>).OnGenerateRow_Static(&FBetterHLSLEditorWindow::MakeErrorListItem).ListItemsSource(&HLSLBox->GetHighlighter()->GetErrors());
	
	HLSLBox->GetHighlighter()->OnFinishHighlighting.BindRaw(this, &FBetterHLSLEditorWindow::RefreshErrorList);
	
	return SNew(SDockTab)
		.TabRole(NomadTab)
		[
			SNew(SSplitter).Orientation(Orient_Vertical)
			+SSplitter::Slot().Resizable(false).Value(.1f)
			[
				SNew(SBox).HAlign(HAlign_Left)
				[
					SNew(SSimpleButton)
					.Text(FText::FromString(TEXT("Refresh")))
					.Icon(FAppStyle::GetBrush("Icons.Refresh"))
					.OnClicked_Raw(this, &FBetterHLSLEditorWindow::ReloadHLSL)
				]
			]
			+SSplitter::Slot().Resizable(true)
			[
				SNew(SBorder)
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot().MaxWidth(20.0f)
					[
						LineNumberBox
					]
					+SHorizontalBox::Slot()
					[
						HLSLBox->AsShared()
					]
				]
			]
			+SSplitter::Slot().Resizable(true).Value(.3f)
			[
				ErrorBox->AsShared()
			]
		];
}

void FBetterHLSLEditorWindow::RefreshErrorList()
{
	ErrorBox->RequestListRefresh();
}

TSharedRef<ITableRow> FBetterHLSLEditorWindow::MakeErrorListItem(TSharedPtr<FError> Error, const TSharedRef<STableViewBase>& TableViewBase)
{
	return SNew(STableRow<TSharedPtr<FError>>, TableViewBase).Padding(5.0f)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot().MaxWidth(20.0f).VAlign(VAlign_Center).Padding(0.0f, 0.0f, 3.0f, 0.0f)
			[
				SNew(SImage).Image(FBetterHLSLEditorStyle::Get().GetBrush("BetterHLSLEditor.Error"))
			]
			+SHorizontalBox::Slot().VAlign(VAlign_Center)
			[
				SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("(%i,%i): %s"), Error->LineNumber + 1, Error->LineIndex, Error->Message.GetCharArray().GetData())))
			]
		];
}

void FBetterHLSLEditorWindow::Focus()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName(TEXT("FBetterHLSLEditorWindow")));
}

#undef LOCTEXT_NAMESPACE
