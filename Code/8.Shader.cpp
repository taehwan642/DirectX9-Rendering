#include "DXUT.h"
#include "Draw.h"
#include <string>

namespace Shader
{
	LPD3DXEFFECT effect;
	D3DXHANDLE technique;
	D3DXHANDLE mWVP;

	LPD3DXMESH mesh;
	LPD3DXBUFFER subsetBuffer;
	LPD3DXMATERIAL subset;
	LPDIRECT3DTEXTURE9 texture;
	std::wstring filepath = L"Source/";
	std::wstring filename = L"map.x";

	HRESULT Initialize(LPDIRECT3DDEVICE9 device)
	{
		WCHAR finalpath[MAX_PATH];

		::lstrcpy(finalpath, filepath.c_str());
		::lstrcat(finalpath, filename.c_str());

		if (FAILED(D3DXLoadMeshFromX(finalpath, D3DXMESH_MANAGED, device, nullptr, &subsetBuffer, nullptr, nullptr, &mesh)))
			return E_FAIL;

		subset = static_cast<LPD3DXMATERIAL>(subsetBuffer->GetBufferPointer());

		WCHAR _filename[MAX_PATH] = L"";
		::lstrcpy(finalpath, filepath.c_str());
		MultiByteToWideChar(CP_ACP, 0, subset[0].pTextureFilename, strlen(subset[0].pTextureFilename), _filename, MAX_PATH);
		::lstrcat(finalpath, _filename);

		if (FAILED(D3DXCreateTextureFromFile(device, finalpath, &texture)))
			return E_FAIL;


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
		
		if(error != nullptr) error->Release();

		return S_OK;
	}
	
	// ������ ũ�Ⱑ �ٲ���� �� �� ����̽��� ���µǾ��� ��, OnLostDevice�� ȣ��
	// �������κ��� �����Ǿ��� ��, OnResetDevice�� ȣ��

	void Update(LPDIRECT3DDEVICE9 device)
	{
		effect->SetTechnique(technique);
		effect->Begin(NULL, 0);
		effect->BeginPass(0);

		D3DXMATRIX mWorld, mView, mProj;
		D3DXMatrixIdentity(&mWorld);
		D3DXVECTOR3 eye{ 0.f,5.f,-8.f };
		D3DXVECTOR3 lookat{ 0.f,-2.f,0.f };
		D3DXVECTOR3 up{ 0.f,1.f,0.f };
		D3DXMatrixLookAtLH(&mView, &eye, &lookat, &up);
		D3DXMatrixPerspectiveFovLH(&mProj,
			D3DX_PI / 4, // �þ߰�
			1280 / 720,  // ȭ�� ����
			1.f,		 // ���� Ŭ����
			100.f);		 // �Ĺ� Ŭ����

		D3DXMATRIX m = mWorld * mView * mProj;
		effect->SetMatrix(mWVP, &m);
		effect->SetTexture((D3DXHANDLE)"Tex", texture);
		mesh->DrawSubset(0);

		effect->EndPass();
		effect->End();
	}

	void Close()
	{
		effect->Release();
		texture->Release();
		mesh->Release();
	}
}