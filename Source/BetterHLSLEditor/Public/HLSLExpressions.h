#pragma once
#include "BetterHLSLTokenizer.h"

/*
 * The following classes are based on the book Crafting Interpreters by Robert Nystrom, chapter 5
 * https://craftinginterpreters.com/representing-code.html
 */

class FExpression
{
public:
	FExpression() = default;
	virtual ~FExpression() = default;

	virtual bool IsAssignable() const = 0;

	// Debug function to print the tree as a string representation
	virtual FString Print() const = 0;
};

class FUnary : public FExpression
{
public:
	FUnary(const FHLSLToken& Operator, const TSharedPtr<FExpression>& Child) : Operator(Operator), Child(Child)
	{
	}

	virtual FString Print() const override;

	virtual bool IsAssignable() const override { return false; }

	FHLSLToken Operator;
	TSharedPtr<FExpression> Child;
};

class FCall : public FExpression
{
public:
	FCall(const FHLSLToken& Callee, const FHLSLToken& ClosingParenthesis,
	      const TArray<TSharedPtr<FExpression>>& Arguments) : Callee(Callee), ClosingParenthesis(ClosingParenthesis),
	                                                          Arguments(Arguments)
	{
	}

	virtual FString Print() const override;

	virtual bool IsAssignable() const override { return false; }

	FHLSLToken Callee;
	FHLSLToken ClosingParenthesis;
	TArray<TSharedPtr<FExpression>> Arguments;
};

class FBinary : public FExpression
{
public:
	FBinary(const TSharedPtr<FExpression>& Left, const FHLSLToken& Operator,
	        const TSharedPtr<FExpression>& Right) : Left(Left), Operator(Operator), Right(Right)
	{
	}

	virtual FString Print() const override;

	virtual bool IsAssignable() const override { return false; }

	TSharedPtr<FExpression> Left;
	FHLSLToken Operator;
	TSharedPtr<FExpression> Right;
};

class FObjectAccess : public FExpression
{
public:
	FObjectAccess(const TSharedPtr<FExpression>& Object, const FHLSLToken& PropertyName) : Object(Object),
		PropertyName(PropertyName)
	{
	}

	virtual FString Print() const override;

	virtual bool IsAssignable() const override { return true; }

private:
	TSharedPtr<FExpression> Object;
	FHLSLToken PropertyName;
};

class FTernary : public FExpression
{
public:
	FTernary(const TSharedPtr<FExpression>& Boolean, const TSharedPtr<FExpression>& TrueExpression,
	         const TSharedPtr<FExpression>& FalseExpression) : Boolean(Boolean), TrueExpression(TrueExpression),
	                                                           FalseExpression(FalseExpression)
	{
	}

	virtual FString Print() const override;

	virtual bool IsAssignable() const override { return false; }

	TSharedPtr<FExpression> Boolean;
	TSharedPtr<FExpression> TrueExpression;
	TSharedPtr<FExpression> FalseExpression;
};

class FLiteral : public FExpression
{
public:
	explicit FLiteral(const FString& Value) : Value(Value)
	{
	}

	virtual FString Print() const override;

	virtual bool IsAssignable() const override { return false; }

	FString Value; // String representation of the literal value, as it is in the source code
};

class FGrouping : public FExpression
{
public:
	explicit FGrouping(const TSharedPtr<FExpression>& Expression) : Expression(Expression)
	{
	}

	virtual FString Print() const override;

	virtual bool IsAssignable() const override { return false; }

	TSharedPtr<FExpression> Expression;
};

class FAssign : public FExpression
{
public:
	FAssign(const TSharedPtr<FExpression>& Name, const FHLSLToken& Operator,
	        const TSharedPtr<FExpression>& Value) : Name(Name), Operator(Operator), Value(Value)
	{
	}

	virtual FString Print() const override;

	virtual bool IsAssignable() const override { return true; }

	TSharedPtr<FExpression> Name;
	FHLSLToken Operator;
	TSharedPtr<FExpression> Value;
};

class FCast : public FExpression
{
public:
	FCast(const FString& TypeName, const TSharedPtr<FExpression>& Expression) : TypeName(TypeName), Expression(Expression) {}
	
	virtual FString Print() const override;
	
	virtual bool IsAssignable() const override { return false; }
	
private:
	FString TypeName;
	TSharedPtr<FExpression> Expression;
};

class FArrayIndex : public FExpression
{
public:
	FArrayIndex(const TSharedPtr<FExpression>& Indexee, const TSharedPtr<FExpression>& Index) : Indexee(Indexee),
		Index(Index)
	{
	}

	virtual FString Print() const override;

	virtual bool IsAssignable() const override { return true; }

private:
	TSharedPtr<FExpression> Indexee;
	TSharedPtr<FExpression> Index;
};

class FVariable : public FExpression
{
public:
	explicit FVariable(const FHLSLToken& Name) : Name(Name)
	{
	}

	virtual FString Print() const override;

	virtual bool IsAssignable() const override { return true; }

	FHLSLToken Name;
};

// Something to be thrown that we can recognize
class FParseError
{
};
