#include "DXUT.h"
#include "Draw.h"
namespace DrawTexture
{
	LPDIRECT3DTEXTURE9 texture = nullptr;
	LPDIRECT3DVERTEXBUFFER9 vb = nullptr;

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_TEX1)

	struct CUSTOMVERTEX
	{
		float x, y, z, rhw;
		float u, v;
	};

	HRESULT Initialize(LPDIRECT3DDEVICE9 device)
	{
		CUSTOMVERTEX vertices[] =
		{
			//x, y, z, rhw, color
			{0.0f, 0.0f, 0.5f, 1.0f, 0, 0},//D3DCOLOR_RGBA(0x00, 0x00, 0xff, 0xff)},
			{250.0f, 0.0f, 0.5f, 1.0f, 1, 0},//D3DCOLOR_RGBA(0xff, 0x00, 0x00, 0xff)},
			{0.0f, 250.0f, 0.5f, 1.0f, 0, 1},//D3DCOLOR_RGBA(0x00, 0xff, 0xff, 0xff)},
			{250.0f, 250.0f, 0.5f, 1.0f,1,1}// D3DCOLOR_RGBA(0x00, 0xff, 0x00, 0xff)}
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
			L"Source/earth.bmp", // 파일명
			&texture))) // 텍스쳐 오브젝트
		{
			return E_FAIL;
		}

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

	void Close()
	{
		SAFE_RELEASE(texture);
		SAFE_RELEASE(vb);
	}
}