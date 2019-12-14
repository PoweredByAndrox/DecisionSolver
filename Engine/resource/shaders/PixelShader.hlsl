Texture2D ObjTexture;
SamplerState SampleType;

struct PInputTypeModel
{
	float4 Pos: SV_POSITION;
	float2 Tex: TEXCOORD;
};
struct PInputTypeUI
{
	float4 Pos: SV_POSITION;
	float4 col: COLOR0;
	float2 Tex: TEXCOORD;
};

struct DDrawPS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

float4 Pixel_model_PS(PInputTypeModel input): SV_Target
{
	return ObjTexture.Sample(SampleType, input.Tex);
}

float4 Pixel_ui_PS(PInputTypeUI input): SV_Target
{
	return input.col * ObjTexture.Sample(SampleType, input.Tex);
}

float4 DDraw_PS(DDrawPS_INPUT input) : SV_Target
{
    return input.Color;
}