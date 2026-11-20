#pragma once
#include "HLSLExpressions.h"
/*
 * The following classes are based on the book Crafting Interpreters by Robert Nystrom, chapter 5
 * https://craftinginterpreters.com/representing-code.html
 */

class FStatement
{
public:
	virtual ~FStatement()
	{
	}

	virtual FString Print() const = 0;
};

class FInOutStatement : public FStatement
{
public:
	FInOutStatement(const FHLSLToken& InOutKeyword, const FHLSLToken& Name) : InOutKeyword(InOutKeyword), Name(Name)
	{
	}

	virtual FString Print() const override;

private:
	FHLSLToken InOutKeyword;
	FHLSLToken Name;
};

class FExpressionStatement : public FStatement
{
public:
	explicit FExpressionStatement(const TSharedPtr<FExpression>& Expression) : Expression(Expression)
	{
	}

	virtual FString Print() const override;

private:
	TSharedPtr<FExpression> Expression;
};

class FIfStatement : public FStatement
{
public:
	FIfStatement(const TSharedPtr<FExpression>& Condition, const TSharedPtr<FStatement>& ThenBranch) :
		Condition(Condition), ThenBranch(ThenBranch), ElseBranch(nullptr)
	{
	}

	FIfStatement(const TSharedPtr<FExpression>& Condition, const TSharedPtr<FStatement>& ThenBranch,
	             const TSharedPtr<FStatement>& ElseBranch) : Condition(Condition), ThenBranch(ThenBranch),
	                                                         ElseBranch(ElseBranch)
	{
	}

	virtual FString Print() const override;

private:
	TSharedPtr<FExpression> Condition;
	TSharedPtr<FStatement> ThenBranch;
	TSharedPtr<FStatement> ElseBranch;
};

class FWhileStatement : public FStatement
{
public:
	FWhileStatement(const TSharedPtr<FExpression>& Condition,
	                const TSharedPtr<FStatement>& Body) : Condition(Condition), Body(Body)
	{
	}

	virtual FString Print() const override;

private:
	TSharedPtr<FExpression> Condition;
	TSharedPtr<FStatement> Body;
};

class FReturnStatement : public FStatement
{
public:
	FReturnStatement(const FHLSLToken& Keyword, const TSharedPtr<FExpression>& Value) : Keyword(Keyword), Value(Value)
	{
	}

	virtual FString Print() const override;

private:
	FHLSLToken Keyword;
	TSharedPtr<FExpression> Value;
};

class FBlock : public FStatement
{
public:
	explicit FBlock(const TArray<TSharedPtr<FStatement>>& Statements) : Statements(Statements)
	{
	}

	virtual FString Print() const override;

private:
	TArray<TSharedPtr<FStatement>> Statements;
};

class FVariableDeclarationStatement : public FStatement
{
public:
	FVariableDeclarationStatement(const FHLSLToken& Name,
	                              const TSharedPtr<FExpression>& Initializer) : Name(Name), Initializer(Initializer)
	{
	}

	virtual FString Print() const override;

private:
	FHLSLToken Name;
	TSharedPtr<FExpression> Initializer;
};
