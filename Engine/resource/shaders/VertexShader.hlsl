cbuffer ConstantBuffer: register(b0)
{
	float4x4 WVP;
}

struct VInputTypeModel
{
	float4 Pos: POSITION;
	float2 Tex: TEXCOORD;
};

struct POutputTypeModel
{
	float4 Pos: SV_POSITION;
	float2 Tex: TEXCOORD;
};

struct VInputTypeUI
{
	float4 Pos: POSITION;
	float4 col: COLOR0;
	float2 Tex: TEXCOORD;
};

struct POutputTypeUI
{
	float4 Pos: SV_POSITION;
	float4 col: COLOR0;
	float2 Tex: TEXCOORD;
};

POutputTypeModel Vertex_model_VS(VInputTypeModel input)
{
	POutputTypeModel output;

	output.Pos = mul(input.Pos, WVP);
	output.Tex = input.Tex;

	return output;
}
POutputTypeUI Vertex_ui_VS(VInputTypeUI input)
{
	POutputTypeUI output;

	output.Pos = mul(input.Pos, WVP);
	output.col = input.col;
	output.Tex = input.Tex;

	return output;
}