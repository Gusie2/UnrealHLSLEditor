#pragma once
#include "BetterHLSLTokenizer.h"
#include "Framework/Text/SyntaxHighlighterTextLayoutMarshaller.h"
#include "Materials/MaterialExpressionCustom.h"

struct FError
{
	FString Message;
	FTextRange Range;
	int32 LineNumber;
	int32 LineIndex;		// The index from the start of the line where the error occured
};

class FBetterHLSLHighlighter : public FPlainTextLayoutMarshaller
{
public:
	
	static TSharedRef<FBetterHLSLHighlighter> Create();
	
	struct FHLSLHighlighterColors
	{
		FLinearColor FlowControlColor;
		FLinearColor PrimitiveTypeColor;
		FLinearColor SemanticColor;
		FLinearColor CustomTypeColor;
		FLinearColor KeywordColor;
		FLinearColor VariableNameColor;
		FLinearColor FunctionColor;
		FLinearColor CommentColor;
		FLinearColor PreprocessorColor;
		FLinearColor NormalTextColor;
		FLinearColor StringColor;
		FLinearColor NumberColor;
		FLinearColor ErrorColor;
	};
	
	FBetterHLSLHighlighter();
	
	virtual void SetText(const FString& SourceString, FTextLayout& TargetTextLayout) override;
	virtual void GetText(FString& TargetString, const FTextLayout& SourceTextLayout) override
	{
		SourceTextLayout.GetAsText(TargetString);
	}
	virtual bool RequiresLiveUpdate() const override { return true;}
	
	TArray<TSharedPtr<FError>>& GetErrors() {return Errors;}
	
	DECLARE_DELEGATE(FOnFinishHighlighting)

	FOnFinishHighlighting OnFinishHighlighting;
protected:
	void ParseTokens(const TArray<FHLSLToken>& Tokens, FTextLayout& TargetTextLayout);
	
private:
	friend class SHLSLTextBox;	
	
	// Defines what we are currently parsing
	// Used for things that require an opening and a closing action (e.g. string literals, comments, etc.)
	enum class EParseState : uint8
	{
		None,
		LookingForTypeName,
		LookingForVariableName,
		LookingForString,
		LookingForCharacter,
	};
	
	// Defines the possible states of the custom declarations
	enum class ECustomDeclarationsState : uint8
	{
		Available,			// Indicates that new declarations are allowed
		LookingForIn,		// Indicates that we are looking for an in variable declaration
		LookingForOut,		// Indicates that we are looking for an out variable declaration
		LookingForInOut,	// Indicates that we are looking for an in-out variable declaration
		Forfeited			// Indicates that no new declarations are allowed
	};
	
	enum class ECommentState : uint8
	{
		None,
		SingleLine,
		MultiLine,
	};
	
	void ProcessTokens(const TArray<FHLSLToken>& Tokens, TArray<FTextLayout::FNewLineData>& OutLinesToAdd);
	
	TArray<FTextLineHighlight> Squiggles;
	TArray<TSharedPtr<FError>> Errors;
	
	void UpdateHighlights();
	
	TSharedPtr<FBetterHLSLTokenizer> Tokenizer;
	
	FHLSLHighlighterColors Colors;
	FSlateFontInfo Font;
	
	TArray<FName> InVariables;
	TArray<FCustomOutput> OutVariables;
	int32 HLSLIndex;
};
