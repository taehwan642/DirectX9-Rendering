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
			"Source/earth.bmp", // ���ϸ�
			&texture))) // �ؽ��� ������Ʈ
		{
			return E_FAIL;
		}

		device->SetRenderState(D3DRS_DITHERENABLE, FALSE);
		device->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
		device->SetRenderState(D3DRS_ZENABLE, TRUE);
		device->SetRenderState(D3DRS_AMBIENT, 0x000F0F0F);

		// MODULATE
		// COLORARG1 * COLORARG2�� ���� ��ģ ���� ���.
		device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		// COLORARG1�� ���� Texture�� ������ ���� �����߰�,
		device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		// COLORARG2�� ���� �������� ������ ���� �����ߴ�.
		device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		return S_OK;
	}

	void Update(LPDIRECT3DDEVICE9 device)
	{
		device->SetTexture(0, // ��Ƽ �ؽ��ĸ� ���� ��ȣ (�����￡ ���� �ؽ��ĸ� ������ ���)
			texture);

		// �������� ���� ���� ����
		device->SetStreamSource(0,
			vb,
			0,
			sizeof(CUSTOMVERTEX)); // ���� ���ۿ� ����ϴ� �������� ũ��

		device->SetFVF(D3DFVF_CUSTOMVERTEX); // �÷����� ���� ���� ����

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

	void Close(LPDIRECT3DDEVICE9 device)
	{
		SAFE_RELEASE(texture);
		SAFE_RELEASE(vb);
	}
}