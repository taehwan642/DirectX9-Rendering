#ifndef __draw_h__
#define __draw_h__

#include <d3d9.h>
#include <d3dx9.h>

namespace DrawTriangle
{
	HRESULT Initialize(LPDIRECT3DDEVICE9 device);
	void Update(LPDIRECT3DDEVICE9 device);
	void Close();
}

namespace DrawTexture
{
	HRESULT Initialize(LPDIRECT3DDEVICE9 device);
	void Update(LPDIRECT3DDEVICE9 device);
	void Close();
}

namespace DrawTextureWithStage
{
	HRESULT Initialize(LPDIRECT3DDEVICE9 device);
	void Update(LPDIRECT3DDEVICE9 device);
	void Close();
}

namespace DrawTextureAlpha
{
	HRESULT Initialize(LPDIRECT3DDEVICE9 device);
	void Update(LPDIRECT3DDEVICE9 device);
	void Close();
}

namespace StencilBufferMirror
{
	D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);
	D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);
	HRESULT Initialize(LPDIRECT3DDEVICE9 device);
	void Update(LPDIRECT3DDEVICE9 device);
	void DrawScene(LPDIRECT3DDEVICE9 device);
	void DrawMirror(LPDIRECT3DDEVICE9 device);
	void Close();
}

namespace StencilBufferShadow
{
	D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);
	D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);
	HRESULT Initialize(LPDIRECT3DDEVICE9 device);
	void RenderScene(LPDIRECT3DDEVICE9 Device);
	void RenderMirror(LPDIRECT3DDEVICE9 Device);
	void RenderShadow(LPDIRECT3DDEVICE9 Device);
	void Update(LPDIRECT3DDEVICE9 device);
	void Close();
}

namespace XFile
{
	HRESULT Initialize(LPDIRECT3DDEVICE9 device);
	void SetMatrices(LPDIRECT3DDEVICE9 device);
	void Update(LPDIRECT3DDEVICE9 device);
	void Close();
}

#endif __draw_h__