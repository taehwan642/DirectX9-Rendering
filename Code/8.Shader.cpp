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
			device, // 디바이스
			L"Shader/Shader.fx", // 이펙트 파일 명 포인터
			NULL, // 전처리기 포인터
			NULL, // 옵션 인터페이스 포인터
			0, // D3DXSHADER 식별 컴파일 옵션
			NULL, // 공유 인수로 사용하는 ID3DXEffectPool 오브젝트 포인터
			&effect, // 컴파일된 이펙트 파일이 저장될 버퍼
			&error))) // 컴파일 에러가 저장될 버퍼
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
	
	// 윈도우 크기가 바뀌었을 때 등 디바이스가 리셋되었을 때, OnLostDevice를 호출
	// 리셋으로부터 복원되었을 때, OnResetDevice를 호출

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
			D3DX_PI / 4, // 시야각
			1280 / 720,  // 화면 비율
			1.f,		 // 전방 클립면
			100.f);		 // 후방 클립면

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