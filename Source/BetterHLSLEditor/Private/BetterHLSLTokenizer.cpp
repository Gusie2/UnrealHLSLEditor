#include "BetterHLSLTokenizer.h"

#include "BetterHLSLEditorWindow.h"

TSharedRef<FBetterHLSLTokenizer> FBetterHLSLTokenizer::Create(TArray<TSharedPtr<FError>>& Errors)
{
	return MakeShareable(new FBetterHLSLTokenizer(Errors));
}

void FBetterHLSLTokenizer::Process(TArray<FHLSLToken>& OutTokens, const FString& Input)
{
	Source = Input;
	CurrentIndex = 0;
	LineIndex = 0;
	LineNumber = 0;
	StartIndex = 0;
	Tokens.Empty();

	ScanTokens();

	OutTokens = Tokens;
}

FBetterHLSLTokenizer::FBetterHLSLTokenizer(TArray<TSharedPtr<FError>>& Errors) : Errors(Errors)
{
}

bool FBetterHLSLTokenizer::AtEnd() const
{
	return CurrentIndex >= Source.Len();
}

TCHAR FBetterHLSLTokenizer::Advance()
{
	return Source[CurrentIndex++];
}

bool FBetterHLSLTokenizer::Match(const TCHAR Char)
{
	if (AtEnd()) return false;
	if (Source[CurrentIndex] != Char) return false;

	CurrentIndex++;
	return true;
}

TCHAR FBetterHLSLTokenizer::Peek(const uint32 PeekOffset)
{
	if ((CurrentIndex + static_cast<int32>(PeekOffset)) >= Source.Len()) return TEXT('\0');
	return Source[CurrentIndex + PeekOffset];
}

void FBetterHLSLTokenizer::ScanTokens()
{
	while (!AtEnd())
	{
		// Read the first character of a new token

		StartIndex = CurrentIndex;

		TCHAR Char = Advance();

		switch (Char)
		{
		case ' ':
		case '\t':
			AddToken(EHLSLTokenType::Whitespace);
			break;

		case '\r':
			Match('\n');
		case '\n':
			LineNumber++;
			AddToken(EHLSLTokenType::NewLine);
			LineIndex = CurrentIndex;
			break;


		// First we'll handle simple single or double character operators
		// Miscellaneous operators
		case '(': AddToken(EHLSLTokenType::LeftParenthesis);
			break;
		case ')': AddToken(EHLSLTokenType::RightParenthesis);
			break;
		case '{': AddToken(EHLSLTokenType::LeftCurlyBrace);
			break;
		case '}': AddToken(EHLSLTokenType::RightCurlyBrace);
			break;
		case '[': AddToken(EHLSLTokenType::LeftSquareBracket);
			break;
		case ']': AddToken(EHLSLTokenType::RightSquareBracket);
			break;
		case '.': AddToken(EHLSLTokenType::Dot);
			break;
		case ';': AddToken(EHLSLTokenType::SemiColon);
			break;
		case ',': AddToken(EHLSLTokenType::Comma);
			break;
		case '?': AddToken(EHLSLTokenType::QuestionMark);
			break;
		case ':': Match(':') ? AddToken(EHLSLTokenType::DoubleColon) : AddToken(EHLSLTokenType::Colon);
			break;

		// Mathematical operators (*, %, *=, %=)
		// We also check if the character after is a '=' to distinguish immediate assignment
		case '*': Match('=') ? AddToken(EHLSLTokenType::AsteriskEquals) : AddToken(EHLSLTokenType::Asterisk);
			break;
		case '%': Match('=') ? AddToken(EHLSLTokenType::ModuloEquals) : AddToken(EHLSLTokenType::Modulo);
			break;

		// Bitwise operators (~, &, |, ^, &=, |=, ^=)
		case '~': AddToken(EHLSLTokenType::BitwiseNot);
			break;
		case '^': Match('=') ? AddToken(EHLSLTokenType::XOREquals) : AddToken(EHLSLTokenType::XOR);
			break;

		// If there is a '=' after, it is the binary operator "!=", otherwise it is the unary operator '!'
		case '!': Match('=') ? AddToken(EHLSLTokenType::NotEquals) : AddToken(EHLSLTokenType::BooleanNot);
			break;

		// Now we'll check things that are more complex

		// We need to check for both the bitwise and logical operators
		case '&':
			if (Match('&'))
			{
				// This is the boolean operator &&
				AddToken(EHLSLTokenType::BooleanAnd);
			}
			else
			{
				// This is the bitwise operator &
				Match('=') ? AddToken(EHLSLTokenType::BitwiseAndEquals) : AddToken(EHLSLTokenType::BitwiseAnd);
			}
			break;
		case '|':
			if (Match('|'))
			{
				// This is the boolean operator ||
				AddToken(EHLSLTokenType::BooleanOr);
			}
			else
			{
				// This is the bitwise operator |
				Match('=') ? AddToken(EHLSLTokenType::BitwiseOrEquals) : AddToken(EHLSLTokenType::BitwiseOr);
			}
			break;

		// Plus and minus are both used in 4 different operators
		case '+':
			if (Match('+'))
			{
				// This is the pre-/postfix operator ++
				AddToken(EHLSLTokenType::PrePostFixOperator);
			}
			else if (Match('='))
			{
				// This is the binary operator +=
				AddToken(EHLSLTokenType::PlusEquals);
			}
			else
			{
				// The symbol is a single +
				// This can be either a unary or binary operator, so will need special attention later on
				AddToken(EHLSLTokenType::Plus);
			}
			break;
		case '-':
			if (Match('-'))
			{
				// This is the pre-/postfix operator --
				AddToken(EHLSLTokenType::PrePostFixOperator);
			}
			else if (Match('='))
			{
				// This is the binary operator -=
				AddToken(EHLSLTokenType::MinusEquals);
			}
			else
			{
				// The symbol is a single -
				// This can be either a unary or binary operator, so will need special attention later on
				AddToken(EHLSLTokenType::Minus);
			}
			break;

		// Assignment and equality operators (=, ==)
		case '=': Match('=') ? AddToken(EHLSLTokenType::Equals) : AddToken(EHLSLTokenType::Assignment);
			break;

		// Inequality and bitshift operators (<, >, <=, >=, <<, >>, <<=, >>=)
		case '<':
			if (Match('<')) // See if it is actually a bitshift operator
			{
				Match('=') ? AddToken(EHLSLTokenType::ShiftLeftEquals) : AddToken(EHLSLTokenType::ShiftLeft);
			}
			else
			{
				Match('=') ? AddToken(EHLSLTokenType::LessThanEquals) : AddToken(EHLSLTokenType::LessThan);
			}
			break;
		case '>':
			if (Match('>')) // See if it is actually a bitshift operator
			{
				Match('=') ? AddToken(EHLSLTokenType::ShiftRightEquals) : AddToken(EHLSLTokenType::ShiftRight);
			}
			else
			{
				Match('=') ? AddToken(EHLSLTokenType::GreaterThanEquals) : AddToken(EHLSLTokenType::GreaterThan);
			}
			break;

		// Division requires some extra care due to comments also using '/'
		case '/':
			if (Match('/'))
			{
				// This is a single-line comment, so we'll consume until the end of the line
				while (Peek() != '\n' && !AtEnd()) Advance();
				AddToken(EHLSLTokenType::Comment);
			}
			else if (Match('*'))
			{
				// This is a multi-line comment, so we'll consume until we encounter "*/" or the end of the string
				while (!AtEnd() && Peek() != '*' && Peek(1) != '/') Advance();
				AddToken(EHLSLTokenType::Comment);
			}
			else
			{
				// This is the mathematical division operator
				Match('=') ? AddToken(EHLSLTokenType::DivisionEquals) : AddToken(EHLSLTokenType::Division);
			}
			break;

		case '"': String();
			break;

		default:
			// Not an operator
			if (TChar<TCHAR>::IsDigit(Char))
			{
				Number();
			}
			else if (TChar<TCHAR>::IsAlpha(Char) || TChar<TCHAR>::IsUnderscore(Char))
			{
				Identifier();
			}
			else
			{
				// Unknown character
				AddToken(EHLSLTokenType::Unknown);
				Error("Unexpected character.");
			}
			break;
		}
	}

	StartIndex = CurrentIndex;
	AddToken(EHLSLTokenType::EndOfFile);
}

void FBetterHLSLTokenizer::AddToken(EHLSLTokenType Type)
{
	FHLSLToken Token;
	Token.Type = Type;
	Token.LineNumber = LineNumber;
	Token.Range = FTextRange(StartIndex, CurrentIndex);
	Token.RangeInLine = FTextRange(StartIndex - LineIndex, CurrentIndex - LineIndex);
	Token.Source = Source.Mid(StartIndex, CurrentIndex - StartIndex);
	Tokens.Add(Token);
}

void FBetterHLSLTokenizer::String()
{
	bool bEscapeNextCharacter = false;

	while ((bEscapeNextCharacter || Peek() != '"') && Peek() != '\n' && !AtEnd())
	{
		// Consume characters until the end of string or the end of the line
		TCHAR Char = Advance();

		if (bEscapeNextCharacter)
		{
			bEscapeNextCharacter = false;
		}
		else if (Char == '\\')
		{
			bEscapeNextCharacter = true;
		}
	}

	if (AtEnd() || Peek() == '\n')
	{
		Error("Missing terminating \" at the end of string.");
	}

	AddToken(EHLSLTokenType::String);
}

void FBetterHLSLTokenizer::Number()
{
	while (TChar<TCHAR>::IsDigit(Peek())) Advance();

	// Check for decimals
	if (Peek() == '.' && TChar<TCHAR>::IsDigit(Peek(1)))
	{
		Advance(); // Consume the "."

		while (TChar<TCHAR>::IsDigit(Peek())) Advance();
	}

	AddToken(EHLSLTokenType::Number);
}

void FBetterHLSLTokenizer::Identifier()
{
	while (TChar<TCHAR>::IsAlnum(Peek()) || TChar<TCHAR>::IsUnderscore(Peek())) Advance();

	FString IdentifierName = Source.Mid(StartIndex, CurrentIndex - StartIndex);

	EHLSLTokenType TokenType = EHLSLTokenType::CustomIdentifier;

	if (BoolKeywords.Contains(IdentifierName)) TokenType = EHLSLTokenType::Bool;
	else if (HalfKeywords.Contains(IdentifierName)) TokenType = EHLSLTokenType::Half;
	else if (FloatKeywords.Contains(IdentifierName)) TokenType = EHLSLTokenType::Float;
	else if (DoubleKeywords.Contains(IdentifierName)) TokenType = EHLSLTokenType::Double;
	else if (UintKeywords.Contains(IdentifierName)) TokenType = EHLSLTokenType::Uint;
	else if (IntKeywords.Contains(IdentifierName)) TokenType = EHLSLTokenType::Int;
	else if (HlslFunctions.Contains(IdentifierName)) TokenType = EHLSLTokenType::Function;
	else if (HlslTypeKeywords.Contains(IdentifierName)) TokenType = EHLSLTokenType::TypeKeyword;
	else if (HlslKeywords.Contains(IdentifierName)) TokenType = EHLSLTokenType::Keyword;
	else if (IdentifierName.Equals("in", ESearchCase::CaseSensitive)) TokenType = EHLSLTokenType::InKeyword;
	else if (IdentifierName.Equals("out", ESearchCase::CaseSensitive)) TokenType = EHLSLTokenType::OutKeyword;
	else if (IdentifierName.Equals("inout", ESearchCase::CaseSensitive)) TokenType = EHLSLTokenType::InOutKeyword;
	else if (IdentifierName.Equals("false", ESearchCase::CaseSensitive)) TokenType = EHLSLTokenType::False;
	else if (IdentifierName.Equals("true", ESearchCase::CaseSensitive)) TokenType = EHLSLTokenType::True;
	else if (IdentifierName.Equals("vector", ESearchCase::CaseSensitive)) TokenType = EHLSLTokenType::Vector;
	else if (IdentifierName.Equals("matrix", ESearchCase::CaseSensitive)) TokenType = EHLSLTokenType::Matrix;
	else if (IdentifierName.Equals("Buffer", ESearchCase::CaseSensitive)) TokenType = EHLSLTokenType::Buffer;
	else if (IdentifierName.Equals(TEXT("if"), ESearchCase::CaseSensitive))
	{
		TokenType = EHLSLTokenType::If;
	}
	else if (IdentifierName.Equals(TEXT("else"), ESearchCase::CaseSensitive))
	{
		TokenType = EHLSLTokenType::Else;
	}
	else if (IdentifierName.Equals(TEXT("for"), ESearchCase::CaseSensitive))
	{
		TokenType = EHLSLTokenType::For;
	}
	else if (IdentifierName.Equals(TEXT("while"), ESearchCase::CaseSensitive))
	{
		TokenType = EHLSLTokenType::While;
	}
	else if (IdentifierName.Equals(TEXT("do"), ESearchCase::CaseSensitive))
	{
		TokenType = EHLSLTokenType::Do;
	}
	else if (IdentifierName.Equals(TEXT("continue"), ESearchCase::CaseSensitive))
	{
		TokenType = EHLSLTokenType::Continue;
	}
	else if (IdentifierName.Equals(TEXT("goto"), ESearchCase::CaseSensitive))
	{
		TokenType = EHLSLTokenType::Goto;
	}
	else if (IdentifierName.Equals(TEXT("switch"), ESearchCase::CaseSensitive))
	{
		TokenType = EHLSLTokenType::Switch;
	}
	else if (IdentifierName.Equals(TEXT("case"), ESearchCase::CaseSensitive))
	{
		TokenType = EHLSLTokenType::Case;
	}
	else if (IdentifierName.Equals(TEXT("default"), ESearchCase::CaseSensitive))
	{
		TokenType = EHLSLTokenType::Default;
	}
	else if (IdentifierName.Equals(TEXT("break"), ESearchCase::CaseSensitive))
	{
		TokenType = EHLSLTokenType::Break;
	}
	else if (IdentifierName.Equals(TEXT("return"), ESearchCase::CaseSensitive))
	{
		TokenType = EHLSLTokenType::Return;
	}

	AddToken(TokenType);
}

void FBetterHLSLTokenizer::Error(FString Message)
{
	FError Error;
	Error.LineNumber = LineNumber;
	Error.LineIndex = StartIndex - LineIndex;
	Error.Message = Message;
	Error.Range = FTextRange(Error.LineIndex, CurrentIndex - LineIndex);
	Errors.Add(MakeShared<FError>(Message, FTextRange(StartIndex, CurrentIndex), LineNumber, StartIndex - LineIndex));
}
