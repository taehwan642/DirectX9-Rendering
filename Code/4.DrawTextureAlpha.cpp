#include "DXUT.h"
#include "Draw.h"
namespace DrawTextureAlpha
{
	LPDIRECT3DTEXTURE9 texture = nullptr;
	LPDIRECT3DTEXTURE9 texture2 = nullptr;
	LPDIRECT3DVERTEXBUFFER9 vb = nullptr;

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define RenderType 0

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
			{50.0f, 50.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0xff, 0xff, 0xff, 0xdd), 0, 0},
			{250.0f, 50.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0xff, 0xff, 0xff, 0xdd),1, 0},
			{50.0f, 250.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0xff, 0xff, 0xff, 0xdd), 0, 1},
			{250.0f, 250.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0xff, 0xff, 0xff, 0xdd),1,1}
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
		if (FAILED(D3DXCreateTextureFromFile(device,
			L"Source/shadow.png", // 파일명
			&texture2))) // 텍스쳐 오브젝트
		{
			return E_FAIL;
		}

		// 합성법을 A * 렌더링할 폴리곤 색 + B * 이미 렌더링된 색으로 변경
		device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		// 이 때 A는 SRCBLEND, B는 DESTBLEND이다. (D3DRS_SRC/DESTBLEND)
		// SRCBLEND에 값을 넣는게 바로 SetRenderState(D3DRS_SRCBLEND, 값A);
		// DESTBLEND도 동일. 단지 값B로 바뀌는것 뿐

		// MODULATE, 어느 알파 값을 합성의 알파 값으로 사용할지 결정
		// ALPHAARG1 * ALPHAARG2 의 색을 합친 색을 출력.
		device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		// ALPHAARG1의 색은 Texture의 색으로 직접 설정했고,
		device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		// ALPHAARG2의 색은 폴리곤의 색으로 직접 설정했다.
		device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

#if RenderType == 0 // 선형 합성 C = Cd(1 - As) + CsAs (최종 색 = 1 - a * 바닥 색 + a * 덮일 색)
		// 이제부터 렌더링하려는 폴리곤의 합성 강도, Source의 알파로 이루어진 Vec4 (As, As, As, As)
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		// 이미 렌더링된 렌더링 타겟의 합성 강도, Source의 알파에 1 - 을 한 Vec4 (1 - As, 1 - As, 1 - As, 1 - As)
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

#elif RenderType == 1 // 덧셈 합성 C = Cd + CsAs (최종 색 = 바닥 색 + a * 덮일 색)
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE); // 1, 1, 1, 1

#elif RenderType == 2 // 뺄셈 합성 C = Cd - CsAs (최종 색 = 바닥 색 - a * 덮일 색)
		// 합성법을 -A * 렌더링할 폴리곤 색 + B * 이미 렌더링된 색으로 변경
		device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT); 
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

#elif RenderType == 3 // 곱셈 합성 C = Cd * Cs (최종 색 = 바닥 색 * 덮일 색)
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO); // 0, 0, 0, 0
		// Rs, Gs, Bs, As (이제부터 렌더링하려는 폴리곤의 색)
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR); 

#elif RenderType == 4 // 제곱 합성 (최종 색 = 바닥 색 * 바닥 색)
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		// Rd, Gd, Bd, Ad (렌더 타겟의 색상)
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR);

#elif RenderType == 5 // 네거티브 포지티브 반전 합성 (최종 색 = (1 - 바닥 색) * 덮일 색)
		// 1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
#endif
		return S_OK;
	}

	void Update(LPDIRECT3DDEVICE9 device)
	{
		device->SetStreamSource(0,
			vb,
			0,
			sizeof(CUSTOMVERTEX)); // 정점 버퍼에 사용하는 데이터의 크기

		device->SetFVF(D3DFVF_CUSTOMVERTEX); // 플렉서블 정점 포맷 지정
		// 렌더링할 정점 버퍼 선택
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE); // 알파블렌딩 사용
		device->SetTexture(0, // 멀티 텍스쳐를 위한 번호 (폴리곤에 여러 텍스쳐를 입히는 방법)
			texture);
		// 실제 렌더링
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, // 어떤 종류의 폴리곤을 출력할지 지정
			0,
			2); // 폴리곤 수

		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); // 알파블렌딩 사용
		device->SetTexture(0, // 멀티 텍스쳐를 위한 번호 (폴리곤에 여러 텍스쳐를 입히는 방법)
			texture2);
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
		SAFE_RELEASE(texture2);
		SAFE_RELEASE(vb);
	}
}