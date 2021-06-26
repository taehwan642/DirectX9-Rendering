#include "DXUT.h"
#include "Draw.h"
#include <string>

namespace XFile
{
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

		if (FAILED(D3DXLoadMeshFromX(
			finalpath, // 읽어들이고자 하는 XFile의 파일명
			D3DXMESH_MANAGED, // 메시를 만드는 데 이용될 하나 이상의 생성 플래그
			// 32BIT == 메시는 32비트 인덱스를 이용하게 된다
			// MANAGED == 메시는 관리 메모리 풀 내에 보관된다
			// WRITEONLY == 메시 데이터는 쓰기만 허용된다
			// DYNAMIC == 메시 버퍼는 동적으로 만들어진다
			device, // 메시와 연계될 장치
			nullptr, // 메시 접근 정보를 위한 DWORD 배열을 포함하는 ID3DXBuffer를 리턴
			&subsetBuffer, // 매시 재질 정보를 위한 D3DXMATERIAL 구조체 배열을 포함하는 ID3DXBuffer를 리턴
			nullptr, // D3DXEFFECTINSTANCE 구조체 배열을 포함하는 ID3DXBuffer를 리턴
			nullptr, // 메시의 재질 수를 리턴 (subsetBuffer로 받은 D3DXMATERIAL 배열 내 요소의 수)
			&mesh))) // XFile 기하정보로 채워진 ID3DXMesh 객체를 리턴
			return E_FAIL;
		
		subset = static_cast<LPD3DXMATERIAL>(subsetBuffer->GetBufferPointer());

		WCHAR _filename[MAX_PATH] = L"";
		::lstrcpy(finalpath, filepath.c_str());
		MultiByteToWideChar(CP_ACP, 0, subset[0].pTextureFilename, strlen(subset[0].pTextureFilename), _filename, MAX_PATH);
		::lstrcat(finalpath, _filename);

		if (FAILED(D3DXCreateTextureFromFile(device, finalpath, &texture)))
			return E_FAIL;

		device->SetRenderState(D3DRS_DITHERENABLE, FALSE);
		device->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
		device->SetRenderState(D3DRS_ZENABLE, TRUE);
		device->SetRenderState(D3DRS_AMBIENT, 0x000F0F0F);
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		device->SetRenderState(D3DRS_LIGHTING, FALSE);

		device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

		// D3DTEXF_LINEAR == Bilinear Interpolation
		// D3DTEXF_POINT == Nearest neighbor Interpolation
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

		return S_OK;
	}
	
	void SetMatrices(LPDIRECT3DDEVICE9 device)
	{
		D3DXMATRIX mWorld, mView, mProj;
		D3DXMatrixIdentity(&mWorld);
		device->SetTransform(D3DTS_WORLD, &mWorld);
		
		D3DXVECTOR3 eye{ 0.f,5.f,-8.f };
		D3DXVECTOR3 lookat{ 0.f,-2.f,0.f };
		D3DXVECTOR3 up{ 0.f,1.f,0.f };
		D3DXMatrixLookAtLH(&mView, &eye, &lookat, &up);
		device->SetTransform(D3DTS_VIEW, &mView);

		D3DXMatrixPerspectiveFovLH(&mProj, 
			D3DX_PI / 4, // 시야각
			1280 / 720,  // 화면 비율
			1.f,		 // 전방 클립면
			100.f);		 // 후방 클립면

		device->SetTransform(D3DTS_PROJECTION, &mProj);
	}

	void Update(LPDIRECT3DDEVICE9 device)
	{
		SetMatrices(device);

		device->SetTexture(0, texture);
		mesh->DrawSubset(0);
	}

	void Close()
	{
		texture->Release();
		subsetBuffer->Release();
		mesh->Release();
	}
}