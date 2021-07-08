#include "DXUT.h"
#include "Draw.h"
#include <string>

namespace BoxFilter
{
	// ����Ʈ ����Ʈ��
	// 1. �׳� ������ ��
	// 2. ������ ȭ���� �ؽ�ó�� ����ؼ� �������� ȭ�� ��ü�� ������
	// ������ ȭ���� �ؽ�ó�� ����Ϸ��� �ؽ�ó�� �������ؾ��ϰ�,
	// ������ Ÿ���� �� �ؽ�ó�� �� �ؽ�ó�� �ٷ� ǥ���� �ʿ��ϴ�.
	// �� �������� ���� ���� ���۵� �ʿ��ϴ�.

	LPDIRECT3DSURFACE9 mapZ; // ���� ����
	LPDIRECT3DTEXTURE9 originalTex; // �ؽ�ó
	LPDIRECT3DSURFACE9 originalSurf; // ǥ��

	LPD3DXEFFECT effect;
	D3DXHANDLE technique;
	D3DXHANDLE mWVP;
	D3DXHANDLE lightDir;

	LPD3DXMESH background;
	LPD3DXMESH ufo;

	LPD3DXBUFFER subsetBuffer;
	LPD3DXMATERIAL subset;
	LPDIRECT3DTEXTURE9 texture;
	DWORD numMaterial;
	std::wstring filepath = L"Source/";
	std::wstring filename = L"map.x";
	std::wstring filename2 = L"ufo.x";

	HRESULT Initialize(LPDIRECT3DDEVICE9 device)
	{
		WCHAR finalpath[MAX_PATH];

		::lstrcpy(finalpath, filepath.c_str());
		::lstrcat(finalpath, filename.c_str());

		if (FAILED(D3DXLoadMeshFromX(finalpath, D3DXMESH_MANAGED, device, nullptr, &subsetBuffer, nullptr, nullptr, &background)))
			return E_FAIL;

		subset = static_cast<LPD3DXMATERIAL>(subsetBuffer->GetBufferPointer());

		WCHAR _filename[MAX_PATH] = L"";
		::lstrcpy(finalpath, filepath.c_str());
		MultiByteToWideChar(CP_ACP, 0, subset[0].pTextureFilename, strlen(subset[0].pTextureFilename), _filename, MAX_PATH);
		::lstrcat(finalpath, _filename);

		if (FAILED(D3DXCreateTextureFromFile(device, finalpath, &texture)))
			return E_FAIL;

		::lstrcpy(finalpath, filepath.c_str());
		::lstrcat(finalpath, filename2.c_str());

		if (FAILED(D3DXLoadMeshFromX(finalpath, D3DXMESH_MANAGED, device, nullptr, &subsetBuffer, nullptr, &numMaterial, &ufo)))
			return E_FAIL;

		LPD3DXBUFFER error;

		if (FAILED(D3DXCreateEffectFromFile(
			device, // ����̽�
			L"Shader/16BoxFilter.fx", // ����Ʈ ���� �� ������
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
			lightDir = effect->GetParameterByName(NULL, "vLightDir");
		}

		if (error != nullptr) error->Release();

		device->SetRenderState(D3DRS_LIGHTING, TRUE);

		mapZ = NULL;
		originalTex = NULL;
		originalSurf = NULL;

		// ���� ���� ����
		if (FAILED(device->CreateDepthStencilSurface(1280, 720, D3DFMT_D16,
			D3DMULTISAMPLE_NONE, 0, TRUE, &mapZ, NULL))) return E_FAIL;

		// �Ϲ� ȭ���� �������� ��
		// ������ Ÿ���� �����ϴ� �����̴�
		if(FAILED(device->CreateTexture(1280, 720, 1, 
			D3DUSAGE_RENDERTARGET, D3DFMT_A8B8G8R8,
			D3DPOOL_DEFAULT, &originalTex, NULL))) return E_FAIL;

		if (FAILED(originalTex->GetSurfaceLevel(0, &originalSurf))) return E_FAIL;

		return S_OK;
	}

	void DrawBackGround(LPDIRECT3DDEVICE9 device)
	{
		device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

		device->SetTexture(0, texture);
		background->DrawSubset(0);
	}

	void DrawUFO(LPDIRECT3DDEVICE9 device)
	{
		device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

		device->SetRenderState(D3DRS_LIGHTING, TRUE);
		device->LightEnable(0, true);

		D3DXMATRIX mWorld, mView, mProj;

		D3DXVECTOR3 eye{ 0.f,3.f,-8.f };
		D3DXVECTOR3 lookat{ 0.f,-0.f,0.f };
		D3DXVECTOR3 up{ 0.f,1.f,0.f };
		D3DXMatrixLookAtLH(&mView, &eye, &lookat, &up);
		D3DXMatrixPerspectiveFovLH(&mProj,
			D3DX_PI / 4, // �þ߰�
			1280 / 720,  // ȭ�� ����
			1.f,		 // ���� Ŭ����
			100.f);		 // �Ĺ� Ŭ����


		D3DXMATRIX mTrans, mRot, m;
		D3DXMatrixTranslation(&m, 1.0f, 0.0f, 0.0f);
		static float delta = 0;
		delta += DXUTGetElapsedTime();
		D3DXMatrixRotationY(&mRot, delta);
		D3DXMatrixTranslation(&mTrans, 0.0f, 2.2f, -4.0f);
		mWorld = m * mRot * mTrans;
		device->SetTransform(D3DTS_WORLD, &mWorld);
		device->SetTransform(D3DTS_VIEW, &mView);
		device->SetTransform(D3DTS_PROJECTION, &mProj);

		device->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL);
		for (int i = 0; i < numMaterial; ++i)
		{
			ufo->DrawSubset(i);
		}
	}

	void Update(LPDIRECT3DDEVICE9 device)
	{
		LPDIRECT3DSURFACE9 oldBackBuffer, oldZBuffer;
		D3DVIEWPORT9 oldViewport;

		// ������ Ÿ�� ����
		device->GetRenderTarget(0, &oldBackBuffer);
		device->GetDepthStencilSurface(&oldZBuffer);
		device->GetViewport(&oldViewport);

		// ������ Ÿ�� ����
		device->SetRenderTarget(0, originalSurf);
		device->SetDepthStencilSurface(mapZ);
		// ����Ʈ ����
		D3DVIEWPORT9 viewport = { 0, 0, // ���� ��� ��ǥ
			1280, 720, // ��, ����
			0.0f, 1.0f }; // ����, �ĸ�
		device->SetViewport(&viewport);

		// ������ Ÿ�� Ŭ����
		device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0);
		
		DrawBackGround(device);
		DrawUFO(device);

		// �ؽ�ó�� �������� �� ��, ������ Ÿ���� ������ ������ ���۷� ����.
		device->SetRenderTarget(0, oldBackBuffer);
		device->SetDepthStencilSurface(oldZBuffer);
		device->SetViewport(&oldViewport);
		oldBackBuffer->Release();
		oldZBuffer->Release();

		device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0);
		
		if (effect != nullptr)
		{
			effect->SetTechnique(technique);
			effect->Begin(NULL, 0);
			effect->BeginPass(0);

			device->SetRenderState(D3DRS_ZENABLE, false);
			device->SetRenderState(D3DRS_LIGHTING, false);
			typedef struct { FLOAT p[4]; FLOAT t[2]; } TVERTEX;
			TVERTEX Vertex1[4] =
			{
				{-1.f, +1.f, 0.1f, 0, 0},
				{+1.f, +1.f, 0.1f, 1, 0},
				{+1.f, -1.f, 0.1f, 1, 1},
				{-1.f, -1.f, 0.1f, 0, 1}
			};

			device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
			effect->SetTexture((D3DXHANDLE)"SrcMap", originalTex);
			device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, Vertex1, sizeof(TVERTEX));

			effect->EndPass();
			effect->End();
		}
	}

	void Close()
	{
		effect->Release();
		texture->Release();
		background->Release();
		ufo->Release();
	}
}