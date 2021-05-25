#include <iostream>
#include <Windows.h>
#include "Code/Draw.h"

HWND MainWindowHandle = 0;
LPDIRECT3D9 g_pD3D{ nullptr };
LPDIRECT3DDEVICE9 g_pD3Ddev{ nullptr };

#define CurrentNamespace DrawTextureAlpha

HRESULT InitD3D(HWND hWnd)
{
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION); // Direct3D 사용
	if (g_pD3D == nullptr)
		return E_FAIL;

	D3DDISPLAYMODE d3ddm;
	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm))) // 디스플레이 모드 조사
		return E_FAIL;

	// 디바이스 생성
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.Windowed = TRUE; // 윈도우 모드
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // 화면 전환 방법
	d3dpp.BackBufferFormat = d3ddm.Format; // 화면 포맷을 현재의 화면과 같게

	if (FAILED(g_pD3D->CreateDevice(
		D3DADAPTER_DEFAULT, // 여러 개의 비디오 카드가 있을 때, 선택하기 위한 용도
		D3DDEVTYPE_HAL, // 셰이딩을 하드웨어에서 할 것인가? (HAL = 사용/REF = 미사용)
		hWnd, // 윈도우 핸들
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, // Direct3D의 전반적인 동작 제어 (HARDWARE = 사용/SOFTWARE = 미사용)
		&d3dpp, // d3dpp
		&g_pD3Ddev)))  // 생성될 디바이스
		return E_FAIL;

	if (FAILED(CurrentNamespace::Initialize(g_pD3Ddev)))
		return E_FAIL;

	return S_OK;
}

void Render(void)
{
	// 배경을 검게 칠한다
	g_pD3Ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0);

	g_pD3Ddev->BeginScene(); // 렌더링 시작

	// 함수 추가
	CurrentNamespace::Update(g_pD3Ddev);

	g_pD3Ddev->EndScene(); // 렌더링 종료

	g_pD3Ddev->Present(NULL, NULL, NULL, NULL); // 렌더링한것들을 실제 윈도우에 출력
}

void CleanUp(void)
{
	CurrentNamespace::Close(g_pD3Ddev);
	SAFE_RELEASE(g_pD3Ddev);
	SAFE_RELEASE(g_pD3D);
}

int Run()
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			Render();
		}
	}
	
	CleanUp();
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND windowHandle,
						 UINT msg,
						 WPARAM wParam,
						 LPARAM lParam)
{
	PAINTSTRUCT paint;
	HDC hdc;
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		::MessageBox(0, "HELLO!!", "asdf", MB_OK);
		return 0;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			::DestroyWindow(MainWindowHandle);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(MainWindowHandle, &paint);
		EndPaint(MainWindowHandle, &paint);
		return 0;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(windowHandle, msg, wParam, lParam);
}

bool InitWindowsApp(HINSTANCE instanceHandle, int show)
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW; // 윈도우 클래스 스타일
	wc.lpfnWndProc = WndProc; // 윈도우 프로시져
	wc.cbClsExtra = 0; // 확장용
	wc.cbWndExtra = 0; // 확장용
	wc.hInstance = instanceHandle; // 인스턴스 핸들
	wc.hIcon = ::LoadIcon(0, IDI_APPLICATION); // 아이콘 핸들
	wc.hCursor = ::LoadCursor(0, IDC_ARROW); // 커서 핸들
	wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH)); // 배경 브러시 핸들
	wc.lpszMenuName = 0; // 메뉴 이름
	wc.lpszClassName = "asdf1"; // 윈도우 클래스 이름

	::RegisterClass(&wc);

	MainWindowHandle = ::CreateWindow(
		"asdf123", // 클래스명
		"dd", // 윈도우 이름
		WS_OVERLAPPEDWINDOW, // 윈도우 스타일
		CW_USEDEFAULT, // 윈도우의 최초 출력 x 좌표
		CW_USEDEFAULT, // 윈도우의 최초 출력 y 좌표
		CW_USEDEFAULT, // 윈도우의 폭
		CW_USEDEFAULT, // 윈도우의 높이
		0, // 부모 윈도우 핸들
		0, // 메뉴 핸들, 혹은 자식 위도우 ID
		instanceHandle, // 인스턴스 핸들
		0); // 윈도우 생성 데이터

	if (FAILED(InitD3D(MainWindowHandle)))
		return false;

	::ShowWindow(MainWindowHandle, show);
	::UpdateWindow(MainWindowHandle);
	return true;
}



int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   PSTR pCmdLine,
				   int nShowCmd)
{
	if (!InitWindowsApp(hInstance, nShowCmd))
	{
		::MessageBox(MainWindowHandle, "BOOM", "INIT WINDOW FAIL", MB_OK);
		return WM_QUIT;
	}

	return Run();
}