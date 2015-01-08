//
//
//
//
Texture2D       InputMap : register(t0);
SamplerState    LinearSampler : register( s0 );

cbuffer ImageViewingData
{
	float4 WindowSize;		// The size of the window being used
	//float4 ImageSize;		// The size of the image being displayed
	//float4 ViewingParams;	// x,y = tex coords for center of view
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
    float2 tex      : TEXCOORD0;
};

float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	//int3 screenspace = int3( input.position.x, input.position.y, 0 );

	//float4 vSample = ColorMap00.Load( screenspace );
	//float4 vSample = InputMap.Sample( LinearSampler, input.tex );
	const uint2 pos = uint2((input.position.xy + WindowSize.xy)/uint(WindowSize.z));
	float4 vSample = InputMap[pos];
	return( vSample );
}

