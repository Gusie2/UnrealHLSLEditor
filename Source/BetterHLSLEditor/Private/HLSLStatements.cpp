#include "HLSLStatements.h"

FString FInOutStatement::Print() const
{
	TStringBuilder<128> Builder;
	Builder.Append("(");
	Builder.Append(InOutKeyword.Source);
	Builder.Append(" ");
	Builder.Append(Name.Source);
	Builder.Append(")");
	return Builder.ToString();
}

FString FExpressionStatement::Print() const
{
	return Expression->Print();
}

FString FIfStatement::Print() const
{
	TStringBuilder<256> Builder;
	Builder.Append("(if ");
	Builder.Append(Condition->Print());
	Builder.Append(" then ");
	Builder.Append(ThenBranch->Print());
	if (ElseBranch)
	{
		Builder.Append(" else ");
		Builder.Append(ElseBranch->Print());
	}
	Builder.Append(")");
	return Builder.ToString();
}

FString FWhileStatement::Print() const
{
	TStringBuilder<256> Builder;
	Builder.Append("(while ");
	Builder.Append(Condition->Print());
	Builder.Append(" do ");
	Builder.Append(Body->Print());
	Builder.Append(")");
	return Builder.ToString();
}

FString FReturnStatement::Print() const
{
	TStringBuilder<128> Builder;
	Builder.Append("(return ");
	if (Value)
	{
		Builder.Append(Value->Print());
	}
	Builder.Append(")");
	return Builder.ToString();
}

FString FBlock::Print() const
{
	TStringBuilder<256> Builder;
	Builder.Append("{");
	for (auto Statement : Statements)
	{
		if (Statement)
		{
			if (Statement != Statements[0])
			{
				Builder.Append(" -> ");
			}
			Builder.Append(Statement->Print());
		}
	}
	Builder.Append("}");
	return Builder.ToString();
}

FString FVariableDeclarationStatement::Print() const
{
	TStringBuilder<128> Builder;
	Builder.Append("(Declare ");
	Builder.Append(Name.Source);
	if (Initializer)
	{
		Builder.Append(" ");
		Builder.Append(Initializer->Print());
	}
	Builder.Append(")");
	return Builder.ToString();
}
