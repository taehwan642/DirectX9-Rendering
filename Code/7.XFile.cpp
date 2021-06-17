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

		if (FAILED(D3DXLoadMeshFromX(finalpath, D3DXMESH_MANAGED, device, nullptr, &subsetBuffer, nullptr, nullptr, &mesh)))
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