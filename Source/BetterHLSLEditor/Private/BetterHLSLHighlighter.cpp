#include "BetterHLSLHighlighter.h"

#include "BetterHLSLEditorStyle.h"
#include "BetterHLSLTokenizer.h"
#include "HLSLSyntaxTree.h"

#include "Framework/Text/SlateTextRun.h"
#include "Framework/Text/SlateTextUnderlineLineHighlighter.h"

TSharedRef<FBetterHLSLHighlighter> FBetterHLSLHighlighter::Create()
{
	return MakeShareable(new FBetterHLSLHighlighter());
}

FBetterHLSLHighlighter::FBetterHLSLHighlighter()
{
	InVariables.Empty();
	OutVariables.Empty();
	HLSLIndex = 0;

	Colors = FHLSLHighlighterColors(
		FColor(216, 160, 223, 255),
		FColor(86, 156, 214, 255),
		FColor(187, 181, 41, 255),
		FColor(77, 201, 176, 255),
		FColor(86, 156, 214, 255),
		FColor(156, 220, 254, 255),
		FColor(220, 220, 169, 255),
		FColor(87, 166, 74, 255),
		FColor(128, 128, 128, 255),
		FColor(255, 255, 255, 255),
		FColor(214, 157, 133, 255),
		FColor(181, 206, 168, 255),
		FColor(255, 0, 0, 255)
	);

	Font = FBetterHLSLEditorStyle::GetCodeFont();

	Errors.Empty();

	Tokenizer = FBetterHLSLTokenizer::Create(Errors);
}

void FBetterHLSLHighlighter::SetText(const FString& SourceString, FTextLayout& TargetTextLayout)
{
	Errors.Empty();
	TargetTextLayout.ClearLines();
	TArray<FHLSLToken> Tokens;
	Tokenizer->Process(Tokens, SourceString.Replace(TEXT("\r\n"), TEXT("\n")));
	ParseTokens(Tokens, TargetTextLayout);
}

void FBetterHLSLHighlighter::ParseTokens(const TArray<FHLSLToken>& Tokens, FTextLayout& TargetTextLayout)
{
	TArray<FTextLayout::FNewLineData> LinesToAdd;

	ProcessTokens(Tokens, LinesToAdd);

	LinesToAdd.Shrink();

	TargetTextLayout.AddLines(LinesToAdd);

	OnFinishHighlighting.ExecuteIfBound();

	FHLSLSyntaxTree Tree(Errors);

	auto Program = Tree.Process(Tokens);

	HLSLIndex = Tree.HLSLIndex;
	InVariables = Tree.InVariables;
	OutVariables = Tree.OutVariables;
	
	for (auto& Statement : Program)
	{
		if (Statement)
		{
			UE_LOG(LogTemp, Log, TEXT("Parsed Tree: %s"), *Statement->Print());
		}
	}

	UpdateHighlights();

	TargetTextLayout.SetLineHighlights(Squiggles);
}

void FBetterHLSLHighlighter::ProcessTokens(const TArray<FHLSLToken>& Tokens,
                                           TArray<FTextLayout::FNewLineData>& OutLinesToAdd)
{
	TSharedRef<FString> DisplayString = MakeShared<FString>();
	TArray<TSharedRef<IRun>> Runs;

	check(Tokens.Last().Type == EHLSLTokenType::EndOfFile)

	for (const FHLSLToken& Token : Tokens)
	{
		FTextRange DisplayRange(DisplayString->Len(), DisplayString->Len() + Token.Source.Len());

		if (Token.Type != EHLSLTokenType::NewLine)
		{
			DisplayString->Append(Token.Source);
		}
		else
		{
			// Don't include the '\n
			DisplayRange.EndIndex--;
		}

		FRunInfo RunInfo(TEXT("SyntaxHighlight.HLSL.Normal"));
		FTextBlockStyle TokenStyle = FTextBlockStyle::GetDefault();
		TokenStyle.SetFont(FBetterHLSLEditorStyle::GetCodeFont());
		TokenStyle.SetColorAndOpacity(Colors.NormalTextColor);

		switch (Token.Type)
		{
		// All of these tokens use the normal text color
		case EHLSLTokenType::False:
		case EHLSLTokenType::Null:
		case EHLSLTokenType::True:
		case EHLSLTokenType::Assignment:
		case EHLSLTokenType::BooleanNot:
		case EHLSLTokenType::GreaterThanEquals:
		case EHLSLTokenType::LessThanEquals:
		case EHLSLTokenType::NotEquals:
		case EHLSLTokenType::XOREquals:
		case EHLSLTokenType::Asterisk:
		case EHLSLTokenType::AsteriskEquals:
		case EHLSLTokenType::BitwiseAnd:
		case EHLSLTokenType::BitwiseOr:
		case EHLSLTokenType::BitwiseAndEquals:
		case EHLSLTokenType::BitwiseNot:
		case EHLSLTokenType::BitwiseOrEquals:
		case EHLSLTokenType::BooleanAnd:
		case EHLSLTokenType::BooleanOr:
		case EHLSLTokenType::Comma:
		case EHLSLTokenType::Division:
		case EHLSLTokenType::DivisionEquals:
		case EHLSLTokenType::Equals:
		case EHLSLTokenType::GreaterThan:
		case EHLSLTokenType::LessThan:
		case EHLSLTokenType::MinusEquals:
		case EHLSLTokenType::Modulo:
		case EHLSLTokenType::ModuloEquals:
		case EHLSLTokenType::PlusEquals:
		case EHLSLTokenType::ShiftLeft:
		case EHLSLTokenType::ShiftLeftEquals:
		case EHLSLTokenType::ShiftRight:
		case EHLSLTokenType::ShiftRightEquals:
		case EHLSLTokenType::XOR:
		case EHLSLTokenType::PrePostFixOperator:
		case EHLSLTokenType::LeftParenthesis:
		case EHLSLTokenType::RightParenthesis:
		case EHLSLTokenType::LeftSquareBracket:
		case EHLSLTokenType::RightSquareBracket:
		case EHLSLTokenType::LeftCurlyBrace:
		case EHLSLTokenType::RightCurlyBrace:
		case EHLSLTokenType::Dot:
		case EHLSLTokenType::SemiColon:
		case EHLSLTokenType::DoubleColon:
		case EHLSLTokenType::QuestionMark:
		case EHLSLTokenType::Colon:
		case EHLSLTokenType::Plus:
		case EHLSLTokenType::Minus:
		case EHLSLTokenType::Whitespace:
		case EHLSLTokenType::Unknown:
			break;

		case EHLSLTokenType::Number:
			RunInfo.Name = TEXT("SyntaxHighlight.HLSL.Number");
			TokenStyle.SetColorAndOpacity(Colors.NumberColor);
			break;

		case EHLSLTokenType::String:
			RunInfo.Name = TEXT("SyntaxHighlight.HLSL.String");
			TokenStyle.SetColorAndOpacity(Colors.StringColor);
			break;

		case EHLSLTokenType::Comment:
			RunInfo.Name = TEXT("SyntaxHighlight.HLSL.Comment");
			TokenStyle.SetColorAndOpacity(Colors.CommentColor);
			break;

		case EHLSLTokenType::Function:
			RunInfo.Name = TEXT("SyntaxHighlight.HLSL.Function");
			TokenStyle.SetColorAndOpacity(Colors.FunctionColor);
			break;

		case EHLSLTokenType::If:
		case EHLSLTokenType::Else:
		case EHLSLTokenType::For:
		case EHLSLTokenType::While:
		case EHLSLTokenType::Do:
		case EHLSLTokenType::Continue:
		case EHLSLTokenType::Goto:
		case EHLSLTokenType::Switch:
		case EHLSLTokenType::Case:
		case EHLSLTokenType::Default:
		case EHLSLTokenType::Break:
		case EHLSLTokenType::Return:
			RunInfo.Name = TEXT("SyntaxHighlight.HLSL.FlowControl");
			TokenStyle.SetColorAndOpacity(Colors.FlowControlColor);
			break;

		case EHLSLTokenType::PrimitiveType:
			RunInfo.Name = TEXT("SyntaxHighlight.HLSL.PrimitiveType");
			TokenStyle.SetColorAndOpacity(Colors.PrimitiveTypeColor);
			break;

		case EHLSLTokenType::Keyword:
		case EHLSLTokenType::TypeKeyword:
		case EHLSLTokenType::InKeyword:
		case EHLSLTokenType::OutKeyword:
		case EHLSLTokenType::InOutKeyword:
			RunInfo.Name = TEXT("SyntaxHighlight.HLSL.Keyword");
			TokenStyle.SetColorAndOpacity(Colors.KeywordColor);
			break;

		case EHLSLTokenType::CustomIdentifier:
			RunInfo.Name = TEXT("SyntaxHighlight.HLSL.CustomIdentifier");
			TokenStyle.SetColorAndOpacity(Colors.VariableNameColor);
			break;

		case EHLSLTokenType::NewLine:
			Runs.Add(FSlateTextRun::Create(RunInfo, DisplayString, TokenStyle, DisplayRange));

			OutLinesToAdd.Emplace(DisplayString, Runs);
			DisplayString = MakeShared<FString>();
			Runs.Empty();
			continue;

		case EHLSLTokenType::EndOfFile:
			if (Runs.IsEmpty())
			{
				// Add a dummy run, so we can edit the last line
				DisplayString = MakeShared<FString>();
				Runs.Add(FSlateTextRun::Create(RunInfo, DisplayString, TokenStyle, FTextRange(0, 0)));
			}

			OutLinesToAdd.Emplace(DisplayString, Runs);
			return;
		}

		Runs.Add(FSlateTextRun::Create(RunInfo, DisplayString, TokenStyle, DisplayRange));
	}
}

void FBetterHLSLHighlighter::UpdateHighlights()
{
	Squiggles.Empty();
	FTextBlockStyle UnderlineStyle = FTextBlockStyle::GetDefault();
	UnderlineStyle.SetUnderlineBrush(*FAppStyle::Get().GetDefaultBrush());

	for (auto& Error : Errors)
	{
		Squiggles.Add(FTextLineHighlight(
			Error->LineNumber,
			Error->Range,
			FSlateTextUnderlineLineHighlighter::DefaultZIndex,
			FSlateTextUnderlineLineHighlighter::Create(UnderlineStyle.UnderlineBrush, UnderlineStyle.Font,
			                                           FLinearColor::Red, UnderlineStyle.ShadowOffset,
			                                           UnderlineStyle.ShadowColorAndOpacity)
		));
	}
}
