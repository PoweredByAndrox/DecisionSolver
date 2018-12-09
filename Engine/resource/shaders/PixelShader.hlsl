Texture2D diffTexture;
Texture2D shaderTexture;

SamplerState SampleType;

float4 main(float4 pos: SV_POSITION, float2 texcoord: TEXCOORD): SV_TARGET
{
	float4 Textures = diffTexture.Sample(SampleType, texcoord);
	return Textures;
}

float4 ColorPixelShader(float4 pos: SV_POSITION, float2 texcoord: TEXCOORD): SV_TARGET
{
	float4 Texture = shaderTexture.Sample(SampleType, texcoord);
    return Texture;
}