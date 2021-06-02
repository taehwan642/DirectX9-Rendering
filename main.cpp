#include "DXUT.h"
#include "resource.h"
#include "Draw.h"

#define CurrentNamespace DrawTriangle

BOOL IsDepthFormatOk(D3DFORMAT DepthFormat,
    D3DFORMAT AdapterFormat,
    D3DFORMAT BackBufferFormat)
{
    // Verify that the depth format exists
    HRESULT hr = DXUTGetD3D9Object()->CheckDeviceFormat(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        AdapterFormat,
        D3DUSAGE_DEPTHSTENCIL,
        D3DRTYPE_SURFACE,
        DepthFormat);
    if (FAILED(hr)) return FALSE;

    // Verify that the backbuffer format is valid
    hr = DXUTGetD3D9Object()->CheckDeviceFormat(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        AdapterFormat,
        D3DUSAGE_RENDERTARGET,
        D3DRTYPE_SURFACE,
        BackBufferFormat);
    if (FAILED(hr)) return FALSE;

    // Verify that the depth format is compatible
    hr = DXUTGetD3D9Object()->CheckDepthStencilMatch(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        AdapterFormat,
        BackBufferFormat,
        DepthFormat);

    return SUCCEEDED(hr);
}


//--------------------------------------------------------------------------------------
// Rejects any D3D9 devices that aren't acceptable to the app by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D9DeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
    bool bWindowed, void* pUserContext)
{
    // Typically want to skip back buffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    if (FAILED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,
        AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
        D3DRTYPE_TEXTURE, BackBufferFormat)))
        return false;


    // Must support stencil buffer
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


//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
    // This sample requires a stencil buffer.
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


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
// and aren't tied to the back buffer size
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9CreateDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
    void* pUserContext)
{

    CurrentNamespace::Initialize(pd3dDevice);
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
// or that are tied to the back buffer size 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9ResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
    void* pUserContext)
{
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{

}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9FrameRender(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
{
    HRESULT hr;

  
    // Clear the render target and the zbuffer 
    V(pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0));

    // Render the scene
    if (SUCCEEDED(pd3dDevice->BeginScene()))
    {
        CurrentNamespace::Update(pd3dDevice);

        V(pd3dDevice->EndScene());
    }
}




//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    bool* pbNoFurtherProcessing, void* pUserContext)
{
    return 0;
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9ResetDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9LostDevice(void* pUserContext)
{
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9CreateDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9DestroyDevice(void* pUserContext)
{
    CurrentNamespace::Close();
}


//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    // Set the callback functions
    DXUTSetCallbackD3D9DeviceAcceptable(IsD3D9DeviceAcceptable);
    DXUTSetCallbackD3D9DeviceCreated(OnD3D9CreateDevice);
    DXUTSetCallbackD3D9DeviceReset(OnD3D9ResetDevice);
    DXUTSetCallbackD3D9FrameRender(OnD3D9FrameRender);
    DXUTSetCallbackD3D9DeviceLost(OnD3D9LostDevice);
    DXUTSetCallbackD3D9DeviceDestroyed(OnD3D9DestroyDevice);
    DXUTSetCallbackDeviceChanging(ModifyDeviceSettings);
    DXUTSetCallbackMsgProc(MsgProc);
    DXUTSetCallbackFrameMove(OnFrameMove);

    // This sample requires a stencil buffer. See the callback function for details.
    CGrowableArray <D3DFORMAT>* pDSFormats = DXUTGetD3D9Enumeration()->GetPossibleDepthStencilFormatList();
    pDSFormats->RemoveAll();
    pDSFormats->Add(D3DFMT_D24S8);
    pDSFormats->Add(D3DFMT_D24X4S4);
    pDSFormats->Add(D3DFMT_D15S1);
    pDSFormats->Add(D3DFMT_D24FS8);

    // TODO: Perform any application-level initialization here

    // Initialize DXUT and create the desired Win32 window and Direct3D device for the application
    DXUTInit(true, true); // Parse the command line and show msgboxes
    DXUTSetHotkeyHandling(true, true, true);  // handle the default hotkeys
    DXUTSetCursorSettings(true, true); // Show the cursor and clip it when in full screen
    DXUTCreateWindow(L"DirectX9");
    DXUTCreateDevice(true, 1280, 720);

    // Start the render loop
    DXUTMainLoop();

    // TODO: Perform any application-level cleanup here

    return DXUTGetExitCode();
}


