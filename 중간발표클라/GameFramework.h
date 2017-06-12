#pragma once

#include "Timer.h"
#include "Player.h"
#include "Scene.h"
#include "TitileScene.h"
#include "d3dx9core.h"
#include <d2d1.h>
#include <d2d1helper.h>
#include "Font.h"

#define	BUF_SIZE				512
#define	WM_SOCKET				WM_USER + 1

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();
	ID2D1HwndRenderTarget			*m_pd2dRenderTarget;
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();
	CFont *font;
	bool CreateRenderTargetDepthStencilView();
	bool CreateDirect3DDisplay();

	void CreateShaderVariables();
	void ReleaseShaderVariables();

	void BuildObjects();
	void ReleaseObjects();
	void veiwProcessInput();
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
	int InitNetWork(HWND main_window_handle);
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	ID3D11DeviceContext				*m_pd3dDeviceContext;
private:
	HINSTANCE						m_hInstance;
	HWND							m_hWnd;
	HMENU							m_hMenu;

	int								m_nWndClientWidth;
	int								m_nWndClientHeight;

	
	IDXGISwapChain					*m_pDXGISwapChain;
	ID3D11RenderTargetView			*m_pd3dRenderTargetView;

	UINT							m_n4xMSAAQualities;

	ID3D11Texture2D					*m_pd3dDepthStencilBuffer;
	ID3D11DepthStencilView			*m_pd3dDepthStencilView;

	CGameTimer						m_GameTimer;

	CScene							*m_pScene;

	CCamera							*m_pCamera;
	

	CTitileScene							*m_pScene1;
	POINT							m_ptOldCursorPos;
	_TCHAR							m_pszBuffer[50];

	CGameObject						*m_pBoundingBox;
	CGameObject					*m_pBullet;

	CAnimationShader                *m_pPlayerShader = { nullptr };
};
