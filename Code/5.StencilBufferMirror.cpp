#include "DXUT.h"
#include "Draw.h"
namespace StencilBufferMirror
{
	const D3DXCOLOR      WHITE(D3DCOLOR_XRGB(255, 255, 255));
	const D3DXCOLOR      BLACK(D3DCOLOR_XRGB(0, 0, 0));
	const D3DXCOLOR        RED(D3DCOLOR_XRGB(255, 0, 0));
	const D3DXCOLOR      GREEN(D3DCOLOR_XRGB(0, 255, 0));
	const D3DXCOLOR       BLUE(D3DCOLOR_XRGB(0, 0, 255));
	const D3DXCOLOR     YELLOW(D3DCOLOR_XRGB(255, 255, 0));
	const D3DXCOLOR       CYAN(D3DCOLOR_XRGB(0, 255, 255));
	const D3DXCOLOR    MAGENTA(D3DCOLOR_XRGB(255, 0, 255));

	D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
	{
		D3DMATERIAL9 mtrl;
		mtrl.Ambient = a;
		mtrl.Diffuse = d;
		mtrl.Specular = s;
		mtrl.Emissive = e;
		mtrl.Power = p;
		return mtrl;
	}

	D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
	{
		D3DLIGHT9 light;
		::ZeroMemory(&light, sizeof(light));

		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Ambient = *color * 0.4f;
		light.Diffuse = *color;
		light.Specular = *color * 0.6f;
		light.Direction = *direction;

		return light;
	}

	const D3DMATERIAL9 WHITE_MTRL = InitMtrl(WHITE, WHITE, WHITE, BLACK, 2.0f);
	const D3DMATERIAL9 RED_MTRL = InitMtrl(RED, RED, RED, BLACK, 2.0f);
	const D3DMATERIAL9 GREEN_MTRL = InitMtrl(GREEN, GREEN, GREEN, BLACK, 2.0f);
	const D3DMATERIAL9 BLUE_MTRL = InitMtrl(BLUE, BLUE, BLUE, BLACK, 2.0f);
	const D3DMATERIAL9 YELLOW_MTRL = InitMtrl(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);

	IDirect3DVertexBuffer9* VB = 0;

	IDirect3DTexture9* FloorTex = 0;
	IDirect3DTexture9* WallTex = 0;
	IDirect3DTexture9* MirrorTex = 0;

	D3DMATERIAL9 FloorMtrl = WHITE_MTRL;
	D3DMATERIAL9 WallMtrl = WHITE_MTRL;
	D3DMATERIAL9 MirrorMtrl = WHITE_MTRL;

	ID3DXMesh* Teapot = 0;
	D3DXVECTOR3 TeapotPosition(0.0f, 3.0f, -7.5f);
	D3DMATERIAL9 TeapotMtrl = YELLOW_MTRL;

	struct Vertex
	{
		Vertex() {}
		Vertex(float x, float y, float z,
			float nx, float ny, float nz,
			float u, float v)
		{
			_x = x;  _y = y;  _z = z;
			_nx = nx; _ny = ny; _nz = nz;
			_u = u;  _v = v;
		}
		float _x, _y, _z;
		float _nx, _ny, _nz;
		float _u, _v;

		static const DWORD FVF;
	};
	const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

	HRESULT Initialize(LPDIRECT3DDEVICE9 device)
	{
		//
	// Make walls have low specular reflectance - 20%.
	//

		WallMtrl.Specular = WHITE * 0.2f;

		//
		// Create the teapot.
		//

		D3DXCreateTeapot(device, &Teapot, 0);

		//
		// Create and specify geometry.  For this sample we draw a floor
		// and a wall with a mirror on it.  We put the floor, wall, and
		// mirror geometry in one vertex buffer.
		//
		//   |----|----|----|
		//   |Wall|Mirr|Wall|
		//   |    | or |    |
		//   /--------------/
		//  /   Floor      /
		// /--------------/
		//
		device->CreateVertexBuffer(
			24 * sizeof(Vertex),
			0, // usage
			Vertex::FVF,
			D3DPOOL_MANAGED,
			&VB,
			0);

		Vertex* v = 0;
		VB->Lock(0, 0, (void**)&v, 0);

		// floor
		v[0] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
		v[1] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
		v[2] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

		v[3] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
		v[4] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
		v[5] = Vertex(7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

		// wall
		v[6] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[7] = Vertex(-7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
		v[8] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

		v[9] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[10] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
		v[11] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		// Note: We leave gap in middle of walls for mirror

		v[12] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[13] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
		v[14] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

		v[15] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[16] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
		v[17] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		// mirror
		v[18] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[19] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
		v[20] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

		v[21] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[22] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
		v[23] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		VB->Unlock();

		//
		// Load Textures, set filters.
		//

		D3DXCreateTextureFromFile(device, L"Source/checker.jpg", &FloorTex);
		D3DXCreateTextureFromFile(device, L"Source/brick0.jpg", &WallTex);
		D3DXCreateTextureFromFile(device, L"Source/ice.bmp", &MirrorTex);

		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		//
		// Lights.
		//

		D3DXVECTOR3 lightDir(0.707f, -0.707f, 0.707f);
		D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
		D3DLIGHT9 light = InitDirectionalLight(&lightDir, &color);

		device->SetLight(0, &light);
		device->LightEnable(0, true);

		device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		device->SetRenderState(D3DRS_SPECULARENABLE, true);

		//
		// Set Camera.
		//

		D3DXVECTOR3    pos(-10.0f, 3.0f, -15.0f);
		D3DXVECTOR3 target(0.0, 0.0f, 0.0f);
		D3DXVECTOR3     up(0.0f, 1.0f, 0.0f);

		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &pos, &target, &up);

		device->SetTransform(D3DTS_VIEW, &V);

		//
		// Set projection matrix.
		//
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI / 4.0f, // 45 - degree
			(float)1280 / (float)720,
			1.0f,
			1000.0f);
		device->SetTransform(D3DTS_PROJECTION, &proj);
		return S_OK;
	}

	void Update(LPDIRECT3DDEVICE9 device)
	{
		static float radius = 20.0f;
		static float lastTime = (float)timeGetTime();
		float currTime = (float)timeGetTime();
		float timeDelta = (currTime - lastTime) * 0.001f;


		if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
			TeapotPosition.x -= 3.0f * timeDelta;

		if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
			TeapotPosition.x += 3.0f * timeDelta;

		if (::GetAsyncKeyState(VK_UP) & 0x8000f)
			radius -= 2.0f * timeDelta;

		if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
			radius += 2.0f * timeDelta;


		static float angle = (3.0f * D3DX_PI) / 2.0f;

		if (::GetAsyncKeyState('A') & 0x8000f)
			angle -= 0.5f * timeDelta;

		if (::GetAsyncKeyState('S') & 0x8000f)
			angle += 0.5f * timeDelta;

		D3DXVECTOR3 position(cosf(angle) * radius, 3.0f, sinf(angle) * radius);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		device->SetTransform(D3DTS_VIEW, &V);

		DrawScene(device);
		DrawMirror(device);

		lastTime = currTime;
	}

	void DrawScene(LPDIRECT3DDEVICE9 device)
	{
		// draw teapot
		device->SetMaterial(&TeapotMtrl);
		device->SetTexture(0, 0);
		D3DXMATRIX W;
		D3DXMatrixTranslation(&W,
			TeapotPosition.x,
			TeapotPosition.y,
			TeapotPosition.z);

		device->SetTransform(D3DTS_WORLD, &W);
		Teapot->DrawSubset(0);

		D3DXMATRIX I;
		D3DXMatrixIdentity(&I);
		device->SetTransform(D3DTS_WORLD, &I);

		device->SetStreamSource(0, VB, 0, sizeof(Vertex));
		device->SetFVF(Vertex::FVF);

		// draw the floor
		device->SetMaterial(&FloorMtrl);
		device->SetTexture(0, FloorTex);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		// draw the walls
		device->SetMaterial(&WallMtrl);
		device->SetTexture(0, WallTex);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 4);

		// draw the mirror
		device->SetMaterial(&MirrorMtrl);
		device->SetTexture(0, MirrorTex);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);
	}

	void DrawMirror(LPDIRECT3DDEVICE9 device)
	{
		device->SetRenderState(D3DRS_STENCILENABLE, true);
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		device->SetRenderState(D3DRS_STENCILREF, 0x1);
		device->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
		device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
		device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

		// disable writes to the depth and back buffers
		device->SetRenderState(D3DRS_ZWRITEENABLE, false);
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		// draw the mirror to the stencil buffer
		device->SetStreamSource(0, VB, 0, sizeof(Vertex));
		device->SetFVF(Vertex::FVF);
		device->SetMaterial(&MirrorMtrl);
		device->SetTexture(0, MirrorTex);
		D3DXMATRIX I;
		D3DXMatrixIdentity(&I);
		device->SetTransform(D3DTS_WORLD, &I);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);

		// re-enable depth writes
		device->SetRenderState(D3DRS_ZWRITEENABLE, true);

		// only draw reflected teapot to the pixels where the mirror
		// was drawn to.
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

		// position reflection
		D3DXMATRIX W, T, R;
		D3DXPLANE plane(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
		D3DXMatrixReflect(&R, &plane);

		D3DXMatrixTranslation(&T,
			TeapotPosition.x,
			TeapotPosition.y,
			TeapotPosition.z);

		W = T * R;

		// clear depth buffer and blend the reflected teapot with the mirror
		device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

		// Finally, draw the reflected teapot
		device->SetTransform(D3DTS_WORLD, &W);
		device->SetMaterial(&TeapotMtrl);
		device->SetTexture(0, 0);

		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		Teapot->DrawSubset(0);

		// Restore render states.
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		device->SetRenderState(D3DRS_STENCILENABLE, false);
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}

	void Close()
	{
		VB->Release();
		FloorTex->Release();
		WallTex->Release();
		MirrorTex->Release();
		Teapot->Release();
	}
}
