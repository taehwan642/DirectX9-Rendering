#include "Draw.h"

namespace DrawTriangle
{
	LPDIRECT3DVERTEXBUFFER9 vb = nullptr;

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

	struct CUSTOMVERTEX
	{
		float x, y, z, rhw;
		DWORD color;
	};

	HRESULT Initialize(LPDIRECT3DDEVICE9 device)
	{
		CUSTOMVERTEX vertices[] =
		{
			//x, y, z, rhw, color
			{150.0f, 50.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0xff, 0x00, 0x00, 0xff)},
			{250.0f, 250.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0x00, 0xff, 0x00, 0xff)},
			{50.0f, 250.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0x00, 0x00, 0xff, 0xff)}
		};

		// 정점 버퍼 생성
		if (FAILED(device->CreateVertexBuffer(
			3 * sizeof(CUSTOMVERTEX),
			0,
			D3DFVF_CUSTOMVERTEX,
			D3DPOOL_DEFAULT,
			&vb,
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

		return S_OK;
	}

	void Update(LPDIRECT3DDEVICE9 device)
	{
		// 렌더링할 정점 버퍼 선택
		device->SetStreamSource(0,
			vb,
			0,
			sizeof(CUSTOMVERTEX)); // 정점 버퍼에 사용하는 데이터의 크기

		device->SetFVF(D3DFVF_CUSTOMVERTEX); // 플렉서블 정점 포맷 지정

		// 실제 렌더링
		device->DrawPrimitive(D3DPT_TRIANGLELIST, // 어떤 종류의 폴리곤을 출력할지 지정
			0,
			1); // 폴리곤 수
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
		SAFE_RELEASE(vb);
	}
}
