#include "HLSLSyntaxTree.h"

#include "BetterHLSLEditorWindow.h"

TArray<TSharedPtr<FStatement>> FHLSLSyntaxTree::Process(TArray<FHLSLToken> InTokens)
{
	Tokens = InTokens;
	Tokens.RemoveAll([](FHLSLToken Token)-> bool { return Token.Type == EHLSLTokenType::Whitespace; });
	Tokens.RemoveAll([](FHLSLToken Token)-> bool { return Token.Type == EHLSLTokenType::NewLine; });
	Tokens.RemoveAll([](FHLSLToken Token)-> bool { return Token.Type == EHLSLTokenType::Comment; });
	Current = 0;
	Scope = MakeShared<FVariableScope>();
	HLSLIndex = 0;

	TArray<TSharedPtr<FStatement>> Statements;
	while (Match({EHLSLTokenType::InKeyword, EHLSLTokenType::OutKeyword, EHLSLTokenType::InOutKeyword}))
	{
		if (Previous().Type == EHLSLTokenType::InKeyword)
		{
			Statements.Add(InStatement());
		}
		else if (Previous().Type == EHLSLTokenType::OutKeyword)
		{
			Statements.Add(OutStatement());
		}
		else
		{
			Statements.Add(InOutStatement());
		}
	}

	HLSLIndex = Peek().Range.BeginIndex;

	while (!AtEnd())
	{
		Statements.Add(Declaration());
	}

	return Statements;
}

// inoutStatement		→ "inout" TYPE IDENTIFIER ";" ;
TSharedPtr<FStatement> FHLSLSyntaxTree::InOutStatement()
{
	try
	{
		FHLSLToken InOutKeyword = Previous();

		FHLSLToken Type = Consume(EHLSLTokenType::PrimitiveType, "Expected a type after in out declaration");

		FHLSLToken Name = Consume(EHLSLTokenType::CustomIdentifier, "Expected an identifier");

		// While the book this is based on allows variable redefinitions, HLSL does not, so we need to add a check here
		if (Scope->ContainsExclusive(Name.Source))
		{
			Error(Name, FString::Format(TEXT("Variable {0} has already been defined or declared"), {Name.Source}));
		}
		else
		{
			Scope->KnownIdentifiers.Add(Name.Source);
		}

		InVariables.Add(FName(Name.Source));
		// Determine output type
		ECustomMaterialOutputType CMOT;
		if (Type.Source == TEXT("float") || Type.Source == TEXT("float1"))
		{
			CMOT = CMOT_Float1;
		}
		else if (Type.Source == TEXT("float2"))
		{
			CMOT = CMOT_Float2;
		}
		else if (Type.Source == TEXT("float3"))
		{
			CMOT = CMOT_Float3;
		}
		else if (Type.Source == TEXT("float4"))
		{
			CMOT = CMOT_Float4;
		}
		else if (Type.Source == TEXT("FMaterialAttributes"))
		{
			CMOT = CMOT_MaterialAttributes;
		}
		else
		{
			throw Error(Type, "Invalid output variable type");
		}
		OutVariables.Emplace(FName(Name.Source), CMOT);

		Consume(EHLSLTokenType::SemiColon, "Missing ';' at the end of statement");

		return MakeShared<FInOutStatement>(InOutKeyword, Name);
	}
	catch (FParseError)
	{
		// Panic mode, discard tokens until the next statement
		EHLSLTokenType TokenType = Peek().Type;
		while (TokenType != EHLSLTokenType::SemiColon && TokenType != EHLSLTokenType::EndOfFile)
		{
			Advance();
			TokenType = Peek().Type;
		}

		if (TokenType == EHLSLTokenType::SemiColon)
		{
			Advance(); // Consume the semicolon
		}
		return nullptr;
	}
}

// outStatement		→ "out" TYPE IDENTIFIER ";" ;
TSharedPtr<FStatement> FHLSLSyntaxTree::OutStatement()
{
	try
	{
		FHLSLToken InOutKeyword = Previous();

		FHLSLToken Type = Consume(EHLSLTokenType::PrimitiveType, "Expected a type after in out declaration");

		FHLSLToken Name = Consume(EHLSLTokenType::CustomIdentifier, "Expected an identifier");

		// While the book this is based on allows variable redefinitions, HLSL does not, so we need to add a check here
		if (Scope->ContainsExclusive(Name.Source))
		{
			Error(Name, FString::Format(TEXT("Variable {0} has already been defined or declared"), {Name.Source}));
		}
		else
		{
			Scope->KnownIdentifiers.Add(Name.Source);
		}

		// Determine output type
		ECustomMaterialOutputType CMOT;
		if (Type.Source == TEXT("float") || Type.Source == TEXT("float1"))
		{
			CMOT = CMOT_Float1;
		}
		else if (Type.Source == TEXT("float2"))
		{
			CMOT = CMOT_Float2;
		}
		else if (Type.Source == TEXT("float3"))
		{
			CMOT = CMOT_Float3;
		}
		else if (Type.Source == TEXT("float4"))
		{
			CMOT = CMOT_Float4;
		}
		else if (Type.Source == TEXT("FMaterialAttributes"))
		{
			CMOT = CMOT_MaterialAttributes;
		}
		else
		{
			throw Error(Type, "Invalid output variable type");
		}
		OutVariables.Emplace(FName(Name.Source), CMOT);

		Consume(EHLSLTokenType::SemiColon, "Missing ';' at the end of statement");

		return MakeShared<FInOutStatement>(InOutKeyword, Name);
	}
	catch (FParseError)
	{
		// Panic mode, discard tokens until the next statement
		EHLSLTokenType TokenType = Peek().Type;
		while (TokenType != EHLSLTokenType::SemiColon && TokenType != EHLSLTokenType::EndOfFile)
		{
			Advance();
			TokenType = Peek().Type;
		}

		if (TokenType == EHLSLTokenType::SemiColon)
		{
			Advance(); // Consume the semicolon
		}
		return nullptr;
	}
}

// inStatement		→ "in" TYPE? IDENTIFIER ";" ;
TSharedPtr<FStatement> FHLSLSyntaxTree::InStatement()
{
	try
	{
		FHLSLToken InOutKeyword = Previous();

		Match(EHLSLTokenType::PrimitiveType);

		FHLSLToken Name = Consume(EHLSLTokenType::CustomIdentifier, "Expected an identifier");

		// While the book this is based on allows variable redefinitions, HLSL does not, so we need to add a check here
		if (Scope->ContainsExclusive(Name.Source))
		{
			Error(Name, FString::Format(TEXT("Variable {0} has already been defined or declared"), {Name.Source}));
		}
		else
		{
			Scope->KnownIdentifiers.Add(Name.Source);
		}

		InVariables.Add(FName(Name.Source));

		Consume(EHLSLTokenType::SemiColon, "Missing ';' at the end of statement");

		return MakeShared<FInOutStatement>(InOutKeyword, Name);
	}
	catch (FParseError)
	{
		// Panic mode, discard tokens until the next statement
		EHLSLTokenType TokenType = Peek().Type;
		while (TokenType != EHLSLTokenType::SemiColon && TokenType != EHLSLTokenType::EndOfFile)
		{
			Advance();
			TokenType = Peek().Type;
		}

		if (TokenType == EHLSLTokenType::SemiColon)
		{
			Advance(); // Consume the semicolon
		}
		return nullptr;
	}
}

// Apart from the grammar rule, this is also where we attempt to resynchronize in the case of a parse error
// declaration			→ variableDeclaration | statement ;
TSharedPtr<FStatement> FHLSLSyntaxTree::Declaration()
{
	try
	{
		if (Match(EHLSLTokenType::PrimitiveType))
		{
			return VariableDeclaration();
		}

		return Statement();
	}
	catch (FParseError)
	{
		// Panic mode, discard tokens until the next statement
		EHLSLTokenType TokenType = Peek().Type;
		while (TokenType != EHLSLTokenType::SemiColon && TokenType != EHLSLTokenType::EndOfFile)
		{
			Advance();
			TokenType = Peek().Type;
		}

		if (TokenType == EHLSLTokenType::SemiColon)
		{
			Advance(); // Consume the semicolon
		}
		return nullptr;
	}
}

// statement			→ expressionStatement | block | ifStatement | whileStatement | forStatement | returnStatement ;
TSharedPtr<FStatement> FHLSLSyntaxTree::Statement()
{
	if (Match(EHLSLTokenType::LeftCurlyBrace))
	{
		return Block();
	}

	if (Match(EHLSLTokenType::If))
	{
		return IfStatement();
	}

	if (Match(EHLSLTokenType::While))
	{
		return WhileStatement();
	}

	if (Match(EHLSLTokenType::For))
	{
		return ForStatement();
	}

	if (Match(EHLSLTokenType::Return))
	{
		return ReturnStatement();
	}

	return ExpressionStatement();
}

TSharedPtr<FStatement> FHLSLSyntaxTree::Block()
{
	TArray<TSharedPtr<FStatement>> Statements = {};

	Scope = MakeShared<FVariableScope>(Scope);

	while (!Check(EHLSLTokenType::RightCurlyBrace) && !AtEnd())
	{
		Statements.Add(Declaration());
	}

	Consume(EHLSLTokenType::RightCurlyBrace, "Expected '}'");

	Scope = Scope->Enclosing;

	return MakeShared<FBlock>(Statements);
}

// ifStatement			→ "if" "(" expression ")" statement ( "else" statement )? ;
TSharedPtr<FStatement> FHLSLSyntaxTree::IfStatement()
{
	Consume(EHLSLTokenType::LeftParenthesis, "Expected '(' after 'if'");
	TSharedPtr<FExpression> Condition = Expression();
	Consume(EHLSLTokenType::RightParenthesis, "Expected ')' after condition");

	TSharedPtr<FStatement> ThenBranch = Statement();
	TSharedPtr<FStatement> ElseBranch = nullptr;
	if (Match(EHLSLTokenType::Else))
	{
		ElseBranch = Statement();
	}

	return MakeShared<FIfStatement>(Condition, ThenBranch, ElseBranch);
}

// whileStatement		→ "while" "(" expression ")" statement ;
TSharedPtr<FStatement> FHLSLSyntaxTree::WhileStatement()
{
	Consume(EHLSLTokenType::LeftParenthesis, "Expected '(' after 'while'");
	TSharedPtr<FExpression> Condition = Expression();
	Consume(EHLSLTokenType::RightParenthesis, "Expected ')' after condition");
	TSharedPtr<FStatement> Body = Statement();
	return MakeShared<FWhileStatement>(Condition, Body);
}

// forStatement		→ "for" "(" ( variableDeclaration  | expressionStatement | ";" ) expression? ";" expression? ")" statement ;
TSharedPtr<FStatement> FHLSLSyntaxTree::ForStatement()
{
	// This method desugars the for-loop into an equivalent while-loop representation
	// This is to reduce the amount of node types and also a nice example of the technique
	// It would also reduce the amount of work a potential compiler/interpreter needs to do since it would only need to parse a while-statement, though this is irrelevant in this case.

	Consume(EHLSLTokenType::LeftParenthesis, "Expected '(' after 'for'");
	TSharedPtr<FStatement> Initializer;
	if (Match(EHLSLTokenType::SemiColon))
	{
		Initializer = nullptr;
	}
	else if (Match(EHLSLTokenType::PrimitiveType))
	{
		Initializer = VariableDeclaration();
	}
	else
	{
		Initializer = ExpressionStatement();
	}

	TSharedPtr<FExpression> Condition = nullptr;
	if (!Check(EHLSLTokenType::SemiColon))
	{
		Condition = Expression();
	}
	Consume(EHLSLTokenType::SemiColon, "Expected a ';' after for-loop condition");

	TSharedPtr<FExpression> Increment = nullptr;
	if (!Check(EHLSLTokenType::RightParenthesis))
	{
		Increment = Expression();
	}
	Consume(EHLSLTokenType::RightParenthesis, "Expected a ')'");

	TSharedPtr<FStatement> Body = Statement();

	// Let the desugaring begin

	// Append the increment after the body as an expression statement
	if (Increment)
	{
		Body = MakeShared<FBlock>(TArray<TSharedPtr<FStatement>>{Body, MakeShared<FExpressionStatement>(Increment)});
	}

	// If there is no loop condition, assume true to make an infinite loop
	if (!Condition) Condition = MakeShared<FLiteral>("true");
	Body = MakeShared<FWhileStatement>(Condition, Body);

	// If there is an initializer prepend it into a block before the while loop
	if (Initializer)
	{
		Body = MakeShared<FBlock>(TArray{Initializer, Body});
	}

	return Body;
}

// returnStatement		→ "return" expression? ";" ;
TSharedPtr<FStatement> FHLSLSyntaxTree::ReturnStatement()
{
	FHLSLToken Keyword = Previous();
	TSharedPtr<FExpression> Value = nullptr;

	if (!Check(EHLSLTokenType::SemiColon))
	{
		Value = Expression();
	}

	Consume(EHLSLTokenType::SemiColon, "Expected a ';' after 'return'");

	return MakeShared<FReturnStatement>(Keyword, Value);
}

// expressionStatement	→ expression ";" ;
TSharedPtr<FStatement> FHLSLSyntaxTree::ExpressionStatement()
{
	TSharedPtr<FExpression> Expr = Expression();
	Consume(EHLSLTokenType::SemiColon, "Missing ';' at the end of statement");
	return MakeShared<FExpressionStatement>(Expr);
}

// variableDeclaration	→ TYPE IDENTIFIER ( "=" expression )? ";" ;
TSharedPtr<FStatement> FHLSLSyntaxTree::VariableDeclaration()
{
	FHLSLToken Name = Consume(EHLSLTokenType::CustomIdentifier, "Expected a variable identifier");

	TSharedPtr<FExpression> Initializer = nullptr;

	// Check for same line initialization
	if (Match(EHLSLTokenType::Assignment))
	{
		Initializer = Expression();
	}

	Consume(EHLSLTokenType::SemiColon, "Missing ';' at the end of statement");

	// While the book this is based on allows variable redefinitions, HLSL does not, so we need to add a check here
	if (Scope->ContainsExclusive(Name.Source))
	{
		Error(Name, FString::Format(TEXT("Variable {0} has already been defined or declared"), {Name.Source}));
	}
	else
	{
		Scope->KnownIdentifiers.Add(Name.Source);
	}

	return MakeShared<FVariableDeclarationStatement>(Name, Initializer);
}

// expression     → assignment ( "," assignment )* ;
TSharedPtr<FExpression> FHLSLSyntaxTree::Expression()
{
	TSharedPtr<FExpression> Expression = Assignment();

	while (Match(EHLSLTokenType::Comma))
	{
		FHLSLToken Operator = Previous();
		TSharedPtr<FExpression> RightSide = Assignment();
		Expression = MakeShared<FBinary>(Expression, Operator, RightSide);
	}

	return Expression;
}

// assignment		→ IDENTIFIER ( "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "<<=" | ">>=" | "&=" | "^=" | "|=" ) assignment | ternary ;
TSharedPtr<FExpression> FHLSLSyntaxTree::Assignment()
{
	TSharedPtr<FExpression> Expr = Ternary();
	if (Match({
		EHLSLTokenType::Assignment, EHLSLTokenType::PlusEquals, EHLSLTokenType::MinusEquals,
		EHLSLTokenType::AsteriskEquals, EHLSLTokenType::DivisionEquals, EHLSLTokenType::ModuloEquals,
		EHLSLTokenType::ShiftLeftEquals, EHLSLTokenType::ShiftRightEquals, EHLSLTokenType::BitwiseAndEquals,
		EHLSLTokenType::XOREquals, EHLSLTokenType::BitwiseOrEquals
	}))
	{
		FHLSLToken Equals = Previous();
		TSharedPtr<FExpression> Value = Assignment();

		if (Expr->IsAssignable())
		{
			return MakeShared<FAssign>(Expr, Equals, Value);
		}

		Error(Equals, "Invalid assignment target.");
	}

	return Expr;
}

// ternary		   → logicalOR ( "?" expression ":" assignment ) ;
TSharedPtr<FExpression> FHLSLSyntaxTree::Ternary()
{
	TSharedPtr<FExpression> Expr = LogicalOR();

	if (Match(EHLSLTokenType::QuestionMark))
	{
		TSharedPtr<FExpression> TrueExpression = Expression();
		Consume(EHLSLTokenType::Colon, TEXT("Missing ':' on ternary operator"));
		TSharedPtr<FExpression> FalseExpression = Assignment();
		Expr = MakeShared<FTernary>(Expr, TrueExpression, FalseExpression);
	}

	return Expr;
}

// logicalOR	   → logicalAND ( "||" logicalAND )* ;
TSharedPtr<FExpression> FHLSLSyntaxTree::LogicalOR()
{
	TSharedPtr<FExpression> Expression = LogicalAND();

	while (Match(EHLSLTokenType::BooleanOr))
	{
		FHLSLToken Operator = Previous();
		TSharedPtr<FExpression> RightSide = LogicalAND();
		Expression = MakeShared<FBinary>(Expression, Operator, RightSide);
	}

	return Expression;
}

// logicalAND	   → binaryOR ( "&&" binaryOR )* ;
TSharedPtr<FExpression> FHLSLSyntaxTree::LogicalAND()
{
	TSharedPtr<FExpression> Expression = BinaryOR();

	while (Match(EHLSLTokenType::BooleanAnd))
	{
		FHLSLToken Operator = Previous();
		TSharedPtr<FExpression> RightSide = BinaryOR();
		Expression = MakeShared<FBinary>(Expression, Operator, RightSide);
	}

	return Expression;
}

// binaryOR	   → binaryXOR ( "|" binaryXOR )* ;
TSharedPtr<FExpression> FHLSLSyntaxTree::BinaryOR()
{
	TSharedPtr<FExpression> Expression = BinaryXOR();

	while (Match(EHLSLTokenType::BitwiseOr))
	{
		FHLSLToken Operator = Previous();
		TSharedPtr<FExpression> RightSide = BinaryXOR();
		Expression = MakeShared<FBinary>(Expression, Operator, RightSide);
	}

	return Expression;
}

// binaryXOR	   → binaryAND ( "^" binaryAND )* ;
TSharedPtr<FExpression> FHLSLSyntaxTree::BinaryXOR()
{
	TSharedPtr<FExpression> Expression = BinaryAND();

	while (Match(EHLSLTokenType::XOR))
	{
		FHLSLToken Operator = Previous();
		TSharedPtr<FExpression> RightSide = BinaryAND();
		Expression = MakeShared<FBinary>(Expression, Operator, RightSide);
	}

	return Expression;
}

// binaryAND	   → equality ( "&" equality )* ;
TSharedPtr<FExpression> FHLSLSyntaxTree::BinaryAND()
{
	TSharedPtr<FExpression> Expression = Equality();

	while (Match(EHLSLTokenType::BitwiseAnd))
	{
		FHLSLToken Operator = Previous();
		TSharedPtr<FExpression> RightSide = Equality();
		Expression = MakeShared<FBinary>(Expression, Operator, RightSide);
	}

	return Expression;
}

// equality       → comparison ( ( "!=" | "==" ) comparison )* ;
TSharedPtr<FExpression> FHLSLSyntaxTree::Equality()
{
	TSharedPtr<FExpression> Expression = Comparison();

	while (Match({EHLSLTokenType::NotEquals, EHLSLTokenType::Equals}))
	{
		FHLSLToken Operator = Previous();
		TSharedPtr<FExpression> RightSide = Comparison();
		Expression = MakeShared<FBinary>(Expression, Operator, RightSide);
	}

	return Expression;
}

// comparison		→ bitshift ( ( ">" | ">=" | "<" | "<=" ) bitshift )* ;
TSharedPtr<FExpression> FHLSLSyntaxTree::Comparison()
{
	TSharedPtr<FExpression> Expression = Bitshift();

	while (Match({
		EHLSLTokenType::GreaterThan, EHLSLTokenType::GreaterThanEquals, EHLSLTokenType::LessThan,
		EHLSLTokenType::LessThanEquals
	}))
	{
		FHLSLToken Operator = Previous();
		TSharedPtr<FExpression> RightSide = Bitshift();
		Expression = MakeShared<FBinary>(Expression, Operator, RightSide);
	}

	return Expression;
}

// bitshift		→ term ( ( "<<" | ">>" ) term )* ;
TSharedPtr<FExpression> FHLSLSyntaxTree::Bitshift()
{
	TSharedPtr<FExpression> Expression = Term();

	while (Match({EHLSLTokenType::ShiftLeft, EHLSLTokenType::ShiftRight}))
	{
		FHLSLToken Operator = Previous();
		TSharedPtr<FExpression> RightSide = Term();
		Expression = MakeShared<FBinary>(Expression, Operator, RightSide);
	}

	return Expression;
}

// term           → factor ( ( "-" | "+" ) factor )* ;
TSharedPtr<FExpression> FHLSLSyntaxTree::Term()
{
	TSharedPtr<FExpression> Expression = Factor();

	while (Match({EHLSLTokenType::Minus, EHLSLTokenType::Plus}))
	{
		FHLSLToken Operator = Previous();
		TSharedPtr<FExpression> RightSide = Factor();
		Expression = MakeShared<FBinary>(Expression, Operator, RightSide);
	}

	return Expression;
}

// factor         → unary ( ( "/" | "*" | "%" ) unary )* ;
TSharedPtr<FExpression> FHLSLSyntaxTree::Factor()
{
	TSharedPtr<FExpression> Expression = Unary();

	while (Match({EHLSLTokenType::Division, EHLSLTokenType::Asterisk, EHLSLTokenType::Modulo}))
	{
		FHLSLToken Operator = Previous();
		TSharedPtr<FExpression> RightSide = Unary();
		Expression = MakeShared<FBinary>(Expression, Operator, RightSide);
	}

	return Expression;
}

// unary			→ ( "!" | "-" | "+" | "~" | "++" | "--" | "(" TYPE ")" ) unary | propertyAccess ;
TSharedPtr<FExpression> FHLSLSyntaxTree::Unary()
{
	if (Match({
		EHLSLTokenType::BooleanNot, EHLSLTokenType::Minus, EHLSLTokenType::Plus, EHLSLTokenType::BitwiseNot,
		EHLSLTokenType::PrePostFixOperator
	}))
	{
		FHLSLToken Operator = Previous();
		return MakeShared<FUnary>(Operator, Unary());
	}
	
	if (Check(EHLSLTokenType::LeftParenthesis) && Peek(1).Type == EHLSLTokenType::PrimitiveType)
	{
		Advance();	// Consume the parenthesis
		FHLSLToken TypeName = Advance();	// Consume the typename
		Consume(EHLSLTokenType::RightParenthesis, "Expected right parenthesis after C-style cast");
		return MakeShared<FCast>(TypeName.Source, Unary());
	}

	return PropertyAccess();
}

// propertyAccess	→ call ( "." IDENTIFIER | ( "++" | "--" ) | "[" NUMBER "]" )* ;
TSharedPtr<FExpression> FHLSLSyntaxTree::PropertyAccess()
{
	TSharedPtr<FExpression> Object = Call();
	while (true)
	{
		if (Match(EHLSLTokenType::Dot))
		{
			FHLSLToken PropertyName = Consume(EHLSLTokenType::CustomIdentifier, "Expected property name after '.'");
			Object = MakeShared<FObjectAccess>(Object, PropertyName);
		}
		else if (Match(EHLSLTokenType::PrePostFixOperator))
		{
			Object = MakeShared<FUnary>(Previous(), Object);
		}
		else if (Match(EHLSLTokenType::LeftSquareBracket))
		{
			TSharedPtr<FExpression> Index = Expression();
			Consume(EHLSLTokenType::RightSquareBracket, "Expected ']' after array index");
			Object = MakeShared<FArrayIndex>(Object, Index);
		}
		else
		{
			break;
		}
	}

	return Object;
}

// call			→ ( ( FUNCTION | TYPE ) "(" arguments? ")" ) | primary ;
TSharedPtr<FExpression> FHLSLSyntaxTree::Call()
{
	if (Match({EHLSLTokenType::Function, EHLSLTokenType::PrimitiveType}))
	{
		Consume(EHLSLTokenType::LeftParenthesis, "Expected '(' after function name");
		return FinishCall(Previous());
	}

	return Primary();
}

TSharedPtr<FExpression> FHLSLSyntaxTree::FinishCall(FHLSLToken Callee)
{
	TArray<TSharedPtr<FExpression>> Arguments = {};

	if (!Check(EHLSLTokenType::RightParenthesis))
	{
		do
		{
			Arguments.Add(Expression());
		}
		while (Match(EHLSLTokenType::Comma));
	}

	// Used as a location for reporting errors in the function arguments
	FHLSLToken RightParenthesis = Consume(EHLSLTokenType::RightParenthesis, "Expected ')' after function arguments.");
	return MakeShared<FCall>(Callee, RightParenthesis, Arguments);
}


// primary        → NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" | IDENTIFIER ;
TSharedPtr<FExpression> FHLSLSyntaxTree::Primary()
{
	if (Match({
		EHLSLTokenType::True, EHLSLTokenType::False, EHLSLTokenType::Null, EHLSLTokenType::Number,
		EHLSLTokenType::String
	}))
	{
		return MakeShared<FLiteral>(Previous().Source);
	}

	if (Match(EHLSLTokenType::CustomIdentifier))
	{
		if (!Scope->ContainsInclusive(Previous().Source))
		{
			throw Error(Previous(), "Unknown identifier");
		}

		return MakeShared<FVariable>(Previous());
	}

	if (Match(EHLSLTokenType::LeftParenthesis))
	{
		TSharedPtr<FExpression> Expr = Expression();
		Consume(EHLSLTokenType::RightParenthesis, TEXT("Expected ')' after expression"));
		return MakeShared<FGrouping>(Expr);
	}

	throw Error(Peek(), TEXT("Expected an expression."));
}

bool FHLSLSyntaxTree::Match(const TArray<EHLSLTokenType>& Types)
{
	for (EHLSLTokenType Type : Types)
	{
		if (Check(Type))
		{
			Advance(); // Consume the token
			return true;
		}
	}

	return false;
}

bool FHLSLSyntaxTree::Match(EHLSLTokenType Type)
{
	if (Check(Type))
	{
		Advance();
		return true;
	}

	return false;
}

bool FHLSLSyntaxTree::Check(EHLSLTokenType Type)
{
	if (AtEnd()) return false;
	return Peek().Type == Type;
}

FHLSLToken FHLSLSyntaxTree::Advance()
{
	if (!AtEnd()) Current++;
	return Previous();
}

FHLSLToken FHLSLSyntaxTree::Consume(EHLSLTokenType Type, FString ErrorMessage)
{
	if (Check(Type)) return Advance();

	throw Error(Peek(), ErrorMessage);
}

bool FHLSLSyntaxTree::AtEnd()
{
	return Peek().Type == EHLSLTokenType::EndOfFile;
}

FHLSLToken FHLSLSyntaxTree::Peek(const int32 Offset)
{
	check(Current + Offset < Tokens.Num())
	return Tokens[Current + Offset];
}

FHLSLToken FHLSLSyntaxTree::Previous()
{
	check(Current != 0);
	return Tokens[Current - 1];
}

FParseError FHLSLSyntaxTree::Error(FHLSLToken Token, FString Message)
{
	Errors.Add(MakeShared<FError>(Message, Token.RangeInLine, Token.LineNumber, Token.RangeInLine.BeginIndex));

	return FParseError();
}
