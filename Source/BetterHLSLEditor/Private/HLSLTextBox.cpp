#include "HLSLTextBox.h"

void SHLSLTextBox::Construct(const FArguments& InArgs)
{
	CustomNode = InArgs._CustomNode;
	LineNumberBox = InArgs._LineNumberBox;

	Highlighter = FBetterHLSLHighlighter::Create();

	SMultiLineEditableText::Construct(
		SMultiLineEditableText::FArguments().OnTextCommitted_Raw(this, &SHLSLTextBox::OnTextUpdated)
		                                    .OnTextChanged_Raw(this, &SHLSLTextBox::RefreshLineNumbers)
		                                    .Marshaller(Highlighter)
	);

	Reload();
}

void SHLSLTextBox::SetNode(TObjectPtr<UMaterialGraphNode_Custom> InNode)
{
	check(InNode);

	CustomNode = InNode;
	Reload();
}

FReply SHLSLTextBox::Reload()
{
	if (!CustomNode)
		return FReply::Handled();

	FString Inputs;
	FString Outputs;
	FString InOutputs;

	auto* Expression = Cast<UMaterialExpressionCustom>(CustomNode->MaterialExpression);

	// Check outputs
	for (const auto& Output : Expression->AdditionalOutputs)
	{
		// Skip empty names
		if (Output.OutputName.IsNone())
		{
			continue;
		}

		FString Type;

		switch (Output.OutputType)
		{
		case CMOT_Float1:
			Type = "float ";
			break;

		case CMOT_Float2:
			Type = "float2 ";
			break;

		case CMOT_Float3:
			Type = "float3 ";
			break;

		case CMOT_Float4:
			Type = "float4 ";
			break;

		case CMOT_MaterialAttributes:
			Type = "FMaterialAttributes ";
			break;

		default:
			continue;
		}

		if (Expression->Inputs.ContainsByPredicate([&Output](const FCustomInput& Input)-> bool
		{
			return Input.InputName == Output.OutputName;
		}))
		{
			InOutputs += "inout ";
			InOutputs += Type;
			InOutputs += Output.OutputName.ToString();
			InOutputs += ";\n";
		}
		else
		{
			Outputs += "out ";
			Outputs += Type;
			Outputs += Output.OutputName.ToString();
			Outputs += ";\n";
		}
	}

	// Add inputs
	for (const auto& Input : Expression->Inputs)
	{
		// Skip empty names
		if (Input.InputName.IsNone())
		{
			continue;
		}

		// Skip variables that are also an output
		if (Expression->AdditionalOutputs.ContainsByPredicate([&Input](const FCustomOutput& Output)-> bool
		{
			return Input.InputName == Output.OutputName;
		}))
		{
			continue;
		}

		Inputs += "in ";
		Inputs += Input.InputName.ToString();
		Inputs += ";\n";
	}

	FString Code;
	Code += Inputs;
	Code += InOutputs;
	Code += Outputs;
	Code += "\n";
	Code += CustomNode->GetHlslText().ToString();

	Code.TrimStartInline();

	SetText(FText::FromString(Code));

	RefreshLineNumbers(GetText());

	return FReply::Handled();
}

void SHLSLTextBox::RefreshLineNumbers(const FText& Text)
{
	if (LineNumberBox.IsValid())
	{
		FString Numbers;

		TArray<FTextRange> Lines;
		FTextRange::CalculateLineRangesFromString(Text.ToString(), Lines);

		for (int i = 1; i <= Lines.Num(); i++)
		{
			Numbers.Append(FString::FromInt(i));
			Numbers.AppendChar('\n');
		}

		LineNumberBox->SetText(FText::FromString(Numbers));
	}
}

void SHLSLTextBox::OnTextUpdated(const FText& Text, ETextCommit::Type CommitType) const
{
	if (!CustomNode)
		return;

	auto* Expression = Cast<UMaterialExpressionCustom>(CustomNode->MaterialExpression);
	Expression->Inputs.Empty();
	Expression->AdditionalOutputs.Empty();
	for (const auto& InVariable : Highlighter->InVariables)
	{
		Expression->Inputs.Add(FCustomInput(InVariable, FExpressionInput()));
	}
	for (const auto& OutVariable : Highlighter->OutVariables)
	{
		Expression->AdditionalOutputs.Add(OutVariable);
	}

	Expression->RebuildOutputs();
	Expression->GraphNode->ReconstructNode();

	FString TextCopy = Text.ToString();

	TextCopy.RemoveAt(0, Highlighter->HLSLIndex);

	TextCopy.TrimStartAndEndInline();

	CustomNode->OnCustomHlslTextCommitted(FText::FromString(TextCopy), CommitType);
}
