#pragma once

#include "Timer.h"
#include "Player.h"
#include "Scene.h"



#define	BUF_SIZE				1024
#define	WM_SOCKET				WM_USER + 1

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();
	int InitNetWork(HWND main_window_handle); 
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	bool CreateRenderTargetDepthStencilView();
	bool CreateDirect3DDisplay();

	void CreateShaderVariables();
	void ReleaseShaderVariables();

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
	void ProcessPacket(char *ptr);


	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE						m_hInstance;
	HWND							m_hWnd;
	HMENU							m_hMenu;

	int								m_nWndClientWidth;
	int								m_nWndClientHeight;

	ID3D11Device					*m_pd3dDevice;
	IDXGISwapChain					*m_pDXGISwapChain;
	ID3D11RenderTargetView			*m_pd3dRenderTargetView;
	ID3D11DeviceContext				*m_pd3dDeviceContext;

	UINT							m_n4xMSAAQualities;

	ID3D11Texture2D					*m_pd3dDepthStencilBuffer;
	ID3D11DepthStencilView			*m_pd3dDepthStencilView;

	

	

	CCamera							*m_pCamera;

	POINT							m_ptOldCursorPos;
	_TCHAR							m_pszBuffer[50];
};
