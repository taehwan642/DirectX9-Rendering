/*����ȭ�� ����Ʈ ���� ���*/
// ���ݱ����� ����� I_d * k_d ����� �ߴµ�, �������� ���� �ٲ� ���� ���� ������
// �̸� ����صδ°� ȿ�����̴�.
// ���� ȯ�汤 & Ȯ�� �ݻ籤�� ���� ������ �ٸ� �� ������ �������ϱ⿡
// �ϰ� ������ ����ϴ°� ȿ�����̴�.
// �� : I = color * (0.3f + 1.0f * (N dot L))
// ���⼭ 0.3�� 1.0�� ȯ�汤�� Ȯ�� �ݻ籤�� ����
// 0.3�� ū ������ �ϸ� ȯ�汤�� ��������
// 1.0�� �۰� �ϸ� Ȯ�� �ݻ籤�� �۾�����.

float4x4 mWVP;

float4 vLightDir;
float4 vColor; // ���� * �޽� ��

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
	float4 L = -vLightDir; 
	float amb = L.w;

	Out.Color = vColor * max(amb, dot(Normal, L)); // Ȯ�걤

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