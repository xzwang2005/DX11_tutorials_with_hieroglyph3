Texture2D       SampleTexture;
Texture2D		ImageTexture;
SamplerState    LinearSampler;

struct VS_INPUT
{
	float4 position : POSITION;
	float2 tex		: TEXCOORDS0;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float2 tex      : TEXCOORD0;
};


VS_OUTPUT VSMAIN(in VS_INPUT input)
{
	VS_OUTPUT output;

	output.position = input.position;
	output.tex = input.tex;
	return output;
}

float4 PSMAIN(in VS_OUTPUT input) : SV_Target
{
	float4 vSample = SampleTexture.Sample(LinearSampler, input.tex);
	return(vSample);
}

float4 PSFINAL(in VS_OUTPUT input) : SV_Target
{
	float4 vSample = ImageTexture.Sample(LinearSampler, input.tex);
	if (input.tex.x > 0.5) vSample.x = 0.8f;		// make this shader visible in end result
	return(vSample);
}