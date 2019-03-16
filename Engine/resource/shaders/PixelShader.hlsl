Texture2D ObjTexture;
SamplerState SampleType;

struct PixelInputType
{
	float4 Pos: SV_POSITION;
	float4 col : COLOR0;
	float2 Text: TEXCOORD;
};

float4 PS(PixelInputType input): SV_Target
{
	return input.col * ObjTexture.Sample(SampleType, input.Text);
}