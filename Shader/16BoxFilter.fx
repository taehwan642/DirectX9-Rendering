
texture SrcMap;
sampler SrcSamp = sampler_state
{
	Texture = <SrcMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = Clamp;
	AddressV = Clamp;
};


struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float2 Tex0 : TEXCOORD0;
	float2 Tex1 : TEXCOORD1;
	float2 Tex2 : TEXCOORD2;
	float2 Tex3 : TEXCOORD3;
};

VS_OUTPUT VS
(
	float4 Pos : POSITION,
	float4 Tex : TEXCOORD0
)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	Out.Pos = Pos;

	Out.Tex0 = Tex + float2(-1.f / 1280, -1.f / 720);
	Out.Tex1 = Tex + float2(+1.f / 1280, -1.f / 720);
	Out.Tex2 = Tex + float2(-1.f / 1280, +1.f / 720);
	Out.Tex3 = Tex + float2(+1.f / 1280, +1.f / 720);

	return Out;
}

float4 PS(VS_OUTPUT In) : COLOR0
{
	float4 t0 = tex2D(SrcSamp, In.Tex0);
	float4 t1 = tex2D(SrcSamp, In.Tex1);
	float4 t2 = tex2D(SrcSamp, In.Tex2);
	float4 t3 = tex2D(SrcSamp, In.Tex3);

	return (t0 + t1 + t2 + t3) / 4;
}

technique TShader
{
	pass P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile ps_3_0 PS();
		Sampler[0] = (SrcSamp);
		Sampler[1] = (SrcSamp);
		Sampler[2] = (SrcSamp);
		Sampler[3] = (SrcSamp);
	}
};