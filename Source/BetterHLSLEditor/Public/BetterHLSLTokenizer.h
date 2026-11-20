#pragma once

#pragma region HLSL Concepts
struct FError;
class FBetterHLSLEditorWindow;
// Miscellaneous keywords
const TArray<const TCHAR*> HlslKeywords = TArray(
	{
		TEXT("volatile"),
		TEXT("unorm"),
		TEXT("true"),
		TEXT("static"),
		TEXT("snorm"),
		TEXT("row_major"),
		TEXT("register"),
		TEXT("precise"),
		TEXT("packoffset"),
		TEXT("false"),
		TEXT("const"),
		TEXT("column_major"),
	}
);

const TArray<const TCHAR*> CustomKeywords = TArray(
	{
		TEXT("in"),
		TEXT("out"),
		TEXT("inout"),
	}
);

// HLSL built-in functions
const TArray<const TCHAR*> HlslFunctions = TArray(
	{
		TEXT("abort"),
		TEXT("abs"),
		TEXT("acos"),
		TEXT("all"),
		TEXT("AllMemoryBarrier"),
		TEXT("AllMemoryBarrierWithGroupSync"),
		TEXT("any"),
		TEXT("asdouble"),
		TEXT("asfloat"),
		TEXT("asin"),
		TEXT("asint"),
		TEXT("asuint"),
		TEXT("asuint"),
		TEXT("atan"),
		TEXT("atan2"),
		TEXT("ceil"),
		TEXT("CheckAccessFullyMapped"),
		TEXT("clamp"),
		TEXT("clip"),
		TEXT("cos"),
		TEXT("cosh"),
		TEXT("countbits"),
		TEXT("cross"),
		TEXT("D3DCOLORtoUBYTE4"),
		TEXT("ddx"),
		TEXT("ddx_coarse"),
		TEXT("ddx_fine"),
		TEXT("ddy"),
		TEXT("ddy_coarse"),
		TEXT("ddy_fine"),
		TEXT("degrees"),
		TEXT("determinant"),
		TEXT("DeviceMemoryBarrier"),
		TEXT("DeviceMemoryBarrierWithGroupSync"),
		TEXT("distance"),
		TEXT("dot"),
		TEXT("dst"),
		TEXT("errorf"),
		TEXT("EvaluateAttributeCentroid"),
		TEXT("EvaluateAttributeAtSample"),
		TEXT("EvaluateAttributeSnapped"),
		TEXT("exp"),
		TEXT("exp2"),
		TEXT("f16tof32"),
		TEXT("f32tof16"),
		TEXT("faceforward"),
		TEXT("firstbithigh"),
		TEXT("firstbitlow"),
		TEXT("floor"),
		TEXT("fma"),
		TEXT("fmod"),
		TEXT("frac"),
		TEXT("frexp"),
		TEXT("fwidth"),
		TEXT("GetRenderTargetSampleCount"),
		TEXT("GetRenderTargetSamplePosition"),
		TEXT("GroupMemoryBarrier"),
		TEXT("GroupMemoryBarrierWithGroupSync"),
		TEXT("InterlockedAdd"),
		TEXT("InterlockedAnd"),
		TEXT("InterlockedCompareExchange"),
		TEXT("InterlockedCompareStore"),
		TEXT("InterlockedExchange"),
		TEXT("InterlockedMax"),
		TEXT("InterlockedMin"),
		TEXT("InterlockedOr"),
		TEXT("InterlockedXor"),
		TEXT("isfinite"),
		TEXT("isinf"),
		TEXT("isnan"),
		TEXT("ldexp"),
		TEXT("length"),
		TEXT("lerp"),
		TEXT("lit"),
		TEXT("log"),
		TEXT("log10"),
		TEXT("log2"),
		TEXT("mad"),
		TEXT("max"),
		TEXT("min"),
		TEXT("modf"),
		TEXT("msad4"),
		TEXT("mul"),
		TEXT("noise"),
		TEXT("normalize"),
		TEXT("pow"),
		TEXT("printf"),
		TEXT("Process2DQuadTessFactorsAvg"),
		TEXT("Process2DQuadTessFactorsMax"),
		TEXT("Process2DQuadTessFactorsMin"),
		TEXT("ProcessIsolineTessFactors"),
		TEXT("ProcessQuadTessFactorsAvg"),
		TEXT("ProcessQuadTessFactorsMax"),
		TEXT("ProcessQuadTessFactorsMin"),
		TEXT("ProcessTriTessFactorsAvg"),
		TEXT("ProcessTriTessFactorsMax"),
		TEXT("ProcessTriTessFactorsMin"),
		TEXT("radians"),
		TEXT("rcp"),
		TEXT("reflect"),
		TEXT("refract"),
		TEXT("reversebits"),
		TEXT("round"),
		TEXT("rsqrt"),
		TEXT("saturate"),
		TEXT("sign"),
		TEXT("sin"),
		TEXT("sincos"),
		TEXT("sinh"),
		TEXT("smoothstep"),
		TEXT("sqrt"),
		TEXT("step"),
		TEXT("tan"),
		TEXT("tanh"),
		TEXT("tex1D"),
		TEXT("tex1D"),
		TEXT("tex1Dbias"),
		TEXT("tex1Dgrad"),
		TEXT("tex1Dlod"),
		TEXT("tex1Dproj"),
		TEXT("tex2D"),
		TEXT("tex2D"),
		TEXT("tex2Dbias"),
		TEXT("tex2Dgrad"),
		TEXT("tex2Dlod"),
		TEXT("tex2Dproj"),
		TEXT("tex3D"),
		TEXT("tex3D"),
		TEXT("tex3Dbias"),
		TEXT("tex3Dgrad"),
		TEXT("tex3Dlod"),
		TEXT("tex3Dproj"),
		TEXT("texCUBE"),
		TEXT("texCUBE"),
		TEXT("texCUBEbias"),
		TEXT("texCUBEgrad"),
		TEXT("texCUBElod"),
		TEXT("texCUBEproj"),
		TEXT("transpose"),
		TEXT("trunc"),
	}
);

// Recognized preprocessor directives
const TArray<const TCHAR*> HlslPreProcessorKeywords = TArray(
	{
		TEXT("#include"),
		TEXT("#define"),
		TEXT("#ifndef"),
		TEXT("#ifdef"),
		TEXT("#if"),
		TEXT("#else"),
		TEXT("#endif"),
		TEXT("#pragma"),
		TEXT("#undef"),
	}
);

// Keywords used to define a new type
const TArray<const TCHAR*> HlslTypeKeywords = TArray(
	{
		TEXT("struct"),
		TEXT("enum"),
	}
);

// HLSL primitive types
const TArray<const TCHAR*> HlslPrimitiveTypes = TArray(
	{
		// uints
		TEXT("uint4x4"),
		TEXT("uint4x3"),
		TEXT("uint4x2"),
		TEXT("uint4x1"),
		TEXT("uint4"),
		TEXT("uint3x4"),
		TEXT("uint3x3"),
		TEXT("uint3x2"),
		TEXT("uint3x1"),
		TEXT("uint3"),
		TEXT("uint2x4"),
		TEXT("uint2x3"),
		TEXT("uint2x2"),
		TEXT("uint2x1"),
		TEXT("uint2"),
		TEXT("uint1x4"),
		TEXT("uint1x3"),
		TEXT("uint1x2"),
		TEXT("uint1x1"),
		TEXT("uint1"),
		TEXT("uint"),

		// ints
		TEXT("int4x4"),
		TEXT("int4x3"),
		TEXT("int4x2"),
		TEXT("int4x1"),
		TEXT("int4"),
		TEXT("int3x4"),
		TEXT("int3x3"),
		TEXT("int3x2"),
		TEXT("int3x1"),
		TEXT("int3"),
		TEXT("int2x4"),
		TEXT("int2x3"),
		TEXT("int2x2"),
		TEXT("int2x1"),
		TEXT("int2"),
		TEXT("int1x4"),
		TEXT("int1x3"),
		TEXT("int1x2"),
		TEXT("int1x1"),
		TEXT("int1"),
		TEXT("int"),

		// halfs
		TEXT("half4x4"),
		TEXT("half4x3"),
		TEXT("half4x2"),
		TEXT("half4x1"),
		TEXT("half4"),
		TEXT("half3x4"),
		TEXT("half3x3"),
		TEXT("half3x2"),
		TEXT("half3x1"),
		TEXT("half3"),
		TEXT("half2x4"),
		TEXT("half2x3"),
		TEXT("half2x2"),
		TEXT("half2x1"),
		TEXT("half2"),
		TEXT("half1x4"),
		TEXT("half1x3"),
		TEXT("half1x2"),
		TEXT("half1x1"),
		TEXT("half1"),
		TEXT("half"),

		// floats
		TEXT("float4x4"),
		TEXT("float4x3"),
		TEXT("float4x2"),
		TEXT("float4x1"),
		TEXT("float4"),
		TEXT("float3x4"),
		TEXT("float3x3"),
		TEXT("float3x2"),
		TEXT("float3x1"),
		TEXT("float3"),
		TEXT("float2x4"),
		TEXT("float2x3"),
		TEXT("float2x2"),
		TEXT("float2x1"),
		TEXT("float2"),
		TEXT("float1x4"),
		TEXT("float1x3"),
		TEXT("float1x2"),
		TEXT("float1x1"),
		TEXT("float1"),
		TEXT("float"),

		// bools
		TEXT("bool4x4"),
		TEXT("bool4x3"),
		TEXT("bool4x2"),
		TEXT("bool4x1"),
		TEXT("bool4"),
		TEXT("bool3x4"),
		TEXT("bool3x3"),
		TEXT("bool3x2"),
		TEXT("bool3x1"),
		TEXT("bool3"),
		TEXT("bool2x4"),
		TEXT("bool2x3"),
		TEXT("bool2x2"),
		TEXT("bool2x1"),
		TEXT("bool2"),
		TEXT("bool1x4"),
		TEXT("bool1x3"),
		TEXT("bool1x2"),
		TEXT("bool1x1"),
		TEXT("bool1"),
		TEXT("bool"),

		// misc
		TEXT("void"),
		TEXT("vector"),
		TEXT("dword"),
		TEXT("double"),
		TEXT("matrix"),
		TEXT("Buffer"),
	}
);
#pragma endregion

// A large enum with every operator, reserved keyword and identifier in HLSL
enum class EHLSLTokenType
{
	// Identifiers
	CustomIdentifier,
	PrimitiveType,
	TypeKeyword,
	Function,
	Keyword,

	// Flow control keywords
	If,
	Else,
	For,
	While,
	Do,
	Continue,
	Goto,
	Switch,
	Case,
	Default,
	Break,
	Return,

	// Literals
	True,
	False,
	Null,
	String,
	Number,

	// Grouping operators
	LeftParenthesis, // (
	RightParenthesis, // )
	LeftSquareBracket, // [
	RightSquareBracket, // ]
	LeftCurlyBrace, // {
	RightCurlyBrace, // }

	// Bitwise operators
	BitwiseNot, // ~
	XOR, // ^
	XOREquals, // ^=
	BitwiseAnd, // &
	BitwiseAndEquals, // &=
	BitwiseOr, // |
	BitwiseOrEquals, // |=
	ShiftLeft, // <<
	ShiftLeftEquals, // <<=
	ShiftRight, // >>
	ShiftRightEquals, // >>=

	// Boolean operators
	BooleanNot, // !
	BooleanAnd, // &&
	BooleanOr, // ||

	// Equality operators
	Equals, // ==
	NotEquals, // !=
	LessThan, // <
	LessThanEquals, // <=
	GreaterThan, // >
	GreaterThanEquals, // >=

	// Mathematical operators
	Plus, // +
	PlusEquals, // +=
	Minus, // -
	MinusEquals, // -=
	PrePostFixOperator, // ++ and --
	Asterisk, // *
	AsteriskEquals, // *=
	Modulo, // %
	ModuloEquals, // %=
	Division, // /
	DivisionEquals, // /=

	// Other special operators
	Dot, // .	
	SemiColon, // ;
	DoubleColon, // ::
	QuestionMark, // ?
	Colon, // :
	Comma, // ,
	Assignment, // =

	// Misc markers
	InKeyword,
	OutKeyword,
	InOutKeyword,
	Comment,
	Whitespace,
	Unknown,
	NewLine,
	EndOfFile
};

struct FHLSLToken
{
	int32 LineNumber;
	FTextRange Range;
	FTextRange RangeInLine;
	EHLSLTokenType Type = EHLSLTokenType::Unknown;
	FString Source = "";
};

// Based on the book Crafting Interpreters by Robert Nystrom, chapter 4
// https://craftinginterpreters.com/scanning.html
class FBetterHLSLTokenizer
{
public:
	static TSharedRef<FBetterHLSLTokenizer> Create(TArray<TSharedPtr<FError>>& Errors);

	void Process(TArray<FHLSLToken>& OutTokens, const FString& Input);

protected:
	FBetterHLSLTokenizer(TArray<TSharedPtr<FError>>& Errors);

	bool AtEnd() const;

	// Consumes the next character in the input
	TCHAR Advance();

	// Check if the next character matches the given character
	// If it matches, consumes the next character
	bool Match(const TCHAR Char);

	/// Returns the next character without consuming it
	/// @param PeekOffset How far to look ahead
	TCHAR Peek(const uint32 PeekOffset = 0);

	void ScanTokens();

	void AddToken(EHLSLTokenType Type);

	// Read a string literal
	void String();

	// Read a number literal, including decimal digits
	void Number();

	// Read an identifier name
	void Identifier();

	void Error(FString Message);

	FString Source;
	int32 LineNumber;
	int32 LineIndex;
	int32 StartIndex; // The character index of the start of the current token
	int32 CurrentIndex; // The index of the character that is currently being considered
	TArray<FHLSLToken> Tokens;

	TArray<TSharedPtr<FError>>& Errors;
};
