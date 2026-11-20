// Copyright Epic Games, Inc. All Rights Reserved.

#include "BetterHLSLEditorStyle.h"

#include "InteractiveGizmoManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FBetterHLSLEditorStyle::StyleInstance = nullptr;

void FBetterHLSLEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FBetterHLSLEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FBetterHLSLEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("BetterHLSLEditorStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FBetterHLSLEditorStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("BetterHLSLEditorStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("BetterHLSLEditor")->GetBaseDir() / TEXT("Resources"));

	Style->Set("BetterHLSLEditor.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));
	
	Style->Set("BetterHLSLEditor.Error", new IMAGE_BRUSH(TEXT("error-icon"), Icon20x20));
	
	auto* ErrorSquiggle = new IMAGE_BRUSH_SVG(TEXT("error-line"), Icon20x20);
	ErrorSquiggle->Tiling = ESlateBrushTileType::Horizontal;
	Style->Set("BetterHLSLEditor.ErrorSquiggle", ErrorSquiggle);
	
	return Style;
}

void FBetterHLSLEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

FString FBetterHLSLEditorStyle::GetResourceDir()
{
	return FSlateStyleRegistry::FindSlateStyle(GetStyleSetName())->GetContentRootDir();
}

FSlateFontInfo FBetterHLSLEditorStyle::GetCodeFont()
{
	TSharedPtr<FCompositeFont> Font = MakeShareable(new FCompositeFont("BetterHLSLEditor.CodeFont", GetResourceDir().Append("/JetBrainsMono.ttf"), EFontHinting::Default, EFontLoadingPolicy::LazyLoad));
	return FSlateFontInfo(Font, 10.0f);
}

const ISlateStyle& FBetterHLSLEditorStyle::Get()
{
	return *StyleInstance;
}
