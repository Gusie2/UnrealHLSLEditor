// Copyright Epic Games, Inc. All Rights Reserved.

#include "BetterHLSLEditor.h"
#include "BetterHLSLEditorStyle.h"
#include "BetterHLSLEditorWindow.h"
#include "MaterialGraph/MaterialGraphNode_Custom.h"

#define LOCTEXT_NAMESPACE "FBetterHLSLEditorModule"

void FBetterHLSLEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FBetterHLSLEditorStyle::Initialize();
	FBetterHLSLEditorStyle::ReloadTextures();
	
	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FBetterHLSLEditorModule::RegisterMenus));
	
	Window = MakeShared<FBetterHLSLEditorWindow>();
}

void FBetterHLSLEditorModule::ShutdownModule()
{	
	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);
	
	FBetterHLSLEditorStyle::Shutdown();
}

void FBetterHLSLEditorModule::PluginButtonClicked(TObjectPtr<UMaterialGraphNode_Custom> ClickedNode)
{
	Window->SetNode(ClickedNode);
	Window->Focus();
}

void FBetterHLSLEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("GraphEditor.GraphNodeContextMenu.MaterialGraphNode_Custom");
		{
			if (!Menu)
			{
				return;
			}

			auto ToolMenuDelegate = FNewToolMenuDelegate::CreateLambda(
				[this](UToolMenu* InMenu)
				{
					auto* Context = InMenu->FindContext<UGraphNodeContextMenuContext>();

					if (!Context || !Context->Node)
					{
						return;
					}
					
					TObjectPtr<UMaterialGraphNode_Custom> ClickedNode = nullptr;
					
					// Obtain a non-const reference to the node
					for (TObjectPtr Node : Context->Graph->Nodes)
					{
						if (Node->NodeGuid == Context->Node->NodeGuid)
						{
							ClickedNode = Cast<UMaterialGraphNode_Custom>(Node);
							break;
						}
					}
					
					if (!ClickedNode)
					{
						return;
					}
					
					FToolMenuSection& NewSection = InMenu->AddSection("OpenHLSLEditorAction", INVTEXT("HLSL Editor"));
					NewSection.AddMenuEntry("OpenHLSLEditor", INVTEXT("Open HLSL Editor"),
											INVTEXT("Opens the larger HLSL editor"), FSlateIcon(),
											FUIAction(FExecuteAction::CreateLambda(
												[this, ClickedNode]()
												{
													PluginButtonClicked(ClickedNode);
												})));
				});

			Menu->AddDynamicSection("HLSLEditorSection", ToolMenuDelegate);
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBetterHLSLEditorModule, BetterHLSLEditor)