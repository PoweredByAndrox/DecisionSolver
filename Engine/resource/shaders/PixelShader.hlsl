Texture2D ObjTexture;
SamplerState SampleType;

struct PixelInputType
{
	float4 Pos: SV_POSITION;
	float2 Text: TEXCOORD;
};

float4 PS(PixelInputType input): SV_Target
{
	return ObjTexture.Sample(SampleType, input.Text);
}