float4x4 mWVP;

texture Tex;

sampler Samp = sampler_state
{
	Texture = <Tex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = Clamp;
	AddressV = Clamp;
};

struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD0;
};

VS_OUTPUT VS
(
	float4 Pos : POSITION,
	float2 Tex : TEXCOORD
)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	Out.Pos = mul(Pos, mWVP);
	Out.Tex = Tex;
	return Out;
}

float4 PS(VS_OUTPUT In) : COLOR
{
	return tex2D(Samp, In.Tex);
}

technique TShader
{
	pass P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile ps_3_0 PS();
	}
};