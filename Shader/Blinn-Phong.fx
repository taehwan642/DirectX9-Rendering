float3 vEyePos; // 카메라의 위치(로컬)

float4x4 mWVP;

float4 vLightDir;
float4 vColor; // 광원 * 메시 색

struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float4 Color : COLOR0;
};

VS_OUTPUT VS
(
	float4 Pos : POSITION, // 로컬 좌표
	float4 Normal : NORMAL // 법선 벡터
)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	// 좌표 변환 (로컬 -> 월드)
	Out.Pos = mul(Pos, mWVP);

	float amb = -vLightDir.w; // 환경광 밝기
	float3 N = Normal.xyz;
	float4 L = -vLightDir;
	float3 eye = normalize(vEyePos - Pos.xyz);
	float3 H = normalize(L + eye);

	Out.Color = vColor * max(amb, dot(Normal, -vLightDir))
		+ pow(max(0, dot(N, H)), 10);

	return Out;
}

float4 PS(VS_OUTPUT In) : COLOR
{
	return In.Color;
}

technique TShader
{
	pass P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile ps_3_0 PS();
	}
};