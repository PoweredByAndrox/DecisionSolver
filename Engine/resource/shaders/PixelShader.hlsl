Texture2D diffTexture;
SamplerState SampleType;

float4 main(float4 pos: SV_POSITION, float2 texcoord: TEXCOORD): SV_TARGET
{
	float4 Textures = diffTexture.Sample(SampleType, texcoord);

	return Textures;
}

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 ColorPixelShader(PixelInputType input): SV_TARGET
{
    return input.color;
}