#include "DXUT.h"
#include "Draw.h"

namespace Shader
{
	LPD3DXEFFECT effect;
	D3DXHANDLE technique;
	D3DXHANDLE mWVP;

	HRESULT Initialize(LPDIRECT3DDEVICE9 device)
	{
		LPD3DXBUFFER error;
		if (FAILED(D3DXCreateEffectFromFile(
			device, // ����̽�
			L"Shader/Shader.fx", // ����Ʈ ���� �� ������
			NULL, // ��ó���� ������
			NULL, // �ɼ� �������̽� ������
			0, // D3DXSHADER �ĺ� ������ �ɼ�
			NULL, // ���� �μ��� ����ϴ� ID3DXEffectPool ������Ʈ ������
			&effect, // �����ϵ� ����Ʈ ������ ����� ����
			&error))) // ������ ������ ����� ����
		{
			MessageBox(NULL, (LPCTSTR)error->GetBufferPointer(), L"ERROR", MB_OK);
		}
		else
		{
			technique = effect->GetTechniqueByName("TShader");
			mWVP = effect->GetParameterByName(NULL, "mWVP");
		}
		error->Release();

		return S_OK;
	}
	
	// ������ ũ�Ⱑ �ٲ���� �� �� ����̽��� ���µǾ��� ��, OnLostDevice�� ȣ��
	// �������κ��� �����Ǿ��� ��, OnResetDevice�� ȣ��


	void Update(LPDIRECT3DDEVICE9 device)
	{
		effect->SetTechnique(technique);
		effect->Begin(NULL, 0);
		effect->BeginPass(0);

		

		effect->EndPass();
		effect->End();
	}

	void Close()
	{
		effect->Release();
	}
}