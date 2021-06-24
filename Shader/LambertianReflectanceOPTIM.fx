/*최적화된 램버트 조명 계산*/
// 지금까지는 계수로 I_d * k_d 계산을 했는데, 정점마다 값이 바뀔 일이 없기 때문에
// 미리 계산해두는게 효과적이다.
// 또한 환경광 & 확산 반사광은 색의 강도가 다를 뿐 색감은 비슷비슷하기에
// 일괄 식으로 사용하는게 효율적이다.
// 예 : I = color * (0.3f + 1.0f * (N dot L))
// 여기서 0.3과 1.0은 환경광과 확산 반사광의 강도
// 0.3을 큰 값으로 하면 환경광이 강해지고
// 1.0을 작게 하면 확산 반사광이 작아진다.

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
	float3 Normal : NORMAL // 법선 벡터
)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	// 좌표 변환 (로컬 -> 월드)
	Out.Pos = mul(Pos, mWVP);

	// 정점 색
	float4 L = -vLightDir; 
	float amb = L.w;

	Out.Color = vColor * max(amb, dot(Normal, L)); // 확산광

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