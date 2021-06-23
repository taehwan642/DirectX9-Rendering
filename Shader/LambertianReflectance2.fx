float4x4 mWVP;

float3 vLightDir;

// ���� ���
float4 I_a = { 0.3f, 0.3f, 0.3f, 0.0f }; // ambient
float4 I_d = { 0.7f, 0.7f, 0.7f, 0.0f }; // diffuse

// �ݻ���
float4 k_a = { 1.0f, 1.0f, 1.0f, 1.0f }; // ambient
float4 k_d = { 1.0f, 1.0f, 1.0f, 1.0f }; // diffuse

struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float4 Color : COLOR0;
};

VS_OUTPUT VS
(
	float4 Pos : POSITION, // ���� ��ǥ
	float3 Normal : NORMAL // ���� ����
)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	// ��ǥ ��ȯ (���� -> ����)
	Out.Pos = mul(Pos, mWVP);

	// ���� ��
	float3 L = -vLightDir; // ���� ��ǥ������ ���� ����
	float3 N = Normal; // ���� ��ǥ�迡���� ����

	// ���忡�� ������ ����ϴ� �ͺ���, ��ǥ ��ȯ�� ����ȭ�� �ʿ���� ������ �� ����� �� ������

	Out.Color = I_a * k_a // ȯ�汤
		+ I_d * k_d * max(0, dot(N, L)); // Ȯ�걤

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