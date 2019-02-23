cbuffer ConstantBuffer: register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
}

struct VOut {
	float4 pos: SV_POSITION;
	float2 texcoord: TEXCOORD;
};

VOut main(float4 pos: POSITION, float2 texcoord: TEXCOORD)
{
	VOut output;

	output.pos = mul(pos, World);
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Proj);
	output.texcoord = texcoord;

	return output;
}

struct PColorOut // Pixel
{
    float4 pos: SV_POSITION;
    float2 texcoord: TEXCOORD;
};

PColorOut ColorVertexShader(float4 pos: POSITION, float2 texcoord: TEXCOORD)
{
    PColorOut output;

	// Change the position vector to be 4 units for proper matrix calculations.
	pos.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.pos = mul(pos, 1);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Proj);
    
    // Store the texture coordinates for the pixel shader.
    output.texcoord = texcoord;
    
    return output;
}

//------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR0;
};

//------------------------------------------------------------------------------------
// Vertex Shader
//------------------------------------------------------------------------------------
VS_OUTPUT VS(float4 Pos : POSITION, float4 Color : COLOR)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Proj);
	output.Color = float4(0.5f, 0.5f, 0.5f, 1.0f);
	return output;
}