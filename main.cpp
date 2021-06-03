#include "DXUT.h"
#include "resource.h"
#include "Draw.h"

#define CurrentNamespace DrawTriangle

BOOL IsDepthFormatOk(D3DFORMAT DepthFormat,
    D3DFORMAT AdapterFormat,
    D3DFORMAT BackBufferFormat)
{
    HRESULT hr = DXUTGetD3D9Object()->CheckDeviceFormat(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        AdapterFormat,
        D3DUSAGE_DEPTHSTENCIL,
        D3DRTYPE_SURFACE,
        DepthFormat);
    if (FAILED(hr)) return FALSE;

    hr = DXUTGetD3D9Object()->CheckDeviceFormat(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        AdapterFormat,
        D3DUSAGE_RENDERTARGET,
        D3DRTYPE_SURFACE,
        BackBufferFormat);
    if (FAILED(hr)) return FALSE;

    hr = DXUTGetD3D9Object()->CheckDepthStencilMatch(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        AdapterFormat,
        BackBufferFormat,
        DepthFormat);

    return SUCCEEDED(hr);
}


bool CALLBACK IsD3D9DeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
    bool bWindowed, void* pUserContext)
{
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    if (FAILED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,
        AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
        D3DRTYPE_TEXTURE, BackBufferFormat)))
        return false;


    if (!IsDepthFormatOk(D3DFMT_D24S8,
        AdapterFormat,
        BackBufferFormat) &&
        !IsDepthFormatOk(D3DFMT_D24X4S4,
            AdapterFormat,
            BackBufferFormat) &&
        !IsDepthFormatOk(D3DFMT_D15S1,
            AdapterFormat,
            BackBufferFormat) &&
        !IsDepthFormatOk(D3DFMT_D24FS8,
            AdapterFormat,
            BackBufferFormat))
        return false;

    return true;
}


bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
    if (IsDepthFormatOk(D3DFMT_D24S8,
        pDeviceSettings->d3d9.AdapterFormat,
        pDeviceSettings->d3d9.pp.BackBufferFormat))
        pDeviceSettings->d3d9.pp.AutoDepthStencilFormat = D3DFMT_D24S8;
    else if (IsDepthFormatOk(D3DFMT_D24X4S4,
        pDeviceSettings->d3d9.AdapterFormat,
        pDeviceSettings->d3d9.pp.BackBufferFormat))
        pDeviceSettings->d3d9.pp.AutoDepthStencilFormat = D3DFMT_D24X4S4;
    else if (IsDepthFormatOk(D3DFMT_D24FS8,
        pDeviceSettings->d3d9.AdapterFormat,
        pDeviceSettings->d3d9.pp.BackBufferFormat))
        pDeviceSettings->d3d9.pp.AutoDepthStencilFormat = D3DFMT_D24FS8;
    else if (IsDepthFormatOk(D3DFMT_D15S1,
        pDeviceSettings->d3d9.AdapterFormat,
        pDeviceSettings->d3d9.pp.BackBufferFormat))
        pDeviceSettings->d3d9.pp.AutoDepthStencilFormat = D3DFMT_D15S1;
    return true;
}


HRESULT CALLBACK OnD3D9CreateDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
    void* pUserContext)
{

    CurrentNamespace::Initialize(pd3dDevice);
    return S_OK;
}


HRESULT CALLBACK OnD3D9ResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
    void* pUserContext)
{
    return S_OK;
}


void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
}


void CALLBACK OnD3D9FrameRender(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
{
    HRESULT hr;

    V(pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0));

    if (SUCCEEDED(pd3dDevice->BeginScene()))
    {
        CurrentNamespace::Update(pd3dDevice);

        V(pd3dDevice->EndScene());
    }
}


LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    bool* pbNoFurtherProcessing, void* pUserContext)
{
    return 0;
}


void CALLBACK OnD3D9LostDevice(void* pUserContext)
{
}


void CALLBACK OnD3D9DestroyDevice(void* pUserContext)
{
    CurrentNamespace::Close();
}


INT WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    DXUTSetCallbackD3D9DeviceAcceptable(IsD3D9DeviceAcceptable);
    DXUTSetCallbackD3D9DeviceCreated(OnD3D9CreateDevice);
    DXUTSetCallbackD3D9DeviceReset(OnD3D9ResetDevice);
    DXUTSetCallbackD3D9FrameRender(OnD3D9FrameRender);
    DXUTSetCallbackD3D9DeviceLost(OnD3D9LostDevice);
    DXUTSetCallbackD3D9DeviceDestroyed(OnD3D9DestroyDevice);
    DXUTSetCallbackDeviceChanging(ModifyDeviceSettings);
    DXUTSetCallbackMsgProc(MsgProc);
    DXUTSetCallbackFrameMove(OnFrameMove);

    CGrowableArray <D3DFORMAT>* pDSFormats = DXUTGetD3D9Enumeration()->GetPossibleDepthStencilFormatList();
    pDSFormats->RemoveAll();
    pDSFormats->Add(D3DFMT_D24S8);
    pDSFormats->Add(D3DFMT_D24X4S4);
    pDSFormats->Add(D3DFMT_D15S1);
    pDSFormats->Add(D3DFMT_D24FS8);

    DXUTInit(true, true); 
    DXUTSetHotkeyHandling(true, true, true); 
    DXUTSetCursorSettings(true, true); 
    DXUTCreateWindow(L"DirectX9");
    DXUTCreateDevice(true, 1280, 720);

    DXUTMainLoop();

    return DXUTGetExitCode();
}


