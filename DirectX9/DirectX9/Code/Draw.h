#ifndef __draw_h__
#define __draw_h__

#include "stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>

namespace DrawTriangle
{
	HRESULT Initialize(LPDIRECT3DDEVICE9 device);
	void Update(LPDIRECT3DDEVICE9 device);
	void Close(LPDIRECT3DDEVICE9 device);
}

namespace DrawTexture
{
	HRESULT Initialize(LPDIRECT3DDEVICE9 device);
	void Update(LPDIRECT3DDEVICE9 device);
	void Close(LPDIRECT3DDEVICE9 device);
}

namespace DrawTextureWithStage
{
	HRESULT Initialize(LPDIRECT3DDEVICE9 device);
	void Update(LPDIRECT3DDEVICE9 device);
	void Close(LPDIRECT3DDEVICE9 device);
}

namespace DrawTextureAlpha
{
	HRESULT Initialize(LPDIRECT3DDEVICE9 device);
	void Update(LPDIRECT3DDEVICE9 device);
	void Close(LPDIRECT3DDEVICE9 device);
}

namespace StencilBufferMirror
{
	D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);
	D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);
	HRESULT Initialize(LPDIRECT3DDEVICE9 device);
	void Update(LPDIRECT3DDEVICE9 device);
	void DrawScene(LPDIRECT3DDEVICE9 device);
	void DrawMirror(LPDIRECT3DDEVICE9 device);
	void Close(LPDIRECT3DDEVICE9 device);
}

#endif __draw_h__