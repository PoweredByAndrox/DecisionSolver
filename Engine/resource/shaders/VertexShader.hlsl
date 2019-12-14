cbuffer ConstantBuffer: register(b0)
{
    matrix World, View, Proj;
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

struct DDrawVS_INPUT
{
    float4 Pos : POSITION;
    float4 Color : COLOR;
};

struct DDrawPS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

POutputTypeModel Vertex_model_VS(VInputTypeModel input)
{
	POutputTypeModel output;

	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Proj);
	output.Tex = input.Tex;

	return output;
}
POutputTypeUI Vertex_ui_VS(VInputTypeUI input)
{
	POutputTypeUI output;

	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Proj);
	output.Tex = input.Tex;

	return output;
}

DDrawPS_INPUT DDraw_VS(DDrawVS_INPUT input)
{
    DDrawPS_INPUT output;

    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Proj);
    output.Color = input.Color;

    return output;
}
