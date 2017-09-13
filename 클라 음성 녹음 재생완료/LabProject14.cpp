// LabProject14.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "LabProject14.h"

#include "GameFramework.h"

#define MAX_LOADSTRING 100

TCHAR				szTitle[MAX_LOADSTRING];				// ���� ǥ���� �ؽ�Ʈ�Դϴ�.
TCHAR				szWindowClass[MAX_LOADSTRING];			// �⺻ â Ŭ���� �̸��Դϴ�.

CGameFramework		gGameFramework;      

BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LABPROJECT14, szWindowClass, MAX_LOADSTRING);
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LABPROJECT14));

	if (!InitInstance(hInstance, nCmdShow)) return(FALSE);

	MSG msg = {0};
	while (1) 
    {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        {
			if (msg.message == WM_QUIT) break;
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} 
        else 
        {
			gGameFramework.FrameAdvance();
		}
	}

	gGameFramework.OnDestroy();

	return((int)msg.wParam);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LABPROJECT14));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL/*MAKEINTRESOURCE(IDC_LABPROJECT14)*/;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	if (!RegisterClassEx(&wcex)) return(FALSE);

	RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect(&rc, dwStyle, FALSE);
	HWND hMainWnd = CreateWindow(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!hMainWnd) return(FALSE);

	if (!gGameFramework.OnCreate(hInstance, hMainWnd)) return(FALSE);

	ShowWindow(hMainWnd, nCmdShow);

	return(TRUE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
        case WM_SIZE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MOUSEMOVE:
        case WM_KEYDOWN:
        case WM_KEYUP:
			gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
			break;
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			switch (wmId)
			{
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_SOCKET:
			gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
			break;
		case MM_WIM_OPEN:
			gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
			return TRUE;
		case MM_WIM_DATA:
			gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
			return TRUE;
		case MM_WIM_CLOSE:
			gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
			return TRUE;
		case MM_WOM_OPEN:
			gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
			return TRUE;
		case MM_WOM_DONE:
			gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
			return TRUE;
		case MM_WOM_CLOSE:
			gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
			return TRUE;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return(0);
}
