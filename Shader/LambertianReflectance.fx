float4x4 mWVP;
float4x4 mWIT; // 로컬 좌표계를 월드 좌표계로 변환하는 행렬의 역-전치 행렬.

float3 vLightDir;

// 광원 밝기
float4 I_a = {0.3f, 0.3f, 0.3f, 0.0f}; // ambient
float4 I_d = {0.7f, 0.7f, 0.7f, 0.0f}; // diffuse

// 반사율
float4 k_a = {1.0f, 1.0f, 1.0f, 1.0f}; // ambient
float4 k_d = {1.0f, 1.0f, 1.0f, 1.0f}; // diffuse

struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float4 Color : COLOR0;
};

VS_OUTPUT VS
(
	float4 Pos : POSITION, // 로컬 좌표
	float3 Normal : NORMAL // 법선 벡터
)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	// 좌표 변환 (로컬 -> 월드)
	Out.Pos = mul(Pos, mWVP);

	// 정점 색
	float3 L = -vLightDir;
	float3 N = normalize(mul(Normal, (float3x3)mWIT)); // 월드 좌표계에서의 법선

	Out.Color = I_a * k_a // 환경광
		+ I_d * k_d * max(0, dot(N, L)); // 확산광

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