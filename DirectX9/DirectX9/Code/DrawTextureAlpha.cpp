#include "Draw.h"
namespace DrawTextureAlpha
{
	LPDIRECT3DTEXTURE9 texture = nullptr;
	LPDIRECT3DTEXTURE9 texture2 = nullptr;
	LPDIRECT3DVERTEXBUFFER9 vb = nullptr;
	LPDIRECT3DVERTEXBUFFER9 vb2 = nullptr;

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
			{50.0f, 50.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0xff, 0xff, 0xff, 0x80), 0, 0},
			{250.0f, 50.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0xff, 0xff, 0xff, 0x80),1, 0},
			{50.0f, 250.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0xff, 0xff, 0xff, 0x80), 0, 1},
			{250.0f, 250.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0xff, 0xff, 0xff, 0x80),1,1}
		};

		CUSTOMVERTEX vertices2[] =
		{
			//x, y, z, rhw, color, u, v
			{100.0f, 100.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0xff, 0xff, 0xff, 0xff), 0, 0},
			{300.0f, 100.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0xff, 0xff, 0xff, 0xff),1, 0},
			{100.0f, 300.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0xff, 0xff, 0xff, 0xff), 0, 1},
			{300.0f, 300.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0xff, 0xff, 0xff, 0xff),1,1}
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

		if (FAILED(device->CreateVertexBuffer(
			4 * sizeof(CUSTOMVERTEX), // ���� ���� ũ��
			0, // ����
			D3DFVF_CUSTOMVERTEX, // ���� ����
			D3DPOOL_DEFAULT, // �޸� Ŭ����
			&vb2, // ���� ���� ���ҽ� ����
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

		void* vtx2;
		if (FAILED(vb2->Lock(0,
			sizeof(vertices2),
			static_cast<void**>(&vtx2),
			0
		)))
		{
			return E_FAIL;
		}
		memcpy(vtx2, vertices2, sizeof(vertices2));
		vb2->Unlock();

		if (FAILED(D3DXCreateTextureFromFile(device,
			"Source/earth.bmp", // ���ϸ�
			&texture))) // �ؽ��� ������Ʈ
		{
			return E_FAIL;
		}
		if (FAILED(D3DXCreateTextureFromFile(device,
			"Source/explosion.png", // ���ϸ�
			&texture2))) // �ؽ��� ������Ʈ
		{
			return E_FAIL;
		}

		// ������ ON
		//device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); // ���ĺ��� ���
		device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD); 
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVDESTALPHA);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

		// MODULATE
		// COLORARG1 + COLORARG2�� ���� ��ģ ���� ���.
		device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		// COLORARG1�� ���� Texture�� ������ ���� �����߰�,
		device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		// COLORARG2�� ���� �������� ������ ���� �����ߴ�.
		device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		return S_OK;
	}

	void Update(LPDIRECT3DDEVICE9 device)
	{
		// �������� ���� ���� ����
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE); // ���ĺ��� ���
		device->SetTexture(0, // ��Ƽ �ؽ��ĸ� ���� ��ȣ (�����￡ ���� �ؽ��ĸ� ������ ���)
			texture);
		// ���� ������
		device->SetStreamSource(0,
			vb2,
			0,
			sizeof(CUSTOMVERTEX)); // ���� ���ۿ� ����ϴ� �������� ũ��

		device->SetFVF(D3DFVF_CUSTOMVERTEX); // �÷����� ���� ���� ����
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, // � ������ �������� ������� ����
			0,
			2); // ������ ��
	
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); // ���ĺ��� ���
		device->SetTexture(0, // ��Ƽ �ؽ��ĸ� ���� ��ȣ (�����￡ ���� �ؽ��ĸ� ������ ���)
			texture2);
		// ���� ������
		device->SetStreamSource(0,
			vb,
			0,
			sizeof(CUSTOMVERTEX)); // ���� ���ۿ� ����ϴ� �������� ũ��

		device->SetFVF(D3DFVF_CUSTOMVERTEX); // �÷����� ���� ���� ����
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