cbuffer Transforms
{
	matrix WorldViewProjMatrix;
};

struct VS_INPUT
{
    float3 position: POSITION;
    float3 normal:  NORMAL;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

struct PS_INPUT
{
    float4 position: SV_POSITION;
    float4 color : COLOR;
};

PS_INPUT VSMAIN(VS_INPUT input)
{
    PS_INPUT output;
	output.position = mul(float4(input.position, 1.0f), WorldViewProjMatrix);
    output.color = input.color;
    return output;
};

float4 PSMAIN(PS_INPUT input) : SV_Target
{
    return input.color;
};