#pragma once
#include "BetterHLSLTokenizer.h"
#include "HLSLExpressions.h"
#include "HLSLStatements.h"
#include "Materials/MaterialExpressionCustom.h"

/*
HLSL Operators without support

C-style Cast Operator 	(type)
*/

/*
Grammar rules for statements

program				→ ( inStatement | outStatement | inoutStatement )* declaration* EOF ;
inoutStatement		→ "inout" TYPE IDENTIFIER ";" ;
outStatement		→ "out" TYPE IDENTIFIER ";" ;
inStatement			→ "in" TYPE? IDENTIFIER ";" ;
declaration			→ variableDeclaration | statement ;
statement			→ expressionStatement | block | ifStatement | whileStatement | forStatement | returnStatement ;
block				→ "{" declaration* "}" ;
ifStatement			→ "if" "(" expression ")" statement ( "else" statement )? ;
whileStatement		→ "while" "(" expression ")" statement ;
forStatement		→ "for" "(" ( variableDeclaration  | expressionStatement | ";" ) expression? ";" expression? ")" statement ;
returnStatement		→ "return" expression? ";" ;

expressionStatement	→ expression ";" ;
variableDeclaration	→ TYPE IDENTIFIER ( "=" expression )? ";" ;
*/

/*
Grammar rules for expressions

We need a separate rule for every operator precedence level
Though HLSL does not have every operator from C++, the operators that they do share also share their precedences
Precedence levels sourced from https://en.cppreference.com/cpp/language/operator_precedence

expression		→ assignment ( "," assignment )* ;
assignment		→ IDENTIFIER ( "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "<<=" | ">>=" | "&=" | "^=" | "|=" ) assignment | ternary ;
ternary			→ logicalOR ( "?" expression ":" assignment )* ;
logicalOR		→ logicalAND ( "||" logicalAND )* ;
logicalAND		→ binaryOR ( "&&" binaryOR )* ;
binaryOR		→ binaryXOR ( "|" binaryXOR )* ;
binaryXOR		→ binaryAND ( "^" binaryAND )* ;
binaryAND		→ equality ( "&" equality )* ;
equality		→ comparison ( ( "!=" | "==" ) comparison )* ;
comparison		→ bitshift ( ( ">" | ">=" | "<" | "<=" ) bitshift )* ;
bitshift		→ term ( ( "<<" | ">>" ) term )* ;
term			→ factor ( ( "-" | "+" ) factor )* ;
factor			→ unary ( ( "/" | "*" | "%" ) unary )* ;
unary			→ ( "!" | "-" | "+" | "~" | "++" | "--" | "(" TYPE ")" ) unary | propertyAccess ;
propertyAccess	→ call ( "." IDENTIFIER | ( "++" | "--" ) | "[" NUMBER "]" )* ;
call			→ ( ( FUNCTION | TYPE ) "(" arguments? ")" ) | primary ;
primary			→ NUMBER | STRING | "true" | "false" | "(" expression ")" | IDENTIFIER ;

arguments		→ expression ( "," expression )* ;

*/

struct FVariableScope
{
	FVariableScope() = default;

	explicit FVariableScope(const TSharedPtr<FVariableScope>& Enclosing) : Enclosing(Enclosing)
	{
	}

	TSharedPtr<FVariableScope> Enclosing = nullptr;

	// Checks all scopes, including higher ones. Used when assigning or evaluating.
	bool ContainsInclusive(const FString& Identifier) const
	{
		return KnownIdentifiers.Contains(Identifier) || (Enclosing && Enclosing->ContainsInclusive(Identifier));
	}

	// Only checks the current scope, with no regard for higher scopes. Used when defining new variables
	bool ContainsExclusive(const FString& Identifier) const { return KnownIdentifiers.Contains(Identifier); }

	TArray<FString> KnownIdentifiers = {};
};

struct FError;
// Based on the book Crafting Interpreters by Robert Nystrom, chapter 6
// https://craftinginterpreters.com/parsing-expressions.html
// Uses the grammar rules from the book to parse a list of tokens into a syntax tree
class FHLSLSyntaxTree
{
public:
	explicit FHLSLSyntaxTree(TArray<TSharedPtr<FError>>& Errors) : Errors(Errors)
	{
	}

	TArray<TSharedPtr<FStatement>> Process(TArray<FHLSLToken> Tokens);

private:
	friend class FBetterHLSLHighlighter;

	// These functions correspond to each of the statement rules above
	TSharedPtr<FStatement> InOutStatement();
	TSharedPtr<FStatement> OutStatement();
	TSharedPtr<FStatement> InStatement();
	TSharedPtr<FStatement> Declaration();
	TSharedPtr<FStatement> Statement();
	TSharedPtr<FStatement> Block();
	TSharedPtr<FStatement> IfStatement();
	TSharedPtr<FStatement> WhileStatement();
	TSharedPtr<FStatement> ForStatement();
	TSharedPtr<FStatement> ReturnStatement();
	TSharedPtr<FStatement> ExpressionStatement();
	TSharedPtr<FStatement> VariableDeclaration();

	// These functions correspond to each of the expression rules above
	TSharedPtr<FExpression> Expression();
	TSharedPtr<FExpression> Assignment();
	TSharedPtr<FExpression> Ternary();
	TSharedPtr<FExpression> LogicalOR();
	TSharedPtr<FExpression> LogicalAND();
	TSharedPtr<FExpression> BinaryOR();
	TSharedPtr<FExpression> BinaryXOR();
	TSharedPtr<FExpression> BinaryAND();
	TSharedPtr<FExpression> Equality();
	TSharedPtr<FExpression> Comparison();
	TSharedPtr<FExpression> Bitshift();
	TSharedPtr<FExpression> Term();
	TSharedPtr<FExpression> Factor();
	TSharedPtr<FExpression> Unary();
	TSharedPtr<FExpression> PropertyAccess();
	TSharedPtr<FExpression> Call();
	TSharedPtr<FExpression> Primary();

	// Helper function to simplify function call parsing
	TSharedPtr<FExpression> FinishCall(FHLSLToken Callee);

	/// Looks ahead to the next token and if it matches any of the given types, consumes the token.
	/// @returns Whether the token matched
	bool Match(const TArray<EHLSLTokenType>& Types);
	bool Match(EHLSLTokenType Type);

	// Checks if the next token has the given type, never consumes that token
	bool Check(EHLSLTokenType Type);

	// Checks if we are at the end of the token string
	bool AtEnd();

	// Returns the next token, without consuming it
	FHLSLToken Peek(const int32 Offset = 0);

	// Returns the next token and consumes it
	FHLSLToken Advance();

	/// Requires the next token to be of the given type, otherwise reports an error.
	/// Also consumes the token if it is present
	/// @param Type The type of token to check for
	/// @param ErrorMessage The error message to display when the token was not found
	FHLSLToken Consume(EHLSLTokenType Type, FString ErrorMessage);

	// Looks back to the previously consumed token
	FHLSLToken Previous();

	FParseError Error(FHLSLToken Token, FString Message);

	TArray<FHLSLToken> Tokens;
	int32 Current = 0;

	TArray<FName> InVariables;
	TArray<FCustomOutput> OutVariables;
	int32 HLSLIndex = 0;

	TSharedPtr<FVariableScope> Scope;

	TArray<TSharedPtr<FError>>& Errors;
};
