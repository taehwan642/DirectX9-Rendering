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

		// ���� ���� ����
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

		return S_OK;
	}

	void Update(LPDIRECT3DDEVICE9 device)
	{
		// �������� ���� ���� ����
		device->SetStreamSource(0,
			vb,
			0,
			sizeof(CUSTOMVERTEX)); // ���� ���ۿ� ����ϴ� �������� ũ��

		device->SetFVF(D3DFVF_CUSTOMVERTEX); // �÷����� ���� ���� ����

		// ���� ������
		device->DrawPrimitive(D3DPT_TRIANGLELIST, // � ������ �������� ������� ����
			0,
			1); // ������ ��
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
		SAFE_RELEASE(vb);
	}
}
