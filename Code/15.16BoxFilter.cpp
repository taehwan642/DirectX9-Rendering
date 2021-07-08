#include "DXUT.h"
#include "Draw.h"
#include <string>

namespace BoxFilter
{
	// 포스트 이펙트는
	// 1. 그냥 렌더링 후
	// 2. 렌더링 화면을 텍스처를 사용해서 폴리곤을 화면 전체로 렌더링
	// 렌더링 화면을 텍스처로 사용하려면 텍스처에 렌더링해야하고,
	// 렌더링 타겟이 될 텍스처와 그 텍스처를 다룰 표면이 필요하다.
	// 또 렌더링을 위한 깊이 버퍼도 필요하다.

	LPDIRECT3DSURFACE9 mapZ; // 깊이 버퍼
	LPDIRECT3DTEXTURE9 originalTex; // 텍스처
	LPDIRECT3DSURFACE9 originalSurf; // 표면

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
			device, // 디바이스
			L"Shader/16BoxFilter.fx", // 이펙트 파일 명 포인터
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
			lightDir = effect->GetParameterByName(NULL, "vLightDir");
		}

		if (error != nullptr) error->Release();

		device->SetRenderState(D3DRS_LIGHTING, TRUE);

		mapZ = NULL;
		originalTex = NULL;
		originalSurf = NULL;

		// 깊이 버퍼 생성
		if (FAILED(device->CreateDepthStencilSurface(1280, 720, D3DFMT_D16,
			D3DMULTISAMPLE_NONE, 0, TRUE, &mapZ, NULL))) return E_FAIL;

		// 일반 화면을 렌더링할 곳
		// 렌더링 타겟을 생성하는 절차이다
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
			D3DX_PI / 4, // 시야각
			1280 / 720,  // 화면 비율
			1.f,		 // 전방 클립면
			100.f);		 // 후방 클립면


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

		// 렌더링 타겟 보존
		device->GetRenderTarget(0, &oldBackBuffer);
		device->GetDepthStencilSurface(&oldZBuffer);
		device->GetViewport(&oldViewport);

		// 렌더링 타겟 변경
		device->SetRenderTarget(0, originalSurf);
		device->SetDepthStencilSurface(mapZ);
		// 뷰포트 변경
		D3DVIEWPORT9 viewport = { 0, 0, // 좌측 상단 좌표
			1280, 720, // 폭, 높이
			0.0f, 1.0f }; // 전면, 후면
		device->SetViewport(&viewport);

		// 렌더링 타겟 클리어
		device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0);
		
		DrawBackGround(device);
		DrawUFO(device);

		// 텍스처에 렌더링을 한 뒤, 렌더링 타겟을 원래의 프레임 버퍼로 복구.
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