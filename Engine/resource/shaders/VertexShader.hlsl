cbuffer ConstantBuffer: register(b0)
{
	float4x4 WVP;
}

struct VertexInputType
{
	float4 Pos: POSITION;
	float2 Text: TEXCOORD;
};

struct PixelInputType
{
	float4 Pos: SV_POSITION;
	float2 Text: TEXCOORD;
};

PixelInputType VS(VertexInputType input)
{
	PixelInputType output;

	output.Pos = mul(input.Pos, WVP);
	output.Text = input.Text;

	return output;
}