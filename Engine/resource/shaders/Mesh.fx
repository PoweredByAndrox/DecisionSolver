// Вывод на экран трехмерной модели
// Файл mesh.fx
//-----------------------------------------------------------------------
// Структуры данных
//-----------------------------------------------------------------------
struct VS_INPUT
{
 float4 Pos : POSITION;
 float3 Norm : NORMAL;
 float2 Tex : TEXCOORD0;
};
struct PS_INPUT
{
 float4 Pos : SV_POSITION; 
 float3 Norm : TEXCOORD0;
 float2 Tex : TEXCOORD1;
};
SamplerState samLinear
{
 Filter = MIN_MAG_MIP_LINEAR;
 AddressU = Wrap;
 AddressV = Wrap;
};
DepthStencilState EnableDepth
{
 DepthEnable = TRUE;
 DepthWriteMask = ALL;
 DepthFunc = LESS_EQUAL;
};
BlendState NoBlending
{
 AlphaToCoverageEnable = FALSE;
 BlendEnable[0] = FALSE;
};
//-----------------------------------------------------------------------
// Глобальные переменные
//-----------------------------------------------------------------------
matrix World;
matrix View;
matrix Projection;
float3 vLightDir = float3(-0.577,0.577,-0.577);
Texture2D g_txDiffuse;
//-----------------------------------------------------------------------
// Функция вершинного шейдера
//-----------------------------------------------------------------------
PS_INPUT VS( VS_INPUT Data )
{ 
 PS_INPUT Out;
 Out = (PS_INPUT)0;
 // Умножаем координаты вершины
 // на матрицы преобразований
 Out.Pos = mul (Data.Pos, World );
 Out.Pos = mul (Out.Pos, View);
 Out.Pos = mul (Out.Pos, Projection);

 // Умножаем нормали на мировую матрицу
 Out.Norm = mul (Data.Norm, World);

 // Текстурные координаты
 // передаём без изменений
 Out.Tex = Data.Tex;
 return Out;
}
//-----------------------------------------------------------------------
// Функция пиксельного шейдера (рисует текстуру с учетом освещения)
//-----------------------------------------------------------------------
float4 PS_Mesh( PS_INPUT input ) : SV_Target
{
 float fLighting = saturate( dot( input.Norm, vLightDir ) );
 float4 finalColor = g_txDiffuse.Sample( samLinear, input.Tex ) *
 fLighting;
 finalColor.a = 1;
 return finalColor;
}
//-----------------------------------------------------------------------
// Technique
//-----------------------------------------------------------------------
technique10 RenderMesh
{
 pass P0
 { 
 SetVertexShader( CompileShader(vs_5_0, VS() ) );
 SetGeometryShader( NULL );
 SetPixelShader( CompileShader(ps_5_0, PS_Mesh() ) );
 SetDepthStencilState( EnableDepth, 0 );
 SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ),
 0xFFFFFFFF );
 }
}