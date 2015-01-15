Texture2D		SampleTexture : register(t0);
Texture2D		RedTexture : register(t1);
Texture2D		GreenTexture : register(t2);
Texture2D		BlueTexture : register(t3);
Texture2D		AllColorTexture : register(t4);
Texture2D		FinalTexture : register(t5);
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

struct PSOutput
{
	float4 RedOutput			: SV_Target0;
	float4 GreenOutput			: SV_Target1;
	float4 BlueOutput			: SV_Target2;
	float4 AllColOutput			: SV_Target3;
};

VS_OUTPUT VSMAIN(in VS_INPUT input)
{
	VS_OUTPUT output;

	output.position = input.position;
	output.tex = input.tex;
	return output;
}

PSOutput PSMAIN(in VS_OUTPUT input)
{
	PSOutput output;
	float4 vSample = SampleTexture.Sample(LinearSampler, input.tex);
	output.RedOutput = vSample;
	output.RedOutput.r = 1.0f - output.RedOutput.r;
	output.GreenOutput = vSample;
	output.GreenOutput.g = 1.0f - output.GreenOutput.g;
	output.BlueOutput = vSample;
	output.BlueOutput.b = 1.0f - output.BlueOutput.b;
	output.AllColOutput = vSample;
	return output;
}

float4 PSFINAL(in VS_OUTPUT input) : SV_Target0
{
	float4 red = RedTexture.Sample(LinearSampler, input.tex);
	float4 green = GreenTexture.Sample(LinearSampler, input.tex);
	float4 blue = BlueTexture.Sample(LinearSampler, input.tex);

	float4 vSample = float4(1.0f - red.r, 1.0f - green.g, 1.0f - blue.b, 1.0f);
	return vSample;
}