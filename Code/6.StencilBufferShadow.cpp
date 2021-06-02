#include "DXUT.h"
#include "Draw.h"

namespace StencilBufferShadow
{
    const D3DXCOLOR      WHITE(D3DCOLOR_XRGB(255, 255, 255));
    const D3DXCOLOR      BLACK(D3DCOLOR_XRGB(0, 0, 0));
    const D3DXCOLOR        RED(D3DCOLOR_XRGB(255, 0, 0));
    const D3DXCOLOR      GREEN(D3DCOLOR_XRGB(0, 255, 0));
    const D3DXCOLOR       BLUE(D3DCOLOR_XRGB(0, 0, 255));
    const D3DXCOLOR     YELLOW(D3DCOLOR_XRGB(255, 255, 0));
    const D3DXCOLOR       CYAN(D3DCOLOR_XRGB(0, 255, 255));
    const D3DXCOLOR    MAGENTA(D3DCOLOR_XRGB(255, 0, 255));

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


    const D3DMATERIAL9 WHITE_MTRL = InitMtrl(WHITE, WHITE, WHITE, BLACK, 2.0f);
    const D3DMATERIAL9 RED_MTRL = InitMtrl(RED, RED, RED, BLACK, 2.0f);
    const D3DMATERIAL9 GREEN_MTRL = InitMtrl(GREEN, GREEN, GREEN, BLACK, 2.0f);
    const D3DMATERIAL9 BLUE_MTRL = InitMtrl(BLUE, BLUE, BLUE, BLACK, 2.0f);
    const D3DMATERIAL9 YELLOW_MTRL = InitMtrl(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);

    const int Width = 640;
    const int Height = 480;

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

    void RenderScene(LPDIRECT3DDEVICE9 Device)
    {
        // draw teapot
        Device->SetMaterial(&TeapotMtrl);
        Device->SetTexture(0, 0);
        D3DXMATRIX W;
        D3DXMatrixTranslation(&W,
            TeapotPosition.x,
            TeapotPosition.y,
            TeapotPosition.z);

        Device->SetTransform(D3DTS_WORLD, &W);
        Teapot->DrawSubset(0);

        D3DXMATRIX I;
        D3DXMatrixIdentity(&I);
        Device->SetTransform(D3DTS_WORLD, &I);

        Device->SetStreamSource(0, VB, 0, sizeof(Vertex));
        Device->SetFVF(Vertex::FVF);

        // draw the floor
        Device->SetMaterial(&FloorMtrl);
        Device->SetTexture(0, FloorTex);
        Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

        // draw the walls
        Device->SetMaterial(&WallMtrl);
        Device->SetTexture(0, WallTex);
        Device->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 4);

        // draw the mirror
        Device->SetMaterial(&MirrorMtrl);
        Device->SetTexture(0, MirrorTex);
        Device->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);
    }

    void RenderMirror(LPDIRECT3DDEVICE9 Device)
    {
        //
        // Draw Mirror quad to stencil buffer ONLY.  In this way
        // only the stencil bits that correspond to the mirror will
        // be on.  Therefore, the reflected teapot can only be rendered
        // where the stencil bits are turned on, and thus on the mirror 
        // only.
        //

        Device->SetRenderState(D3DRS_STENCILENABLE, true); // 스텐실 허용
        Device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS); // 스텐실 기능을 항상 성공하게
        Device->SetRenderState(D3DRS_STENCILREF, 0x1); // 참조값 1, 전부 반사
        Device->SetRenderState(D3DRS_STENCILMASK, 0xffffffff); // 마스크 무시, 전부 1이면 그냥 값이 나오므로 무시와 같다.
        Device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff); // 쓰는 마스크도 무시
        Device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP); // 깊이 테스트가 실패하면 스텐실 버퍼 항목을 변경하지 않는다.
        Device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP); // 스텐실을 실패하면 스텐실 버퍼 항목을 변경하지 않는다.
        Device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE); // 스텐실을 패스하면 스텐실 버퍼 항목을 참조값으로 대체한다.

        // disable writes to the depth and back buffers
        Device->SetRenderState(D3DRS_ZWRITEENABLE, false); // 깊이버퍼 작성 해제
        Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true); // 알파블렌딩 허용
        Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO); // 
        Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE); // 

        // draw the mirror to the stencil buffer
        // 거울 오브젝트를 스텐실 버퍼 영역에 그려주는데, 계산을 통해
        // 그려지는 족족 1로 계산되어서 결국 거울 영역만큼의 스텐실 버퍼 픽셀 부분이 1값으로 그려지게 된다.
        Device->SetStreamSource(0, VB, 0, sizeof(Vertex));
        Device->SetFVF(Vertex::FVF);
        Device->SetMaterial(&MirrorMtrl);
        Device->SetTexture(0, MirrorTex);
        D3DXMATRIX I;
        D3DXMatrixIdentity(&I);
        Device->SetTransform(D3DTS_WORLD, &I);
        Device->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);

        // re-enable depth writes
        Device->SetRenderState(D3DRS_ZWRITEENABLE, true);

        // only draw reflected teapot to the pixels where the mirror
        // was drawn to.
        Device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
        Device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

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
        Device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
        Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

        // Finally, draw the reflected teapot
        Device->SetTransform(D3DTS_WORLD, &W);
        Device->SetMaterial(&TeapotMtrl);
        Device->SetTexture(0, 0);

        Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
        Teapot->DrawSubset(0);

        // Restore render states.
        Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
        Device->SetRenderState(D3DRS_STENCILENABLE, false);
        Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    }

    D3DXVECTOR4 lightDirection(0.707f, -0.707f, 0.707f, 0.0f);

    void RenderShadow(LPDIRECT3DDEVICE9 Device)
    {
        Device->SetRenderState(D3DRS_STENCILENABLE, true);
        Device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
        // STENCILREF 상태를 0x0으로 지정해 스텐실 버퍼 내의 대응되는 항목이 0x0인 경우에만 후면 버퍼에
        // 그림자를 렌더링하도록 했다
        Device->SetRenderState(D3DRS_STENCILREF, 0x0);
        Device->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
        Device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
        Device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
        Device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
        // 스텐실 버퍼는 0(0x0)으로 소거되어 있으므로 처음으로 특정 픽셀을 쓸 때는 항상 성공하지만
        // STENCILPASS가 INCR로 지정되어 있기에 이미 쓰여진 픽셀을 쓰려고 하면 테스트가 실패한다.
        // 더블 블렌딩 현상 제거
        Device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);

        // 그림자 변환을 계산하고 장면 내의 적절한 위치로 그림자 이동
        // 주전자를 그림자로 압축하기 위한 변환 계산

        D3DXPLANE groundPlane(0.0f, -1.0f, 0.0f, 0.0f);
        D3DXMATRIX S;
        D3DXMatrixShadow(&S, &lightDirection, &groundPlane);
        D3DXMATRIX T;
        D3DXMatrixTranslation(&T, TeapotPosition.x, TeapotPosition.y, TeapotPosition.z);
        D3DXMATRIX W = T * S;
        Device->SetTransform(D3DTS_WORLD, &W);

        Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
        Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

        D3DMATERIAL9 mtrl = InitMtrl(BLACK, BLACK, BLACK, BLACK, 0.0f);
        mtrl.Diffuse.a = 0.5f; // 50% 투명도

        // 바닥에 그림자를 렌더링할 때 z 쟁탈이 일어나지 않도록
        // 깊이 버퍼를 끈다
        Device->SetRenderState(D3DRS_ZENABLE, false);

        Device->SetMaterial(&mtrl);
        Device->SetTexture(0, 0);
        Teapot->DrawSubset(0);

        Device->SetRenderState(D3DRS_ZENABLE, true);

        Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
        Device->SetRenderState(D3DRS_STENCILENABLE, false);
    }


	void Update(LPDIRECT3DDEVICE9 device)
	{
        float timeDelta = DXUTGetElapsedTime();
        static float radius = 20.0f;

        if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
            TeapotPosition.x -= 3.0f * timeDelta;

        if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
            TeapotPosition.x += 3.0f * timeDelta;

        if (::GetAsyncKeyState(VK_UP) & 0x8000f)
            TeapotPosition.y += 3.0f * timeDelta;

        if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
            TeapotPosition.y -= 3.0f * timeDelta;


        static float angle = (3.0f * D3DX_PI) / 2.0f;

        if (::GetAsyncKeyState('z') & 0x8000f)
            angle -= 0.5f * timeDelta;

        if (::GetAsyncKeyState('X') & 0x8000f)
            angle += 0.5f * timeDelta;



        if (::GetAsyncKeyState('W') & 0x8000f)
            lightDirection.z += 0.5 * timeDelta;

        if (::GetAsyncKeyState('S') & 0x8000f)
            lightDirection.z -= 0.5 * timeDelta;

        if (::GetAsyncKeyState('A') & 0x8000f)
            lightDirection.x -= 0.5 * timeDelta;

        if (::GetAsyncKeyState('D') & 0x8000f)
            lightDirection.x += 0.5 * timeDelta;


        D3DXVECTOR3 position(cosf(angle) * radius, 3.0f, sinf(angle) * radius);
        D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
        D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
        D3DXMATRIX V;
        D3DXMatrixLookAtLH(&V, &position, &target, &up);
        device->SetTransform(D3DTS_VIEW, &V);

        RenderScene(device);
        RenderMirror(device);
        RenderShadow(device);
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