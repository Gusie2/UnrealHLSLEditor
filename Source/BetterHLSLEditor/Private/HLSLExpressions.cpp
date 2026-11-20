#include "HLSLExpressions.h"

FString FUnary::Print() const
{
	TStringBuilder<128> Builder;
	Builder.Append("(");
	Builder.Append(Operator.Source);
	Builder.Append(" ");
	Builder.Append(Child->Print());
	Builder.Append(")");
	return Builder.ToString();
}

FString FCall::Print() const
{
	TStringBuilder<128> Builder;
	Builder.Append("(");
	Builder.Append(Callee.Source);
	Builder.Append(" ");
	for (const auto& Argument : Arguments)
	{
		Builder.Append(Argument->Print());
		Builder.Append(" ");
	}
	Builder.Append(")");
	return Builder.ToString();
}

FString FBinary::Print() const
{
	TStringBuilder<128> Builder;
	Builder.Append("(");
	Builder.Append(Operator.Source);
	Builder.Append(" ");
	Builder.Append(Left->Print());
	Builder.Append(" ");
	Builder.Append(Right->Print());
	Builder.Append(")");
	return Builder.ToString();
}

FString FObjectAccess::Print() const
{
	TStringBuilder<128> Builder;
	Builder.Append(Object->Print());
	Builder.Append(".");
	Builder.Append(PropertyName.Source);
	return Builder.ToString();
}

FString FTernary::Print() const
{
	TStringBuilder<128> Builder;
	Builder.Append("(? ");
	Builder.Append(Boolean->Print());
	Builder.Append(" ");
	Builder.Append(TrueExpression->Print());
	Builder.Append(" ");
	Builder.Append(FalseExpression->Print());
	Builder.Append(")");
	return Builder.ToString();
}

FString FLiteral::Print() const
{
	return Value;
}

FString FGrouping::Print() const
{
	TStringBuilder<128> Builder;
	Builder.Append("(group ");
	Builder.Append(Expression->Print());
	Builder.Append(")");
	return Builder.ToString();
}

FString FAssign::Print() const
{
	TStringBuilder<128> Builder;
	Builder.Append("(");
	Builder.Append(Operator.Source);
	Builder.Append(" ");
	Builder.Append(Name->Print());
	Builder.Append(" ");
	Builder.Append(Value->Print());
	Builder.Append(")");
	return Builder.ToString();
}

FString FCast::Print() const
{
	TStringBuilder<128> Builder;
	Builder.Append("(Cast ");
	Builder.Append(Expression->Print());
	Builder.Append(" to ");
	Builder.Append(TypeName);
	Builder.Append(")");
	return Builder.ToString();
}

FString FArrayIndex::Print() const
{
	TStringBuilder<128> Builder;
	Builder.Append("(");
	Builder.Append(Indexee->Print());
	Builder.Append("@");
	Builder.Append(Index->Print());
	Builder.Append(")");
	return Builder.ToString();
}

FString FVariable::Print() const
{
	return Name.Source;
}
