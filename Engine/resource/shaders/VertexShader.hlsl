cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
}

struct VOut {
	float4 pos: SV_POSITION;
	float2 texcoord : TEXCOORD;
};

struct VColorInput // Vertex
{
	float4 pos: POSITION;
	float2 color: COLOR;
};

struct PColorOut // Pixel
{
	float4 pos: SV_POSITION;
	float2 color: COLOR;
};

VOut main(float4 pos : POSITION, float2 texcoord : TEXCOORD)
{
	VOut output;

	output.pos = mul(pos, 1);
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Proj);
	output.texcoord = texcoord;

	return output;
}

PColorOut ColorVertexShader(VColorInput input)
{
    PColorOut output;

	// Change the position vector to be 4 units for proper matrix calculations.
    input.pos.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.pos = mul(input.pos, 1);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Proj);
    
	// Store the input color for the pixel shader to use.
    output.color = input.color;
    
    return output;
}