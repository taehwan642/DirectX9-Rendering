#include <iostream>
#include <Windows.h>
#include "Code/Draw.h"

HWND MainWindowHandle = 0;
LPDIRECT3D9 g_pD3D{ nullptr };
LPDIRECT3DDEVICE9 g_pD3Ddev{ nullptr };

#define CurrentNamespace DrawTextureAlpha

HRESULT InitD3D(HWND hWnd)
{
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION); // Direct3D ���
	if (g_pD3D == nullptr)
		return E_FAIL;

	D3DDISPLAYMODE d3ddm;
	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm))) // ���÷��� ��� ����
		return E_FAIL;

	// ����̽� ����
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.Windowed = TRUE; // ������ ���
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // ȭ�� ��ȯ ���
	d3dpp.BackBufferFormat = d3ddm.Format; // ȭ�� ������ ������ ȭ��� ����

	if (FAILED(g_pD3D->CreateDevice(
		D3DADAPTER_DEFAULT, // ���� ���� ���� ī�尡 ���� ��, �����ϱ� ���� �뵵
		D3DDEVTYPE_HAL, // ���̵��� �ϵ����� �� ���ΰ�? (HAL = ���/REF = �̻��)
		hWnd, // ������ �ڵ�
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, // Direct3D�� �������� ���� ���� (HARDWARE = ���/SOFTWARE = �̻��)
		&d3dpp, // d3dpp
		&g_pD3Ddev)))  // ������ ����̽�
		return E_FAIL;

	if (FAILED(CurrentNamespace::Initialize(g_pD3Ddev)))
		return E_FAIL;

	return S_OK;
}

void Render(void)
{
	// ����� �˰� ĥ�Ѵ�
	g_pD3Ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0);

	g_pD3Ddev->BeginScene(); // ������ ����

	// �Լ� �߰�
	CurrentNamespace::Update(g_pD3Ddev);

	g_pD3Ddev->EndScene(); // ������ ����

	g_pD3Ddev->Present(NULL, NULL, NULL, NULL); // �������Ѱ͵��� ���� �����쿡 ���
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
	wc.style = CS_HREDRAW | CS_VREDRAW; // ������ Ŭ���� ��Ÿ��
	wc.lpfnWndProc = WndProc; // ������ ���ν���
	wc.cbClsExtra = 0; // Ȯ���
	wc.cbWndExtra = 0; // Ȯ���
	wc.hInstance = instanceHandle; // �ν��Ͻ� �ڵ�
	wc.hIcon = ::LoadIcon(0, IDI_APPLICATION); // ������ �ڵ�
	wc.hCursor = ::LoadCursor(0, IDC_ARROW); // Ŀ�� �ڵ�
	wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH)); // ��� �귯�� �ڵ�
	wc.lpszMenuName = 0; // �޴� �̸�
	wc.lpszClassName = "asdf1"; // ������ Ŭ���� �̸�

	::RegisterClass(&wc);

	MainWindowHandle = ::CreateWindow(
		"asdf123", // Ŭ������
		"dd", // ������ �̸�
		WS_OVERLAPPEDWINDOW, // ������ ��Ÿ��
		CW_USEDEFAULT, // �������� ���� ��� x ��ǥ
		CW_USEDEFAULT, // �������� ���� ��� y ��ǥ
		CW_USEDEFAULT, // �������� ��
		CW_USEDEFAULT, // �������� ����
		0, // �θ� ������ �ڵ�
		0, // �޴� �ڵ�, Ȥ�� �ڽ� ������ ID
		instanceHandle, // �ν��Ͻ� �ڵ�
		0); // ������ ���� ������

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