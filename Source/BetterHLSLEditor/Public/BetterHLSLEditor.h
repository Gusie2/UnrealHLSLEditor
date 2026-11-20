// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class UMaterialGraphNode_Custom;

class FBetterHLSLEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/// Called when the menu option is clicked
	/// Checks if a window already exists for the clicked node
	/// If a window exists, bring it to focus
	/// If no window exists, create a new one.
	void PluginButtonClicked(TObjectPtr<UMaterialGraphNode_Custom> ClickedNode);
private:
	friend class FBetterHLSLEditorWindow;
	
	void RegisterMenus();
	
	TSharedPtr<FBetterHLSLEditorWindow> Window;
};
