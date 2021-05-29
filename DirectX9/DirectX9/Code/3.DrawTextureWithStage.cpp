#include "Draw.h"
namespace DrawTextureWithStage
{
	LPDIRECT3DTEXTURE9 texture = nullptr;
	LPDIRECT3DVERTEXBUFFER9 vb = nullptr;

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

	struct CUSTOMVERTEX
	{
		float x, y, z, rhw;
		DWORD color;
		float u, v;
	};

	HRESULT Initialize(LPDIRECT3DDEVICE9 device)
	{
		CUSTOMVERTEX vertices[] =
		{
			//x, y, z, rhw, color, u, v
			{0.0f, 0.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0xff, 0xff, 0xff, 0xff), 0, 0},
			{250.0f, 0.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0x00, 0x00, 0x00, 0xff),1, 0},
			{0.0f, 250.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0xff, 0xff, 0xff, 0xff), 0, 1},
			{250.0f, 250.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0x00, 0x00, 0x00, 0xff),1,1}
		};

		// 정점 버퍼 생성
		if (FAILED(device->CreateVertexBuffer(
			4 * sizeof(CUSTOMVERTEX), // 정점 버퍼 크기
			0, // 사용법
			D3DFVF_CUSTOMVERTEX, // 정점 포맷
			D3DPOOL_DEFAULT, // 메모리 클래스
			&vb, // 정점 버퍼 리소스 예약
			nullptr)))
		{
			return E_FAIL;
		}

		// 정점 버퍼 포인터를 vtx에 할당, vtx에다 vertices정보 복사
		void* vtx;
		if (FAILED(vb->Lock(0,
			sizeof(vertices),
			static_cast<void**>(&vtx),
			0
		)))
		{
			return E_FAIL;
		}
		memcpy(vtx, vertices, sizeof(vertices));
		vb->Unlock();

		if (FAILED(D3DXCreateTextureFromFile(device,
			"Source/earth.bmp", // 파일명
			&texture))) // 텍스쳐 오브젝트
		{
			return E_FAIL;
		}

		device->SetRenderState(D3DRS_DITHERENABLE, FALSE);
		device->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
		device->SetRenderState(D3DRS_ZENABLE, TRUE);
		device->SetRenderState(D3DRS_AMBIENT, 0x000F0F0F);

		// MODULATE
		// COLORARG1 * COLORARG2의 색을 합친 색을 출력.
		device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		// COLORARG1의 색은 Texture의 색으로 직접 설정했고,
		device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		// COLORARG2의 색은 폴리곤의 색으로 직접 설정했다.
		device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		return S_OK;
	}

	void Update(LPDIRECT3DDEVICE9 device)
	{
		device->SetTexture(0, // 멀티 텍스쳐를 위한 번호 (폴리곤에 여러 텍스쳐를 입히는 방법)
			texture);

		// 렌더링할 정점 버퍼 선택
		device->SetStreamSource(0,
			vb,
			0,
			sizeof(CUSTOMVERTEX)); // 정점 버퍼에 사용하는 데이터의 크기

		device->SetFVF(D3DFVF_CUSTOMVERTEX); // 플렉서블 정점 포맷 지정

		// 실제 렌더링
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, // 어떤 종류의 폴리곤을 출력할지 지정
			0,
			2); // 폴리곤 수
		/*
		D3DPT_POINTLIST             = 1, // 정열
		D3DPT_LINELIST              = 2, // 선
		D3DPT_LINESTRIP             = 3, // 선열
		D3DPT_TRIANGLELIST          = 4, // 삼각형
		D3DPT_TRIANGLESTRIP         = 5, // 띠
		D3DPT_TRIANGLEFAN           = 6, // 부채꼴
		*/
	}

	void Close(LPDIRECT3DDEVICE9 device)
	{
		SAFE_RELEASE(texture);
		SAFE_RELEASE(vb);
	}
}