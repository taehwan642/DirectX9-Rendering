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

		// ���� ���� ����
		if (FAILED(device->CreateVertexBuffer(
			4 * sizeof(CUSTOMVERTEX), // ���� ���� ũ��
			0, // ����
			D3DFVF_CUSTOMVERTEX, // ���� ����
			D3DPOOL_DEFAULT, // �޸� Ŭ����
			&vb, // ���� ���� ���ҽ� ����
			nullptr)))
		{
			return E_FAIL;
		}

		// ���� ���� �����͸� vtx�� �Ҵ�, vtx���� vertices���� ����
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
			L"Source/earth.bmp", // ���ϸ�
			&texture))) // �ؽ��� ������Ʈ
		{
			return E_FAIL;
		}
		if (FAILED(D3DXCreateTextureFromFile(device,
			L"Source/shadow.png", // ���ϸ�
			&texture2))) // �ؽ��� ������Ʈ
		{
			return E_FAIL;
		}

		// �ռ����� A * �������� ������ �� + B * �̹� �������� ������ ����
		device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		// �� �� A�� SRCBLEND, B�� DESTBLEND�̴�. (D3DRS_SRC/DESTBLEND)
		// SRCBLEND�� ���� �ִ°� �ٷ� SetRenderState(D3DRS_SRCBLEND, ��A);
		// DESTBLEND�� ����. ���� ��B�� �ٲ�°� ��

		// MODULATE, ��� ���� ���� �ռ��� ���� ������ ������� ����
		// ALPHAARG1 * ALPHAARG2 �� ���� ��ģ ���� ���.
		device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		// ALPHAARG1�� ���� Texture�� ������ ���� �����߰�,
		device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		// ALPHAARG2�� ���� �������� ������ ���� �����ߴ�.
		device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

#if RenderType == 0 // ���� �ռ� C = Cd(1 - As) + CsAs (���� �� = 1 - a * �ٴ� �� + a * ���� ��)
		// �������� �������Ϸ��� �������� �ռ� ����, Source�� ���ķ� �̷���� Vec4 (As, As, As, As)
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		// �̹� �������� ������ Ÿ���� �ռ� ����, Source�� ���Ŀ� 1 - �� �� Vec4 (1 - As, 1 - As, 1 - As, 1 - As)
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

#elif RenderType == 1 // ���� �ռ� C = Cd + CsAs (���� �� = �ٴ� �� + a * ���� ��)
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE); // 1, 1, 1, 1

#elif RenderType == 2 // ���� �ռ� C = Cd - CsAs (���� �� = �ٴ� �� - a * ���� ��)
		// �ռ����� -A * �������� ������ �� + B * �̹� �������� ������ ����
		device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT); 
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

#elif RenderType == 3 // ���� �ռ� C = Cd * Cs (���� �� = �ٴ� �� * ���� ��)
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO); // 0, 0, 0, 0
		// Rs, Gs, Bs, As (�������� �������Ϸ��� �������� ��)
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR); 

#elif RenderType == 4 // ���� �ռ� (���� �� = �ٴ� �� * �ٴ� ��)
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		// Rd, Gd, Bd, Ad (���� Ÿ���� ����)
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR);

#elif RenderType == 5 // �װ�Ƽ�� ����Ƽ�� ���� �ռ� (���� �� = (1 - �ٴ� ��) * ���� ��)
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
			sizeof(CUSTOMVERTEX)); // ���� ���ۿ� ����ϴ� �������� ũ��

		device->SetFVF(D3DFVF_CUSTOMVERTEX); // �÷����� ���� ���� ����
		// �������� ���� ���� ����
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE); // ���ĺ��� ���
		device->SetTexture(0, // ��Ƽ �ؽ��ĸ� ���� ��ȣ (�����￡ ���� �ؽ��ĸ� ������ ���)
			texture);
		// ���� ������
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, // � ������ �������� ������� ����
			0,
			2); // ������ ��

		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); // ���ĺ��� ���
		device->SetTexture(0, // ��Ƽ �ؽ��ĸ� ���� ��ȣ (�����￡ ���� �ؽ��ĸ� ������ ���)
			texture2);
		// ���� ������

		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, // � ������ �������� ������� ����
			0,
			2); // ������ ��

		/*
		D3DPT_POINTLIST             = 1, // ����
		D3DPT_LINELIST              = 2, // ��
		D3DPT_LINESTRIP             = 3, // ����
		D3DPT_TRIANGLELIST          = 4, // �ﰢ��
		D3DPT_TRIANGLESTRIP         = 5, // ��
		D3DPT_TRIANGLEFAN           = 6, // ��ä��
		*/
	}

	void Close()
	{
		SAFE_RELEASE(texture);
		SAFE_RELEASE(texture2);
		SAFE_RELEASE(vb);
	}
}