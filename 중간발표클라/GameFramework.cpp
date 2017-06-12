
#include "stdafx.h"
#include "GameFramework.h"
#include "Shader.h"
#include <chrono>

using namespace chrono;
extern CGameObject	**m_titleObjects;

SOCKET g_mysocket;
WSABUF	send_wsabuf;
char 	send_buffer[MAX_BUFF_SIZE];
WSABUF	recv_wsabuf;
char	recv_buffer[MAX_BUFF_SIZE];
char	packet_buffer[MAX_PACKET_SIZE];
DWORD		in_packet_size = 0;
int		saved_packet_size = 0;
int		g_myid;
bool veiwtype = false;
HWND hendle;
CGameTimer						m_GameTimer;
CPlayer	*m_pPlayer;
DWORD ddwDirection;
float LcxDelta;
bool movechek = 0;
bool rotachek = 0;
bool team = false;
bool Scene;
int selectroomindex = 0;
int redai;
int blueai;
_TCHAR name[20];
int aiindex = 5000;
int NowScene;
CCamera							*m_pCamera;
ID3D11Device					*m_pd3dDevice;
CScene							*m_pScene;
D3D11_VIEWPORT d3dViewport;
extern CInstancedObjectsShader		**m_ppInstancingShaders;
extern  CHeightMapTerrain *pTerrain;
CGameObject					*m_pBullets[10][15];
HWND hwnd;
struct Overlap_ex {
	WSAOVERLAPPED original_overlap;
	int operation;
	WSABUF wsabuf;
	unsigned char iocp_buffer[MAX_BUFF_SIZE];
};

typedef struct {
	WSAOVERLAPPED	m_Overlapped;
	INT				m_io_type;
} OVERLAPPED_EX;

typedef struct {
	OVERLAPPED_EX	m_OverExSend;
	WSABUF			m_SendWSABuf;
	CHAR			m_SendBuffer[MAX_BUFF_SIZE];
} SEND_STRUCT;

struct player
{
	bool active;
	int id = -1;

};
struct team1 {

	bool active = false;
	int id = -1;

};
struct playerindex
{
	int index;
	bool active = false;
}myteam[4], enemyteam[5];

struct mainroom
{
	int num;
	bool start = 0;
	player player[10];
	int redteam;

	team1 red[5];
	team1 blue[5];
}mainroom;


struct room {
	int num;
	bool active;
	int playtime;
	_TCHAR name[50];
	_TCHAR text[50];
	int count;
	int timereckoning = 0;
	bool statenum;

}roomdata[6];

int roomindex = 0;
_TCHAR roomindexnum[10];
bool SendPacket(CHAR *packet, WORD size)
{
	SEND_STRUCT *Send = new SEND_STRUCT;

	CopyMemory(Send->m_SendBuffer, packet, size);

	Send->m_SendWSABuf.buf = Send->m_SendBuffer;
	Send->m_SendWSABuf.len = size;

	ZeroMemory(&Send->m_OverExSend, sizeof(OVERLAPPED_EX));
	//Send->m_OverExSend.m_io_type = IO_SEND;

	DWORD SendBytes;

	INT Result = WSASend(g_mysocket,
		&Send->m_SendWSABuf,
		1,
		&SendBytes,
		0,
		reinterpret_cast<LPWSAOVERLAPPED> (&Send->m_OverExSend),
		NULL);

	if (Result == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			printf("send() error!!\n");
			return FALSE;
		}
	}

	return TRUE;
}

CGameFramework::CGameFramework()
{
	m_pd3dDevice = NULL;
	m_pDXGISwapChain = NULL;
	m_pd3dRenderTargetView = NULL;
	m_pd3dDeviceContext = NULL;
	Scene = 1;
	m_pd3dDepthStencilBuffer = NULL;
	m_pd3dDepthStencilView = NULL;
	for (int i = 0; i < 10; i++)
		mainroom.player[i].active = 0;

	m_pScene = NULL;
	NowScene = 0;
	m_pPlayer = NULL;
	m_pCamera = NULL;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	_tcscpy_s(m_pszBuffer, _T("LabProject ("));

	srand(timeGetTime());

	m_pBoundingBox = nullptr;
	m_pBullet = nullptr;
	**m_pBullets = { nullptr };
}

CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;
	InitNetWork(hMainWnd);
	if (!CreateDirect3DDisplay()) return(false);
	hwnd = hMainWnd;
	BuildObjects();

	return(true);
}

int CGameFramework::InitNetWork(HWND main_window_handle) {

	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	hendle = main_window_handle;
	g_mysocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int Result = WSAConnect(g_mysocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);
	WSAAsyncSelect(g_mysocket, main_window_handle, WM_SOCKET, FD_CLOSE | FD_READ);

	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUF_SIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUF_SIZE;

	return 0;
}
bool CGameFramework::CreateDirect3DDisplay()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH/*0*/;

	UINT dwCreateDeviceFlags = 0;
#ifdef _DEBUG
	//	dwCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT nDriverTypes = sizeof(d3dDriverTypes) / sizeof(D3D_DRIVER_TYPE);

	D3D_FEATURE_LEVEL pd3dFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT nFeatureLevels = sizeof(pd3dFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);

	D3D_DRIVER_TYPE nd3dDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	HRESULT hResult = S_OK;
#ifdef _WITH_DEVICE_AND_SWAPCHAIN
	for (UINT i = 0; i < nDriverTypes; i++)
	{
		nd3dDriverType = d3dDriverTypes[i];
		if (SUCCEEDED(hResult = D3D11CreateDeviceAndSwapChain(NULL, nd3dDriverType, NULL, dwCreateDeviceFlags, pd3dFeatureLevels, nFeatureLevels, D3D11_SDK_VERSION, &dxgiSwapChainDesc, &m_pDXGISwapChain, &m_pd3dDevice, &nd3dFeatureLevel, &m_pd3dDeviceContext))) break;
	}
#else
	for (UINT i = 0; i < nDriverTypes; i++)
	{
		nd3dDriverType = d3dDriverTypes[i];
		if (SUCCEEDED(hResult = D3D11CreateDevice(NULL, nd3dDriverType, NULL, dwCreateDeviceFlags, pd3dFeatureLevels, nFeatureLevels, D3D11_SDK_VERSION, &m_pd3dDevice, &nd3dFeatureLevel, &m_pd3dDeviceContext))) break;
	}
	if (!m_pd3dDevice) return(false);

	if (FAILED(hResult = m_pd3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_n4xMSAAQualities))) return(false);
#ifdef _WITH_MSAA4_MULTISAMPLING
	dxgiSwapChainDesc.SampleDesc.Count = 4;
	dxgiSwapChainDesc.SampleDesc.Quality = m_n4xMSAAQualities - 1;
#else
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
#endif
	IDXGIFactory1 *pdxgiFactory = NULL;
	if (FAILED(hResult = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **)&pdxgiFactory))) return(false);
	IDXGIDevice *pdxgiDevice = NULL;
	if (FAILED(hResult = m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pdxgiDevice))) return(false);
	if (FAILED(hResult = pdxgiFactory->CreateSwapChain(pdxgiDevice, &dxgiSwapChainDesc, &m_pDXGISwapChain))) return(false);
	if (pdxgiDevice) pdxgiDevice->Release();
	if (pdxgiFactory) pdxgiFactory->Release();
#endif

	if (!CreateRenderTargetDepthStencilView()) return(false);
	
	font = new CFont(m_pd3dDevice, m_pd3dDeviceContext);
	font->m_pContext = m_pd3dDeviceContext;
	font->m_pGraphicDev = m_pd3dDevice;
	font->Ready_Font(_T("Arial"));
	
	swprintf_s(roomindexnum, L"%d", roomindex / 6 + 1);
	return(true);
}

bool CGameFramework::CreateRenderTargetDepthStencilView()
{
	HRESULT hResult = S_OK;

	ID3D11Texture2D *pd3dBackBuffer;
	if (FAILED(hResult = m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pd3dBackBuffer))) return(false);
	if (FAILED(hResult = m_pd3dDevice->CreateRenderTargetView(pd3dBackBuffer, NULL, &m_pd3dRenderTargetView))) return(false);
	if (pd3dBackBuffer) pd3dBackBuffer->Release();

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC d3dDepthStencilBufferDesc;
	ZeroMemory(&d3dDepthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
	d3dDepthStencilBufferDesc.Width = m_nWndClientWidth;
	d3dDepthStencilBufferDesc.Height = m_nWndClientHeight;
	d3dDepthStencilBufferDesc.MipLevels = 1;
	d3dDepthStencilBufferDesc.ArraySize = 1;
	d3dDepthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
#ifdef _WITH_MSAA4_MULTISAMPLING
	d3dDepthStencilBufferDesc.SampleDesc.Count = 4;
	d3dDepthStencilBufferDesc.SampleDesc.Quality = m_n4xMSAAQualities - 1;
#else
	d3dDepthStencilBufferDesc.SampleDesc.Count = 1;
	d3dDepthStencilBufferDesc.SampleDesc.Quality = 0;
#endif
	d3dDepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dDepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	d3dDepthStencilBufferDesc.CPUAccessFlags = 0;
	d3dDepthStencilBufferDesc.MiscFlags = 0;
	if (FAILED(hResult = m_pd3dDevice->CreateTexture2D(&d3dDepthStencilBufferDesc, NULL, &m_pd3dDepthStencilBuffer))) return(false);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.Format = d3dDepthStencilBufferDesc.Format;
#ifdef _WITH_MSAA4_MULTISAMPLING
	d3dDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
#else
	d3dDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
#endif
	d3dDepthStencilViewDesc.Texture2D.MipSlice = 0;
	if (FAILED(hResult = m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dDepthStencilViewDesc, &m_pd3dDepthStencilView))) return(false);

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);

	return(true);
}

void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	if (m_pd3dDeviceContext) m_pd3dDeviceContext->ClearState();
	if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();
	if (m_pDXGISwapChain) m_pDXGISwapChain->Release();
	if (m_pd3dDeviceContext) m_pd3dDeviceContext->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pScene) m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	{
		
		if (Scene) {
			if (NowScene == 0) {
				selectroomindex = 0;
				for (int i = 0; i < 6; i++)
					roomdata[i].active = 0;
				for (int i = 0; i < 10; i++)
					mainroom.player[i].active = false;
				for (int c = 0; c < 5; c++)
					mainroom.red[c].active = false;
				for (int c = 0; c < 5; c++)
					mainroom.blue[c].active = false;
				team = false;
				cs_packet_roominfor my_packet;
				my_packet.size = sizeof(cs_packet_roominfor);
				DWORD iobyte;
				if ((LOWORD(lParam) > d3dViewport.Width / 100 * 41.09&& LOWORD(lParam) < d3dViewport.Width / 100 * 61.71) && (HIWORD(lParam) > d3dViewport.Height / 100 * 50.20 && HIWORD(lParam) < d3dViewport.Height / 100 * 56.24))
				{

					my_packet.type = CS_selectroom;
					my_packet.roomnum = -1;

				}
				else if ((LOWORD(lParam) > d3dViewport.Width / 100 * 41.09&& LOWORD(lParam) < d3dViewport.Width / 100 * 61.71) && (HIWORD(lParam) > d3dViewport.Height / 100 * 58.94 && HIWORD(lParam) < d3dViewport.Height / 100 * 64.98))
				{
					NowScene = 2;
					m_titleObjects[0]->titlevisivle = false;
					m_titleObjects[2]->titlevisivle = false;
					m_titleObjects[4]->titlevisivle = true;
					my_packet.type = CS_roomnumveiw;

				}
				else if ((LOWORD(lParam) > d3dViewport.Width / 100 * 41.09&& LOWORD(lParam) < d3dViewport.Width / 100 * 61.71) && (HIWORD(lParam) > d3dViewport.Height / 100 * 69.16 && HIWORD(lParam) < d3dViewport.Height / 100 * 75))
				{
					::PostQuitMessage(0);
				}
				SendPacket(reinterpret_cast<CHAR*>(&my_packet), my_packet.size);
			}
			else if (NowScene == 2)
			{
				cs_packet_roominfor my_packet;
				my_packet.size = sizeof(cs_packet_roominfor);
				DWORD iobyte;
				if ((LOWORD(lParam) > d3dViewport.Width / 100 * 73.6&& LOWORD(lParam) < d3dViewport.Width / 100 * 93.12) && (HIWORD(lParam) > d3dViewport.Height / 100 * 78.75 && HIWORD(lParam) < d3dViewport.Height / 100 * 85))
				{
					//m_titleObjects[6]->titlevisivle = true;
					//m_titleObjects[6]->titlevisivle = false;

					if (roomdata[selectroomindex - 1].active == true) {
						if (roomdata[selectroomindex - 1].count < 10) {

							NowScene = 1;
							m_titleObjects[7]->titlevisivle = true;
							m_titleObjects[6]->titlevisivle = false;
							my_packet.type = CS_selectroom;
							my_packet.roomnum = roomindex + (selectroomindex - 1);

						}
					}

				}

				else if ((LOWORD(lParam) > d3dViewport.Width / 100 * 73.6&& LOWORD(lParam) < d3dViewport.Width / 100 * 93.12) && (HIWORD(lParam) > d3dViewport.Height / 100 * 87.70 && HIWORD(lParam) < d3dViewport.Height / 100 * 94.16))
				{
					NowScene = 0;

					m_titleObjects[5]->titlevisivle = false;
					m_titleObjects[4]->titlevisivle = false;
					m_titleObjects[0]->titlevisivle = true;
				}
				else if ((LOWORD(lParam) > d3dViewport.Width / 100 * 90.46&& LOWORD(lParam) < d3dViewport.Width / 100 * 94.37) && (HIWORD(lParam) > d3dViewport.Height / 100 * 25.41 && HIWORD(lParam) < d3dViewport.Height / 100 * 44.16))
				{

					if (roomindex != 0) {
						for (int i = 0; i < 6; i++)
							roomdata[i].active = 0;
						roomindex -= 6;
						my_packet.type = CS_roomcontrol;
						my_packet.roomnum = roomindex;
						swprintf_s(roomindexnum, L"%d", roomindex / 6 + 1);
					}

					//패킷전송
				}
				else if ((LOWORD(lParam) > d3dViewport.Width / 100 * 90.46&& LOWORD(lParam) < d3dViewport.Width / 100 * 94.37) && (HIWORD(lParam) > d3dViewport.Height / 100 * 50 && HIWORD(lParam) < d3dViewport.Height / 100 * 68.75))
				{
					//패킷전송
					for (int i = 0; i < 6; i++)
						roomdata[i].active = 0;
					roomindex += 6;
					my_packet.type = CS_roomcontrol;
					my_packet.roomnum = roomindex;
					swprintf_s(roomindexnum, L"%d", roomindex / 6 + 1);

				}
				else if (((LOWORD(lParam) > d3dViewport.Width / 100 * 9.37&& LOWORD(lParam) < d3dViewport.Width / 100 * 47.5) && (HIWORD(lParam) > d3dViewport.Height / 100 * 25 && HIWORD(lParam) < d3dViewport.Height / 100 * 38.12)))
				{
					selectroomindex = 1;
				}
				else if (((LOWORD(lParam) > d3dViewport.Width / 100 * 50.62&& LOWORD(lParam) < d3dViewport.Width / 100 * 87.34) && (HIWORD(lParam) > d3dViewport.Height / 100 * 25 && HIWORD(lParam) < d3dViewport.Height / 100 * 38.12)))
				{
					selectroomindex = 2;
				}
				else if (((LOWORD(lParam) > d3dViewport.Width / 100 * 9.37&& LOWORD(lParam) < d3dViewport.Width / 100 * 47.5) && (HIWORD(lParam) > d3dViewport.Height / 100 * 41.45 && HIWORD(lParam) < d3dViewport.Height / 100 * 55.20)))
				{
					selectroomindex = 3;
				}
				else if (((LOWORD(lParam) > d3dViewport.Width / 100 * 50.62&& LOWORD(lParam) < d3dViewport.Width / 100 * 87.34) && (HIWORD(lParam) > d3dViewport.Height / 100 * 41.45 && HIWORD(lParam) < d3dViewport.Height / 100 * 55.20)))
				{
					selectroomindex = 4;
				}
				else if (((LOWORD(lParam) > d3dViewport.Width / 100 * 9.37&& LOWORD(lParam) < d3dViewport.Width / 100 * 47.5) && (HIWORD(lParam) > d3dViewport.Height / 100 * 58.75 && HIWORD(lParam) < d3dViewport.Height / 100 * 72.08)))
				{
					selectroomindex = 5;
				}
				else if (((LOWORD(lParam) > d3dViewport.Width / 100 * 50.62&& LOWORD(lParam) < d3dViewport.Width / 100 * 87.34) && (HIWORD(lParam) > d3dViewport.Height / 100 * 58.75 && HIWORD(lParam) < d3dViewport.Height / 100 * 72.08)))
				{
					selectroomindex = 6;
				}
				SendPacket(reinterpret_cast<CHAR*>(&my_packet), my_packet.size);
			}
			else if (NowScene == 1)
			{
				cs_packet_roominfor my_packet;
				my_packet.size = sizeof(cs_packet_roominfor);
				DWORD iobyte;
				cs_packet_aipacket send_pack;
				send_pack.size = sizeof(cs_packet_aipacket);



				if ((LOWORD(lParam) > d3dViewport.Width / 100 * 73.6&& LOWORD(lParam) < d3dViewport.Width / 100 * 93.12) && (HIWORD(lParam) > d3dViewport.Height / 100 * 71.70 && HIWORD(lParam) < d3dViewport.Height / 100 * 79))
				{
					if (mainroom.start == 0)
						my_packet.type = CS_gameplay;
					//my_packet.roomnum = roomindex + (selectroomindex - 1);

					//게임 강제스타트

				}
				else if ((LOWORD(lParam) > d3dViewport.Width / 100 * 73.6&& LOWORD(lParam) < d3dViewport.Width / 100 * 93.12) && (HIWORD(lParam) > d3dViewport.Height / 100 * 80.75 && HIWORD(lParam) < d3dViewport.Height / 100 * 87))
				{
					//Scene = 0;
					//m_titleObjects[9]->titlevle = false;
					if (mainroom.start == 1)
						my_packet.type = CS_freeveiw;
					//my_packet.roomnum = roomindex + (selectroomindex - 1);

					//관전모드

				}
				else if ((LOWORD(lParam) > d3dViewport.Width / 100 * 73.6&& LOWORD(lParam) < d3dViewport.Width / 100 * 93.12) && (HIWORD(lParam) > d3dViewport.Height / 100 * 87.70 && HIWORD(lParam) < d3dViewport.Height / 100 * 94.16))
				{
					NowScene = 0;
					m_titleObjects[5]->titlevisivle = false;
					m_titleObjects[4]->titlevisivle = false;
					m_titleObjects[7]->titlevisivle = false;
					m_titleObjects[8]->titlevisivle = false;
					m_titleObjects[9]->titlevisivle = false;
					m_titleObjects[10]->titlevisivle = false;
					m_titleObjects[0]->titlevisivle = true;
					
					my_packet.type = CS_exitroom;
					my_packet.roomnum = mainroom.num;
					//나가기

				}
				else if ((LOWORD(lParam) > d3dViewport.Width / 100 * 12.18 && LOWORD(lParam) < d3dViewport.Width / 100 * 43.90) && (HIWORD(lParam) > d3dViewport.Height / 100 * 10.83 && HIWORD(lParam) < d3dViewport.Height / 100 * 19.58))
				{
					if (team) {
						my_packet.type = CS_changeteam;
						my_packet.roomnum = 1;
						team = false;

					}
					//팀변경 패킷보내 레드로
				}
				else if ((LOWORD(lParam) > d3dViewport.Width / 100 * 55.46 && LOWORD(lParam) < d3dViewport.Width / 100 * 87.5) && (HIWORD(lParam) > d3dViewport.Height / 100 * 10.83 && HIWORD(lParam) < d3dViewport.Height / 100 * 19.58))
				{
					if (team == 0) {
						my_packet.type = CS_changeteam;
						my_packet.roomnum = 2;
						team = true;
					}
					//팀변경패킷 블루로
				}
				else if (((LOWORD(lParam) > d3dViewport.Width / 100 * 12.18&& LOWORD(lParam) < d3dViewport.Width / 100 * 48.75) && (HIWORD(lParam) > d3dViewport.Height / 100 * 22.5 && HIWORD(lParam) < d3dViewport.Height / 100 * 30.20)))
				{
					if (!mainroom.red[0].active)
					{
						send_pack.team = 0;
						send_pack.type = CS_addAIplayer;

						SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						redai = 0;
					}
					else
					{
						if (mainroom.red[0].id > 4999)
						{
							send_pack.type = CS_deleteAIplayer;
							send_pack.team = 0;
							send_pack.id = mainroom.red[0].id;
							SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						}
					}
				}
				else if (((LOWORD(lParam) > d3dViewport.Width / 100 * 51.09&& LOWORD(lParam) < d3dViewport.Width / 100 * 87.5) && (HIWORD(lParam) > d3dViewport.Height / 100 * 22.5 && HIWORD(lParam) < d3dViewport.Height / 100 * 30.20)))
				{
					if (!mainroom.blue[0].active)
					{
						send_pack.team = 1;
						send_pack.type = CS_addAIplayer;

						SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						blueai = 0;
					}
					else
					{
						if (mainroom.blue[0].id > 4999)
						{
							send_pack.type = CS_deleteAIplayer;
							send_pack.team = 1;
							send_pack.id = mainroom.blue[0].id;
							SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						}
					}
				}
				else if (((LOWORD(lParam) > d3dViewport.Width / 100 * 12.18&& LOWORD(lParam) < d3dViewport.Width / 100 * 48.75) && (HIWORD(lParam) > d3dViewport.Height / 100 * 32.5&& HIWORD(lParam) < d3dViewport.Height / 100 * 40)))
				{
					if (!mainroom.red[1].active)
					{
						send_pack.team = 0;
						send_pack.type = CS_addAIplayer;

						SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						redai = 1;
					}
					else
					{
						if (mainroom.red[1].id > 4999)
						{
							send_pack.type = CS_deleteAIplayer;
							send_pack.team = 0;
							send_pack.id = mainroom.red[1].id;
							SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						}
					}
				}
				else if (((LOWORD(lParam) > d3dViewport.Width / 100 * 51.09&& LOWORD(lParam) < d3dViewport.Width / 100 * 87.5) && (HIWORD(lParam) > d3dViewport.Height / 100 * 32.5 && HIWORD(lParam) < d3dViewport.Height / 100 * 40)))
				{
					if (!mainroom.blue[1].active)
					{
						send_pack.team = 1;
						send_pack.type = CS_addAIplayer;

						SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						blueai = 1;
					}
					else
					{
						if (mainroom.blue[1].id > 4999)
						{
							send_pack.type = CS_deleteAIplayer;
							send_pack.team = 1;
							send_pack.id = mainroom.blue[1].id;
							SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						}
					}
				}
				else if (((LOWORD(lParam) > d3dViewport.Width / 100 * 12.18&& LOWORD(lParam) < d3dViewport.Width / 100 * 48.75) && (HIWORD(lParam) > d3dViewport.Height / 100 * 42.08 && HIWORD(lParam) < d3dViewport.Height / 100 * 49.79)))
				{
					if (!mainroom.red[2].active)
					{
						send_pack.team = 0;
						send_pack.type = CS_addAIplayer;

						SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						redai = 2;
					}
					else
					{
						if (mainroom.red[2].id > 4999)
						{
							send_pack.type = CS_deleteAIplayer;
							send_pack.team = 0;
							send_pack.id = mainroom.red[2].id;
							SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						}
					}
				}
				else if (((LOWORD(lParam) > d3dViewport.Width / 100 * 51.09&& LOWORD(lParam) < d3dViewport.Width / 100 * 87.5) && (HIWORD(lParam) > d3dViewport.Height / 100 * 42.08 && HIWORD(lParam) < d3dViewport.Height / 100 * 49.79)))
				{
					if (!mainroom.blue[2].active)
					{
						send_pack.team = 1;
						send_pack.type = CS_addAIplayer;

						SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						blueai = 2;
					}
					else
					{
						if (mainroom.blue[2].id > 4999)
						{
							send_pack.type = CS_deleteAIplayer;
							send_pack.team = 1;
							send_pack.id = mainroom.blue[2].id;
							SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						}
					}
				}
				else if (((LOWORD(lParam) > d3dViewport.Width / 100 * 12.18&& LOWORD(lParam) < d3dViewport.Width / 100 * 48.75) && (HIWORD(lParam) > d3dViewport.Height / 100 * 51.87 && HIWORD(lParam) < d3dViewport.Height / 100 * 58.75)))
				{
					if (!mainroom.red[3].active)
					{
						send_pack.team = 0;
						send_pack.type = CS_addAIplayer;

						SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						redai = 3;
					}
					else
					{
						if (mainroom.red[3].id > 4999)
						{
							send_pack.type = CS_deleteAIplayer;
							send_pack.team = 0;
							send_pack.id = mainroom.red[3].id;
							SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						}
					}
				}
				else if (((LOWORD(lParam) > d3dViewport.Width / 100 * 51.09&& LOWORD(lParam) < d3dViewport.Width / 100 * 87.5) && (HIWORD(lParam) > d3dViewport.Height / 100 * 51.87 && HIWORD(lParam) < d3dViewport.Height / 100 * 58.75)))
				{
					if (!mainroom.blue[3].active)
					{
						send_pack.team = 1;
						send_pack.type = CS_addAIplayer;

						SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						blueai = 3;
					}
					else
					{
						if (mainroom.blue[3].id > 4999)
						{
							send_pack.type = CS_deleteAIplayer;
							send_pack.team = 1;
							send_pack.id = mainroom.blue[3].id;
							SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						}
					}
				}
				else if (((LOWORD(lParam) > d3dViewport.Width / 100 * 12.18&& LOWORD(lParam) < d3dViewport.Width / 100 * 48.75) && (HIWORD(lParam) > d3dViewport.Height / 100 * 61.87 && HIWORD(lParam) < d3dViewport.Height / 100 * 69.16)))
				{
					if (!mainroom.red[4].active)
					{
						send_pack.team = 0;
						send_pack.type = CS_addAIplayer;

						SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						redai = 4;
					}
					else
					{
						if (mainroom.red[4].id > 4999)
						{
							send_pack.type = CS_deleteAIplayer;
							send_pack.team = 0;
							send_pack.id = mainroom.red[4].id;
							SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						}
					}
				}
				else if (((LOWORD(lParam) > d3dViewport.Width / 100 * 51.09&& LOWORD(lParam) < d3dViewport.Width / 100 * 87.5) && (HIWORD(lParam) > d3dViewport.Height / 100 * 61.87 && HIWORD(lParam) < d3dViewport.Height / 100 * 69.16)))
				{
					if (!mainroom.blue[4].active)
					{
						send_pack.team = 1;
						send_pack.type = CS_addAIplayer;

						SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						blueai = 4;
					}
					else
					{
						if (mainroom.blue[4].id > 4999)
						{
							send_pack.type = CS_deleteAIplayer;
							send_pack.team = 1;
							send_pack.id = mainroom.blue[4].id;
							SendPacket(reinterpret_cast<CHAR*>(&send_pack), send_pack.size);
						}
					}
				}
				SendPacket(reinterpret_cast<CHAR*>(&my_packet), my_packet.size);
			}
			break;
		}
	case WM_RBUTTONDOWN:
		SetCapture(hWnd);
		GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
		if (Scene == 0)
		{
	
			cs_packet_lookvector my_packet;
			DWORD iobyte;
			my_packet.type = CS_LKVECTOR;
			my_packet.x = m_pCamera->GetLookVector().x;
			my_packet.y = m_pCamera->GetLookVector().y;
			my_packet.z = m_pCamera->GetLookVector().z;
			my_packet.size = sizeof(cs_packet_lookvector);
		
			SendPacket(reinterpret_cast<CHAR*>(&my_packet), my_packet.size);
		}
	}
	case WM_RBUTTONUP:
		//ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		if (Scene) {
			if (NowScene == 0) {
				if ((LOWORD(lParam) > d3dViewport.Width / 100 * 41.09&& LOWORD(lParam) < d3dViewport.Width / 100 * 61.71) && (HIWORD(lParam) > d3dViewport.Height / 100 * 50.20 && HIWORD(lParam) < d3dViewport.Height / 100 * 56.24))
				{
					m_titleObjects[1]->titlevisivle = true;

				}

				else if ((LOWORD(lParam) > d3dViewport.Width / 100 * 41.09&& LOWORD(lParam) < d3dViewport.Width / 100 * 61.71) && (HIWORD(lParam) > d3dViewport.Height / 100 * 58.94 && HIWORD(lParam) < d3dViewport.Height / 100 * 64.98))
				{
					m_titleObjects[2]->titlevisivle = true;

				}
				else if ((LOWORD(lParam) > d3dViewport.Width / 100 * 41.09&& LOWORD(lParam) < d3dViewport.Width / 100 * 61.71) && (HIWORD(lParam) > d3dViewport.Height / 100 * 69.16 && HIWORD(lParam) < d3dViewport.Height / 100 * 75))
				{
					m_titleObjects[3]->titlevisivle = true;

				}
				else {
					m_titleObjects[1]->titlevisivle = false;
					m_titleObjects[2]->titlevisivle = false;
					m_titleObjects[3]->titlevisivle = false;
				}
			}
			else if (NowScene == 2)
			{
				if ((LOWORD(lParam) > d3dViewport.Width / 100 * 73.6&& LOWORD(lParam) < d3dViewport.Width / 100 * 93.12) && (HIWORD(lParam) > d3dViewport.Height / 100 * 78.75 && HIWORD(lParam) < d3dViewport.Height / 100 * 85))
				{
					m_titleObjects[6]->titlevisivle = true;

				}

				else if ((LOWORD(lParam) > d3dViewport.Width / 100 * 73.6&& LOWORD(lParam) < d3dViewport.Width / 100 * 93.12) && (HIWORD(lParam) > d3dViewport.Height / 100 * 87.70 && HIWORD(lParam) < d3dViewport.Height / 100 * 94.16))
				{
					m_titleObjects[5]->titlevisivle = true;

				}
				else
				{
					m_titleObjects[6]->titlevisivle = false;
					m_titleObjects[5]->titlevisivle = false;

				}
			}
			else if (NowScene == 1)
			{

				if ((LOWORD(lParam) > d3dViewport.Width / 100 * 73.6&& LOWORD(lParam) < d3dViewport.Width / 100 * 93.12) && (HIWORD(lParam) > d3dViewport.Height / 100 * 80.75 && HIWORD(lParam) < d3dViewport.Height / 100 * 87))
				{
					m_titleObjects[9]->titlevisivle = true;

				}
				else if ((LOWORD(lParam) > d3dViewport.Width / 100 * 73.6&& LOWORD(lParam) < d3dViewport.Width / 100 * 93.12) && (HIWORD(lParam) > d3dViewport.Height / 100 * 88.70 && HIWORD(lParam) < d3dViewport.Height / 100 * 94.16))
				{


					m_titleObjects[8]->titlevisivle = true;

				}
				else if ((LOWORD(lParam) > d3dViewport.Width / 100 * 73.6&& LOWORD(lParam) < d3dViewport.Width / 100 * 93.12) && (HIWORD(lParam) > d3dViewport.Height / 100 * 71.70 && HIWORD(lParam) < d3dViewport.Height / 100 * 79))
				{


					m_titleObjects[10]->titlevisivle = true;

				}
				else
				{
					m_titleObjects[8]->titlevisivle = false;
					m_titleObjects[9]->titlevisivle = false;
					m_titleObjects[10]->titlevisivle = false;
				}
			}
		}
		break;
	default:
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pScene) m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			if (Scene == 0)
			{
				Scene = 1;
				ReleaseCapture();
				NowScene = 0;
				m_titleObjects[5]->titlevisivle = false;
				m_titleObjects[4]->titlevisivle = false;
				m_titleObjects[7]->titlevisivle = false;
				m_titleObjects[8]->titlevisivle = false;
				m_titleObjects[9]->titlevisivle = false;
				m_titleObjects[10]->titlevisivle = false;
				m_titleObjects[0]->titlevisivle = true;
				cs_packet_roominfor my_packet;
				my_packet.size = sizeof(cs_packet_roominfor);
				DWORD iobyte;
				
				my_packet.type = CS_exitroom;
				my_packet.roomnum = mainroom.num;
				SendPacket(reinterpret_cast<CHAR*>(&my_packet), my_packet.size);

			}
			else {
				::PostQuitMessage(0);
			}
			break;
		case VK_RETURN:
			break;
		case VK_F1:
		case VK_F2:
		case VK_F3:
			if (m_pPlayer)
			{
				m_pPlayer->ChangeCamera(m_pd3dDevice, DWORD(wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
				m_pCamera = m_pPlayer->GetCamera();
				m_pScene->SetCamera(m_pCamera);
			}
			break;
		case VK_F9:
		{
			BOOL bFullScreenState = FALSE;
			m_pDXGISwapChain->GetFullscreenState(&bFullScreenState, NULL);
			if (!bFullScreenState)
			{
				DXGI_MODE_DESC dxgiTargetParameters;
				dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				dxgiTargetParameters.Width = m_nWndClientWidth;
				dxgiTargetParameters.Height = m_nWndClientHeight;
				dxgiTargetParameters.RefreshRate.Numerator = 0;
				dxgiTargetParameters.RefreshRate.Denominator = 0;
				dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
				dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				m_pDXGISwapChain->ResizeTarget(&dxgiTargetParameters);
			}
			m_pDXGISwapChain->SetFullscreenState(!bFullScreenState, NULL);
			d3dViewport = m_pCamera->GetViewport();
			break;
		}
		case VK_F10:
			break;
		case VK_TAB:
		{

		}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void ProcessPacket(char *ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_PUT_PLAYER:
	{
		sc_packet_put_player *my_packet = reinterpret_cast<sc_packet_put_player *>(ptr);
		if (mainroom.start == false) {
			int id = my_packet->id;
			if (first_time) {
				printf("접속성공");
				first_time = false;
				g_myid = id;
			}
		}
		else {
			if(my_packet->team==0)
				for (int i = 0; i < 5; i++)
				{
					if (mainroom.red[i].id == my_packet->id)
					{
						if (my_packet->id == g_myid)
						{

							m_pPlayer->SetPosition(D3DXVECTOR3(my_packet->mtx._41, my_packet->mtx._42, my_packet->mtx._43));
						}
						else
						{
							m_ppInstancingShaders[7]->m_ppObjects[i]->SetPosition(D3DXVECTOR3(my_packet->mtx._41, my_packet->mtx._42, my_packet->mtx._43));
						}
						break;
					}
				}
			else {
				for (int i = 0; i < 5; i++)
				{
					if (mainroom.red[i].id == my_packet->id)
					{
						if (my_packet->id == g_myid)
						{
							m_pPlayer->SetPosition(D3DXVECTOR3(my_packet->mtx._41, my_packet->mtx._42, my_packet->mtx._43));

						}
						else
						{
						//	m_ppInstancingShaders[8]->m_ppObjects[i]->SetPosition(D3DXVECTOR3(my_packet->mtx._41, my_packet->mtx._42, my_packet->mtx._43));
						}
						break;

					}


				}
			}

		}
		
		

		break;
	}
	case SC_POS:
	{
		sc_packet_pos *my_packet = reinterpret_cast<sc_packet_pos *>(ptr);
		int other_id = my_packet->id;
		
		if (my_packet->team == 0) {
			for (int i = 0; i < 5; i++)
			{
				if (mainroom.red[i].id == my_packet->id)
				{
					if (my_packet->id == g_myid)
					{
						m_pPlayer->SetPosition(D3DXVECTOR3(my_packet->mtx._41, 415, my_packet->mtx._43));
					
					}
					else
					{
						m_ppInstancingShaders[7]->m_ppObjects[i]->SetPosition(my_packet->mtx._41, 415, my_packet->mtx._43);
					}
					break;
				}


			}
		}
		else {
			for (int i = 0; i < 5; i++)
			{
				if (mainroom.blue[i].id == my_packet->id)
				{
					if (my_packet->id == g_myid)
					{
						m_pPlayer->SetPosition(D3DXVECTOR3(my_packet->mtx._41, 415, my_packet->mtx._43));
					}
					else
					{
						m_ppInstancingShaders[8]->m_ppObjects[i]->SetPosition(my_packet->mtx._41, 415, my_packet->mtx._43);
					}
					break;

				}
			}
		}
		break;

	}
	case SC_ROTATE:
	{
		m_pCamera = m_pPlayer->GetCamera();
		sc_packet_rotation *packet_rotaion = reinterpret_cast<sc_packet_rotation *>(ptr);
		int other_id = packet_rotaion->id;
		if (packet_rotaion->team == 0) {
			for (int i = 0; i < 5; i++)
			{
				if (mainroom.red[i].id == packet_rotaion->id)
				{
					if (packet_rotaion->id == g_myid)
					{

						if (packet_rotaion->lr == CS_RIGHTROTATION)
							m_pPlayer->Rotate(0, 2, 0);
						else if (packet_rotaion->lr == CS_LEFTROTATION)
							m_pPlayer->Rotate(0, -2, 0);
						else if (packet_rotaion->lr == CS_UPROTATION) {
							m_pCamera->sumrotate -= 1;
							
						}
						else if (packet_rotaion->lr == CS_DOWNROTATION)
							m_pCamera->sumrotate += 1;
					
					}
					else
					{
						if (packet_rotaion->lr == CS_RIGHTROTATION)
							m_ppInstancingShaders[7]->m_ppObjects[i]->Rotate(0, 2, 0);
						else if (packet_rotaion->lr == CS_LEFTROTATION)
							m_ppInstancingShaders[7]->m_ppObjects[i]->Rotate(0, -2, 0);
			

					}
					break;

				}


			}
		}
		else {
			for (int i = 0; i < 5; i++)
			{
				if (mainroom.blue[i].id == packet_rotaion->id)
				{
					if (packet_rotaion->id == g_myid)
					{
						if (packet_rotaion->lr == CS_RIGHTROTATION)
							m_pPlayer->Rotate(0, 2, 0);
						else if (packet_rotaion->lr == CS_LEFTROTATION)
							m_pPlayer->Rotate(0, -2, 0);
						else if (packet_rotaion->lr == CS_UPROTATION) {
							m_pCamera->sumrotate -= 1;

						}
						else if (packet_rotaion->lr == CS_DOWNROTATION)
							m_pCamera->sumrotate += 1;
					}
					else
					{
						if (packet_rotaion->lr == CS_RIGHTROTATION)
							m_ppInstancingShaders[8]->m_ppObjects[i]->Rotate(0, 2, 0);
						else if (packet_rotaion->lr == CS_LEFTROTATION)
							m_ppInstancingShaders[8]->m_ppObjects[i]->Rotate(0, -2, 0);

					}
					break;

				}
			}
		}
		break;
	}

	case SC_bullet:
	{
		cs_packet_bullet *packet_bullet = reinterpret_cast<cs_packet_bullet *>(ptr);
		int other_id = packet_bullet->id;
	
		//m_pPlayer->SetPosition(D3DXVECTOR3(packet_bullet->x, m_pPlayer->m_d3dxmtxWorld._42 + 5, packet_bullet->y));
		//int other_id = packet_bullet->
		if (packet_bullet->team == 0) {
			for (int i = 0; i < 5; i++)
			{
				if (mainroom.red[i].id == packet_bullet->id)
				{
					if (packet_bullet->id == g_myid)
					{
				
						if (m_pBullets[i][packet_bullet->index]->on == false) {
							
						//	D3DXMatrixIdentity(&m_pBullets[i][packet_bullet->index]->m_d3dxmtxWorld);
							D3DXVECTOR3 d3dxvRight = m_pPlayer->GetRightVector();
							D3DXVECTOR3 d3dxvUp = m_pPlayer->GetUpVector();
							D3DXVECTOR3 d3dxvLook = m_pPlayer->GetLookVector();
							m_pBullets[i][packet_bullet->index]->m_d3dxmtxLocal._11 = d3dxvRight.x; m_pBullets[i][packet_bullet->index]->m_d3dxmtxLocal._21 = d3dxvUp.x;
							m_pBullets[i][packet_bullet->index]->m_d3dxmtxLocal._31 = d3dxvLook.x;
							m_pBullets[i][packet_bullet->index]->m_d3dxmtxLocal._12 = d3dxvRight.y;
							m_pBullets[i][packet_bullet->index]->m_d3dxmtxLocal._22 = d3dxvUp.y;
							m_pBullets[i][packet_bullet->index]->m_d3dxmtxLocal._32 = d3dxvLook.y;
							m_pBullets[i][packet_bullet->index]->m_d3dxmtxLocal._13 = d3dxvRight.z;
							m_pBullets[i][packet_bullet->index]->m_d3dxmtxLocal._23 = d3dxvUp.z; m_pBullets[i][packet_bullet->index]->m_d3dxmtxLocal._33 = d3dxvLook.z;
							m_pBullets[i][packet_bullet->index]->on = true;
						}
						
						m_pBullets[i][packet_bullet->index]->SetPosition(packet_bullet->x, packet_bullet->y, packet_bullet->z);
						if (packet_bullet->x == 4419)
						{
					
							m_pBullets[i][packet_bullet->index]->on = false;
							m_pBullets[i][packet_bullet->index]->SetPosition(5000, 5000, 5000);
						}
					}
					else
					{
						if (m_pBullets[i][packet_bullet->index]->on == false) {
						//	D3DXMatrixIdentity(&m_pBullets[i][packet_bullet->index]->m_d3dxmtxWorld);
							D3DXVECTOR3 d3dxvRight = m_ppInstancingShaders[7]->m_ppObjects[i]->GetRight();
							D3DXVECTOR3 d3dxvUp = m_ppInstancingShaders[7]->m_ppObjects[i]->GetUp();
							D3DXVECTOR3 d3dxvLook = m_ppInstancingShaders[7]->m_ppObjects[i]->GetLook();
						
							m_pBullets[i][packet_bullet->index]->m_d3dxmtxLocal._11 = d3dxvRight.x; m_pBullets[i][packet_bullet->index]->m_d3dxmtxLocal._21 = d3dxvUp.x;
							m_pBullets[i][packet_bullet->index]->m_d3dxmtxLocal._31 = d3dxvLook.x;
							m_pBullets[i][packet_bullet->index]->m_d3dxmtxLocal._12 = d3dxvRight.y;
							m_pBullets[i][packet_bullet->index]->m_d3dxmtxLocal._22 = d3dxvUp.y;
							m_pBullets[i][packet_bullet->index]->m_d3dxmtxLocal._32 = d3dxvLook.y;
							m_pBullets[i][packet_bullet->index]->m_d3dxmtxLocal._13 = d3dxvRight.z;
							m_pBullets[i][packet_bullet->index]->m_d3dxmtxLocal._23 = d3dxvUp.z; m_pBullets[i][packet_bullet->index]->m_d3dxmtxLocal._33 = d3dxvLook.z;
							m_pBullets[i][packet_bullet->index]->on = true;
						}
						m_pBullets[i][packet_bullet->index]->SetPosition(packet_bullet->x, packet_bullet->y, packet_bullet->z);
						if (packet_bullet->x == 4419)
						{
					
							m_pBullets[i][packet_bullet->index]->on = false;
							m_pBullets[i][packet_bullet->index]->SetPosition(5000, 5000, 5000);
						}

					}
					break;

				}
			}
		}
		else {
			for (int i = 0; i < 5; i++)
			{
				if (mainroom.blue[i].id == packet_bullet->id)
				{
					
					if (packet_bullet->id == g_myid)
					{
					
						if (m_pBullets[5+i][packet_bullet->index]->on == false) {
						//	D3DXMatrixIdentity(&m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxWorld);
							D3DXVECTOR3 d3dxvRight = m_pPlayer->GetRightVector();
							D3DXVECTOR3 d3dxvUp = m_pPlayer->GetUpVector();
							D3DXVECTOR3 d3dxvLook = m_pPlayer->GetLookVector();
							m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxLocal._11 = d3dxvRight.x; m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxLocal._21 = d3dxvUp.x;
							m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxLocal._31 = d3dxvLook.x;
							m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxLocal._12 = d3dxvRight.y;
							m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxLocal._22 = d3dxvUp.y;
							m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxLocal._32 = d3dxvLook.y;
							m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxLocal._13 = d3dxvRight.z;
							m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxLocal._23 = d3dxvUp.z; m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxLocal._33 = d3dxvLook.z;
							m_pBullets[5+i][packet_bullet->index]->on = true;
						}
						m_pBullets[5+i][packet_bullet->index]->SetPosition(packet_bullet->x, packet_bullet->y, packet_bullet->z);
						if (packet_bullet->x == 4419)
						{
							m_pBullets[5+i][packet_bullet->index]->on = false;
							m_pBullets[5+i][packet_bullet->index]->SetPosition(5000, 5000, 5000);
						}

					}
					else
					{
						if (m_pBullets[5+i][packet_bullet->index]->on == false) {
						//	D3DXMatrixIdentity(&m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxWorld);
							D3DXVECTOR3 d3dxvRight = m_ppInstancingShaders[8]->m_ppObjects[i]->GetRight();
							D3DXVECTOR3 d3dxvUp = m_ppInstancingShaders[8]->m_ppObjects[i]->GetUp();
							D3DXVECTOR3 d3dxvLook = m_ppInstancingShaders[8]->m_ppObjects[i]->GetLook();
							
							m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxLocal._11 = d3dxvRight.x; m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxLocal._21 = d3dxvUp.x;
							m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxLocal._31 = d3dxvLook.x;
							m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxLocal._12 = d3dxvRight.y;
							m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxLocal._22 = d3dxvUp.y;
							m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxLocal._32 = d3dxvLook.y;
							m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxLocal._13 = d3dxvRight.z;
							m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxLocal._23 = d3dxvUp.z; m_pBullets[5+i][packet_bullet->index]->m_d3dxmtxLocal._33 = d3dxvLook.z;
							m_pBullets[5+i][packet_bullet->index]->on = true;
						}
						m_pBullets[5+i][packet_bullet->index]->SetPosition(packet_bullet->x, packet_bullet->y, packet_bullet->z);
						if (packet_bullet->x == 4419)
						{
							
							m_pBullets[5+i][packet_bullet->index]->on = false;
							m_pBullets[5+i][packet_bullet->index]->SetPosition(5000, 5000, 5000);
						}

					}
					break;

				}
			}
		}
		break;
	}
	case CS_gameplay:
	{

		Scene = 0;
		m_titleObjects[10]->titlevisivle = false;
		mainroom.start = true;
		NowScene = 0;

		break;
	}
	case CS_roomnumveiw:
	{

		cs_packet_roomveiw *my_packet = reinterpret_cast<cs_packet_roomveiw *>(ptr);
		roomdata[my_packet->num].active = true;
		roomdata[my_packet->num].playtime = my_packet->playtime;
		roomdata[my_packet->num].num = my_packet->num;
		roomdata[my_packet->num].timereckoning = 0;
		if (my_packet->playtime == 0)
		{
			roomdata[my_packet->num].statenum = false; // 대기방
		}
		roomdata[my_packet->num].count = my_packet->count;
		break;
	}

	case CS_freeveiw:
	{
		
		Scene = 0;
		NowScene = 0;
		m_titleObjects[10]->titlevisivle = false;
		veiwtype = 1;
	
		break;
	}
	case SC_MINIONPOS:
	{
		sc_packet_minionpos *my_packet = reinterpret_cast<sc_packet_minionpos *>(ptr);
		if (my_packet->team == 0) //레드팀미니언
		{
			
			m_ppInstancingShaders[0]->m_ppObjects[my_packet->id]->SetPosition(my_packet->mtx._41, 415, my_packet->mtx._43);
		}
		else {//블루팀미니언
		
			m_ppInstancingShaders[9]->m_ppObjects[my_packet->id]->SetPosition(my_packet->mtx._41, 415, my_packet->mtx._43);
		}
		break;
	}
	case CS_selectroom:
	{
		
		cs_packet_changeteam *my_packet = reinterpret_cast<cs_packet_changeteam *>(ptr);
		if (my_packet->id == g_myid) {
			NowScene = 1;
			m_titleObjects[0]->titlevisivle = false;
			m_titleObjects[1]->titlevisivle = false;
			m_titleObjects[7]->titlevisivle = true;
		}
		mainroom.start = my_packet->state;
		mainroom.num = my_packet->roomnum;
		for (int i = 0; i < 10; i++) {
			if (mainroom.player[i].active == 0) {
				mainroom.player[i].id = my_packet->id;
				if (my_packet->team == 0) {
					for (int c = 0; c < 5; c++)
					{
						if (!mainroom.red[c].active)
						{
							mainroom.red[c].active = true;
							mainroom.red[c].id = my_packet->id;
							break;
						}
					}
				}
				else {
					for (int c = 0; c < 5; c++)
					{
						if (!mainroom.blue[c].active)
						{
							mainroom.blue[c].active = true;
							mainroom.blue[c].id = my_packet->id;
							break;
						}
					}

				}
				mainroom.player[i].active = true;
				break;
			}
		}
		
		break;
	}
	case CS_exitroom:
	{
		cs_packet_roomveiw *my_packet = reinterpret_cast<cs_packet_roomveiw *>(ptr);
		if (my_packet->count == 0) {
			for (int i = 0; i < 10; i++) {
				mainroom.player[i].active = false;
			}
		}
		for (int i = 0; i < 10; i++)
		{
			if (mainroom.player[i].active)
			{

				if (mainroom.player[i].id == my_packet->num)
				{

					mainroom.player[i].active = false;
					for (int c = 0; c < 5; c++)
					{
						if (mainroom.red[c].id == my_packet->num)
						{
							mainroom.red[c].active = false;
							mainroom.red[c].id = -1;
							break;
						}
					}
					for (int c = 0; c < 5; c++)
					{
						if (mainroom.blue[c].id == my_packet->num)
						{
							mainroom.blue[c].active = false;
							mainroom.blue[c].id = -1;
							break;
						}
					}
					//mainroom.player[i].id = -1;
					break;
				}
			}

		}
		//나를제외한 나머지 리스트에서 제거
		break;
	}
	case CS_changeteam:
	{
		cs_packet_changeteam *my_packet = reinterpret_cast<cs_packet_changeteam *>(ptr);
		if (my_packet->team == 1) {
			for (int j = 0; j < 5; j++)
			{

				if (mainroom.red[j].id == my_packet->id)
				{
					mainroom.red[j].active = false;
					mainroom.red[j].id = -1;
					for (int c = 0; c < 5; c++)
					{
						if (mainroom.blue[c].active == false)
						{
							mainroom.blue[c].active = true;
							mainroom.blue[c].id = my_packet->id;
							break;
						}
					}
					break;

				}

			}
		}
		else if (my_packet->team == 0)
		{
			for (int j = 0; j < 5; j++)
			{
				if (mainroom.blue[j].id == my_packet->id) {
					mainroom.blue[j].active = false;
					mainroom.blue[j].id = -1;
					for (int c = 0; c < 5; c++)
					{
						if (!mainroom.red[c].active)
						{
							mainroom.red[c].active = true;
							mainroom.red[c].id = my_packet->id;
							break;
						}
					}
					break;
				}
			}
		}
		break;
	}

	case CS_COLL:
	{
		sc_packet_collision *my_packet = reinterpret_cast<sc_packet_collision *>(ptr);
		if(my_packet->state==0)
			printf("당신의 %d번째 총알이 %d번플레이어의 몸통을 꿰뚫었습니다.\n", my_packet->id, my_packet->index);
		else if(my_packet->state==1)
			printf("당신의 %d번째 총알이 %d번플레이어의 뚝배기를 깼습니다.\n", my_packet->id, my_packet->index);
		else
			printf("당신의 %d번쨰 총알이 %d번플레이어의 어딘가를 스쳤습니다.\n", my_packet->id, my_packet->index);
		break;

	}
	case CS_roomcontrol:
	{


		cs_packet_roomveiw *my_packet = reinterpret_cast<cs_packet_roomveiw *>(ptr);
		roomdata[my_packet->num % 6].timereckoning = 0;
		roomdata[my_packet->num % 6].active = true;
		roomdata[my_packet->num % 6].playtime = my_packet->playtime;
		roomdata[my_packet->num % 6].num = my_packet->num;
		if (my_packet->playtime == 0)
		{
			roomdata[my_packet->num % 6].statenum = false;
		}

		roomdata[my_packet->num % 6].count = my_packet->count;
		break;

	}
	case CS_addAIplayer: {
		cs_packet_aipacket *my_packet = reinterpret_cast<cs_packet_aipacket *>(ptr);
		if (my_packet->team == 0)
		{
			mainroom.red[redai].active = true;
			mainroom.red[redai].id = my_packet->id;

		}
		else if (my_packet->team == 1)
		{
			mainroom.blue[blueai].active = true;
			mainroom.blue[blueai].id = my_packet->id;
			
		}
	}break;
	case CS_deleteAIplayer: {
		cs_packet_aipacket *my_packet = reinterpret_cast<cs_packet_aipacket *>(ptr);
		for (int c = 0; c < 5; c++)
		{
			if (mainroom.red[c].id == my_packet->id)
			{
				mainroom.red[c].active = false;
				mainroom.red[c].id = -1;
				break;
			}
		}
		for (int c = 0; c < 5; c++)
		{
			if (mainroom.blue[c].id == my_packet->id)
			{
				mainroom.blue[c].active = false;
				mainroom.blue[c].id = -1;
				break;
			}
		}
		//mainroom.player[i].id = -1
	}break;
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
		break;
	}
}
void ReadPacket(SOCKET sock)
{
	DWORD iobyte, ioflag = 0;
	
	int ret = WSARecv(sock, &recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
	if (ret) {
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

	while (0 != iobyte) {//패킷 재조립
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (iobyte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			iobyte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}

void clienterror()
{
	exit(-1);
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);

		m_pd3dDeviceContext->OMSetRenderTargets(0, NULL, NULL);

		if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
		if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
		if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();

		m_pDXGISwapChain->ResizeBuffers(1, m_nWndClientWidth, m_nWndClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

		CreateRenderTargetDepthStencilView();

		if (m_pCamera) m_pCamera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;

	case WM_SOCKET:
	{

		if (WSAGETSELECTERROR(lParam)) {
			closesocket((SOCKET)wParam);
			clienterror();
			break;
		}
		switch (WSAGETSELECTEVENT(lParam)) {
		case FD_READ:

			ReadPacket((SOCKET)wParam);
			break;
		case FD_CLOSE:
			closesocket((SOCKET)wParam);
			clienterror();
			break;
		}
	}
	}
	return(0);
}

void CGameFramework::BuildObjects()
{
	AllocConsole();
	freopen("CONIN$", "rb", stdin);
	freopen("CONOUT$", "wb", stdout);
	freopen("CONOUT$", "wb", stderr);

	CreateShaderVariables();


	//CCubeMeshDiffused *pCubeMesh = new CCubeMeshDiffused(m_pd3dDevice, 4.0f, 12.0f, 4.0f, D3DXCOLOR(0.5f, 0.0f, 0.0f, 0.0f));
	CMarineMeshDiffused *pSkinMesh = new CMarineMeshDiffused(m_pd3dDevice, MARINE_TXT);


	m_pPlayer = new CTerrainPlayer(1);
	m_pPlayer->AddRef();
	m_pPlayer->SetMesh(pSkinMesh);
	//	m_pPlayer->SetTexture(m_pd3dDevice);
	m_pPlayer->ChangeCamera(m_pd3dDevice, THIRD_PERSON_CAMERA, 0.0f);//3인칭이 원래값


	m_pPlayerShader = new CAnimationShader();
	m_pPlayerShader->CreateShader(m_pd3dDevice);
	m_pPlayerShader->CreateShaderVariables(m_pd3dDevice);
	m_pPlayerShader->CreateAnimationBuffer(m_pd3dDevice);
	//	pPlayerShader->CreateRasterizerState(m_pd3dDevice);
	m_pPlayerShader->BuildObjects(m_pd3dDevice, pSkinMesh);
	m_pPlayerShader->AddRef();
	m_pPlayer->SetShader(m_pPlayerShader);
	//CShader *pPlayerBoundingShader = new CBoundingCubeShader();
//	pPlayerBoundingShader->CreateShader(m_pd3dDevice);
//	m_pPlayer->SetBoundingShader(pPlayerBoundingShader);

//총알 매쉬와 텍스쳐 set
	CBulletMeshDiffused * pBulletMeshDiffused = new CBulletMeshDiffused(m_pd3dDevice, "bullet/BulletMesh.txt");
	CObjectsShader *pBulletShader = new CObjectsShader(15);
	for (int j = 0; j < 10; j++)
	{
		for (int i = 0; i < 15; i++)
			m_pBullets[j][i] = new CGameObject(1);
	}
	m_pBullet = new CBulletObject(1);
	
		m_pBullet->AddRef();
		m_pBullet->SetMesh(pBulletMeshDiffused);
		
		pBulletShader->CreateShader(m_pd3dDevice, m_pBullet->GetMeshType());
		m_pBullet->SetShader(pBulletShader);

	
		for (int j = 0; j < 10; j++)
		{
			for (int i = 0; i < 15; i++) {
				m_pBullets[j][i]->AddRef();
				m_pBullets[j][i]->SetMesh(pBulletMeshDiffused);
				if (i == 0)
					pBulletShader->CreateShader(m_pd3dDevice, m_pBullets[0][0]->GetMeshType());
				m_pBullets[j][i]->SetShader(pBulletShader);
			}
		}

	m_pScene = new CScene();
	m_pScene->SetPlayer(m_pPlayer);
	m_pScene->SetBullet(m_pBullet);
	m_pScene->BuildObjects(m_pd3dDevice);


	m_pScene1 = new CTitileScene();
	//m_pScene1->SetPlayer(m_pPlayer);
	m_pScene1->BuildObjects(m_pd3dDevice);

	for (int j = 0; j < 10; j++)
	{
		for (int i = 0; i < 15; i++) 
		m_pBullets[j][i]->SetMaterial(m_pBullet->GetMaterial());
	}
	//m_pScene = new CTitileScene();
	//m_pScene->SetPlayer(m_pPlayer);
	//m_pScene->BuildObjects(m_pd3dDevice);
	//
	CHeightMapTerrain *pTerrain = m_pScene->GetTerrain();
	float fHeight = pTerrain->GetHeight(pTerrain->GetWidth()*0.5f, pTerrain->GetLength()*0.5f, false) + 1300.0f;
	m_pPlayer->SetPosition(D3DXVECTOR3(pTerrain->GetWidth()*0.5f, 416, pTerrain->GetLength()*0.5f));
	m_pPlayer->SetPlayerUpdatedContext(pTerrain);
	m_pPlayer->SetCameraUpdatedContext(pTerrain);
//	m_pPlayer->Rotate()
	
//	m_pPlayer->SetPosition(D3DXVECTOR3(0.f, 0.f, 0.f));

	m_pBoundingBox = new CGameObject(1);
	m_pBoundingBox->AddRef();
	

	//CMaterialColors  *pBoundingColor = new CMaterialColors();
	//pBoundingColor->m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//pBoundingColor->m_d3dxcAmbient = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	//pBoundingColor->m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	//pBoundingColor->m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	//CMaterial *pBoundingMaterial = new CMaterial(pBoundingColor);
	//m_pBoundingBox->SetMaterial(pBoundingMaterial);
	CBoundingCubeMesh *pBoundingMesh = new CBoundingCubeMesh(m_pd3dDevice, pSkinMesh->GetBoundingCube().m_d3dxvMaximum, pSkinMesh->GetBoundingCube().m_d3dxvMinimum);
	m_pBoundingBox->SetMesh(pBoundingMesh);
	m_pPlayer->SetBoundingCubes(pBoundingMesh);


	CShader *pBoundingShader = new CBoundingCubeShader();
	pBoundingShader->CreateShader(m_pd3dDevice);
	m_pBoundingBox->SetShader(pBoundingShader);
//	m_pBoundingBox->SetPosition(m_pPlayer->GetPosition());
	//m_pBoundingBox->SetPosition(1300,416,1300);
//	m_pBoundingBox->CreateShaderVariables(m_pd3dDevice);
	m_pBoundingBox->CreateRasterizerState(m_pd3dDevice);
	//cout << pSkinMesh->GetBoundingCube().m_d3dxvMaximum.x << " " << pSkinMesh->GetBoundingCube().m_d3dxvMaximum.y << " " << pSkinMesh->GetBoundingCube().m_d3dxvMaximum.z << endl;
	//cout << pSkinMesh->GetBoundingCube().m_d3dxvMinimum.x << " " << pSkinMesh->GetBoundingCube().m_d3dxvMinimum.y << " " << pSkinMesh->GetBoundingCube().m_d3dxvMinimum.z << endl;
	m_pCamera = m_pPlayer->GetCamera();
	m_pCamera->SetViewport(m_pd3dDeviceContext, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	m_pCamera->GenerateViewMatrix();
	//	D3DXVECTOR3 t = m_pPlayer->GetPosition();
	m_pScene->SetCamera(m_pCamera);



	d3dViewport = m_pCamera->GetViewport();
}

void CGameFramework::ReleaseObjects()
{
	ReleaseShaderVariables();

	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;

	if (m_pPlayer) delete m_pPlayer;
}

void CGameFramework::CreateShaderVariables()
{
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dBufferDesc.ByteWidth = sizeof(VS_CB_WORLD_MATRIX);
	m_pd3dDevice->CreateBuffer(&d3dBufferDesc, NULL, &CGameObject::m_pd3dcbWorldMatrix);

	d3dBufferDesc.ByteWidth = sizeof(D3DXCOLOR) * 4;
	m_pd3dDevice->CreateBuffer(&d3dBufferDesc, NULL, &CGameObject::m_pd3dcbMaterialColors);

	CCamera::CreateShaderVariables(m_pd3dDevice);
	CTexture::CreateShaderVariables(m_pd3dDevice);
}

void CGameFramework::ReleaseShaderVariables()
{
	CGameObject::ReleaseShaderVariables();
	CCamera::ReleaseShaderVariables();
	CTexture::ReleaseShaderVariables();
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	if (GetKeyboardState(pKeysBuffer) && m_pScene) bProcessedByScene = m_pScene->ProcessInput(pKeysBuffer);
	if (!bProcessedByScene)
	{
		DWORD dwDirection = 0;
		if (pKeysBuffer['W'] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeysBuffer['S'] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeysBuffer['A'] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeysBuffer['D'] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeysBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeysBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
		
		if ((movechek != 0 && dwDirection == 0) || dwDirection != 0) {

			if (dwDirection == 0)
			{
				movechek = 0;
			}
			if (dwDirection != ddwDirection)
			{
				movechek = 0;
			}
			if (movechek == 0) {
				cs_packet_input *my_packet = reinterpret_cast<cs_packet_input *>(send_buffer);
				my_packet->size = sizeof(my_packet);
				send_wsabuf.len = sizeof(my_packet);
				DWORD iobyte;
				my_packet->type = dwDirection;
				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					//printf("Error while sending packet [%d]", error_code);
				}
				movechek = true;
				ddwDirection = dwDirection;
				if (dwDirection == 0)
				{
					movechek = 0;
				}
			}

		}
		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		if (GetCapture() == m_hWnd)
		{
			SetCursor(NULL);
			GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 30.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 30.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}

		if (cxDelta != 0.0f) //마우스는 키보드랑 달라서 계속 마우스값이 찍히는게아님 0갔다가 왔다리 갔다리 해서 이렇게
		{
			cs_packet_input *my_packet = reinterpret_cast<cs_packet_input *>(send_buffer);
			my_packet->size = sizeof(my_packet);
			send_wsabuf.len = sizeof(my_packet);
			DWORD iobyte;
			if (cxDelta < 0.0f)
				my_packet->type = CS_LEFTROTATION;
			else if (cxDelta > 0.0f)
				my_packet->type = CS_RIGHTROTATION;
			WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		}
		if (cyDelta != 0.0f) //마우스는 키보드랑 달라서 계속 마우스값이 찍히는게아님 0갔다가 왔다리 갔다리 해서 이렇게
		{
		//	if(m_pCamera->sumrotate)
			cs_packet_input *my_packet = reinterpret_cast<cs_packet_input *>(send_buffer);
			my_packet->size = sizeof(my_packet);
			send_wsabuf.len = sizeof(my_packet);
			DWORD iobyte;
			if (cyDelta < 0.0f) {
				if (m_pCamera->sumrotate > -80) 
					my_packet->type = CS_UPROTATION;
				else
					m_pCamera->sumrotate = -80;
			

			}
			else if (cyDelta > 0.0f)
			{
				if (m_pCamera->sumrotate < -10) 
					my_packet->type = CS_DOWNROTATION;
				else
					m_pCamera->sumrotate = -10;
				
			}
			WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		}

	}
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::AnimateObjects()
{
	float fTimeElapsed = m_GameTimer.GetTimeElapsed();


	if (m_pPlayer) m_pPlayer->Animate(fTimeElapsed, NULL);
	if (m_pPlayerShader)m_pPlayerShader->AnimateObjects(m_GameTimer.GetTimeElapsed());
	if (m_pScene) m_pScene->AnimateObjects(fTimeElapsed);
//	if (m_pBoundingBox)m_pBoundingBox->Animate(fTimeElapsed,NULL);
//	if (m_pBullet)m_pBullet->Animate(fTimeElapsed);
	
}

//#define _WITH_PLAYER_TOP

void CGameFramework::veiwProcessInput()
{
	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	if (GetKeyboardState(pKeysBuffer) && m_pScene) bProcessedByScene = m_pScene->ProcessInput(pKeysBuffer);
	if (!bProcessedByScene)
	{
		DWORD dwDirection = 0;
		if (pKeysBuffer['W'] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeysBuffer['S'] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeysBuffer['A'] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeysBuffer['D'] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeysBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeysBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;

		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		if (GetCapture() == m_hWnd)
		{
			SetCursor(NULL);
			GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}

		if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (cxDelta || cyDelta)
			{
				if (pKeysBuffer[VK_RBUTTON] & 0xF0)
					m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
				else
					m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
			}
			if (dwDirection) m_pPlayer->Move(dwDirection, 200.0f * m_GameTimer.GetTimeElapsed(), true);
		}
	}
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}
void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick();

	if (veiwtype)
	{
		veiwProcessInput();
	}
	else {
		ProcessInput();
	}
	AnimateObjects();

	if (m_pScene) m_pScene->OnPreRender(m_pd3dDeviceContext);

	float fClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	if (m_pd3dRenderTargetView) m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fClearColor);
	if (m_pd3dDepthStencilView) m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	if (Scene) {
		if (m_pScene1) m_pScene1->Render(m_pd3dDeviceContext, m_pCamera);
	}
	else
	{
		m_pCamera->SetViewport(m_pd3dDeviceContext);

		m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);


		if (m_pPlayer) m_pPlayer->UpdateShaderVariables(m_pd3dDeviceContext);
		if (m_pScene) m_pScene->Render(m_pd3dDeviceContext, m_pCamera);

#ifdef _WITH_PLAYER_TOP
		m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
#endif
		if (m_pPlayer) m_pPlayer->Render(m_pd3dDeviceContext, m_pCamera);
		D3DXMATRIX player = m_pPlayer->m_d3dxmtxLocal;
		const D3DXVECTOR3 up = { 0.f,1.0f,0.f };
		D3DXMATRIX  Rotate;
		D3DXMatrixIdentity(&Rotate);
		D3DXMatrixRotationAxis(&Rotate, &up, D3DXToRadian(180));
		D3DXMatrixMultiply(&m_pPlayer->m_d3dxmtxLocal, &Rotate, &m_pPlayer->m_d3dxmtxLocal);
		if (m_pPlayerShader)m_pPlayerShader->Render(m_pd3dDeviceContext, &m_pPlayer->m_d3dxmtxLocal);
		m_titleObjects[11]->Render(m_pd3dDeviceContext, m_pCamera);
		m_titleObjects[12]->Render(m_pd3dDeviceContext, m_pCamera);
		m_titleObjects[13]->Render(m_pd3dDeviceContext, m_pCamera);
		m_titleObjects[14]->Render(m_pd3dDeviceContext, m_pCamera);
		//	m_pBoundingBox->SetPosition(m_pPlayer->GetPosition());
		m_pBoundingBox->m_d3dxmtxLocal = m_pPlayer->m_d3dxmtxLocal;
	//	if (m_pBoundingBox)m_pBoundingBox->Render(m_pd3dDeviceContext, m_pCamera);
		if (m_pBullets) {
			for (int j = 0; j < 10; j++)
			{
				for (int i = 0; i < 15; i++) {
					m_pBullets[j][i]->Render(m_pd3dDeviceContext, m_pCamera);
				}
			}
		}
		if (veiwtype)
		{
			if (m_pPlayer)
				{
					m_pPlayer->ChangeCamera(m_pd3dDevice, SPACESHIP_CAMERA, m_GameTimer.GetTimeElapsed());
					m_pCamera = m_pPlayer->GetCamera();
					m_pScene->SetCamera(m_pCamera);
				}
		}
	}
	if (NowScene == 1) {
		for (int j = 0; j < 5; j++)
		{

			if (mainroom.red[j].active)
			{
				if (mainroom.red[j].id > 4999)
				{
					swprintf_s(name, L"ID : %d번 AI플레이어", mainroom.red[j].id - 5000);
				}
				else
					swprintf_s(name, L"ID : %d번 플레이어", mainroom.red[j].id);
				font->Render_Font(name, 20 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 12.5, (d3dViewport.Height / 100 * 22.91) + (d3dViewport.Height / 100 * 9.79) * j, 0xff0000ff);
				if (mainroom.red[j].id == g_myid) {
					font->Render_Font(_T("나->"), 22 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 4.68, d3dViewport.Height / 100 * 22.9 + d3dViewport.Height / 100 * 9.79 * j, 0xffffffff);
				}
			}
		}
		for (int j = 0; j < 5; j++)
		{
			if (mainroom.blue[j].active) {
				if (mainroom.blue[j].id > 4999)
				{
					swprintf_s(name, L"ID : %d번 AI플레이어", mainroom.blue[j].id - 5000);
				}
				else
					swprintf_s(name, L"ID : %d번 플레이어", mainroom.blue[j].id);
				font->Render_Font(name, 20 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 51.42, (d3dViewport.Height / 100 * 22.91) + (d3dViewport.Height / 100 * 9.79)  * j, 0xffff0000);
				if (mainroom.blue[j].id == g_myid) {
					font->Render_Font(_T("<-나"), 22 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 87.82, (d3dViewport.Height / 100 * 22.91) + (d3dViewport.Height / 100 * 9.79)  * j, 0xffffffff);
				}
			}
		}
	}
	if (NowScene == 2) {
		auto static start_time = high_resolution_clock::now();
		auto du = high_resolution_clock::now() - start_time;
		auto msec = std::chrono::duration_cast<chrono::milliseconds>(du);
		/// 0.1당 
		//_stprintf_s(roomdata[0].name, _T("#0번방 초보자만 %d / %d "), roomdata[0].redteam, roomdata[0].blueteam);
		//printf("%s", roomdata[0].name);
		for (int j = 0; j < 6; j++) {
			if (roomdata[j].active)
			{
				swprintf_s(roomdata[j].name, L"#%d번방 너만 오면 고 %d/10", roomdata[j].num + 1, roomdata[j].count);
				if (roomdata[j].playtime != 0)
				{

					if (msec.count() > 1000) {
						start_time = high_resolution_clock::now();
						roomdata[j].timereckoning = 1;
						roomdata[j].playtime += roomdata[j].timereckoning;

					}

					swprintf_s(roomdata[j].text, L"게임중 플레이시간 : %d : %d  ", roomdata[j].playtime / 60, roomdata[j].playtime % 60);


				}
				switch (j)
				{
				case 0:
					font->Render_Font(roomdata[j].name, 20 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 10.62, d3dViewport.Height / 100 * 25, 0xffffffff);
					if (roomdata[j].playtime == 0)
					{
						font->Render_Font(_T("대기방"), 17 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 37.62, d3dViewport.Height / 100 * 30, 0xffffffff);
					}
					else
					{
						font->Render_Font(roomdata[j].text, 17 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 15.62, d3dViewport.Height / 100 * 30, 0xffffffff);
					}
					break;
				case 1:
					font->Render_Font(roomdata[j].name, 20 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 51.56, d3dViewport.Height / 100 * 25, 0xffffffff);
					if (roomdata[j].playtime == 0)
					{
						font->Render_Font(_T("대기방"), 17 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 78.56, d3dViewport.Height / 100 * 30, 0xffffffff);
					}
					else
					{
						font->Render_Font(roomdata[j].text, 17 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 56.56, d3dViewport.Height / 100 * 30, 0xffffffff);
					}
					break;
				case 2:
					font->Render_Font(roomdata[j].name, 20 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 10.62, d3dViewport.Height / 100 * 41.66, 0xffffffff);
					if (roomdata[j].playtime == 0)
					{
						font->Render_Font(_T("대기방"), 17 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 37.62, d3dViewport.Height / 100 * 46.66, 0xffffffff);
					}
					else
					{
						font->Render_Font(roomdata[j].text, 17 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 15.62, d3dViewport.Height / 100 * 46.66, 0xffffffff);
					}
					break;
				case 3:
					font->Render_Font(roomdata[j].name, 20 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 51.56, d3dViewport.Height / 100 * 41.66, 0xffffffff);
					if (roomdata[j].playtime == 0)
					{
						font->Render_Font(_T("대기방"), 17 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 78.56, d3dViewport.Height / 100 * 46.66, 0xffffffff);
					}
					else
					{
						font->Render_Font(roomdata[j].text, 17 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 56.56, d3dViewport.Height / 100 * 46.66, 0xffffffff);
					}
					break;
				case 4:
					font->Render_Font(roomdata[j].name, 20 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 10.62, d3dViewport.Height / 100 * 58.75, 0xffffffff);
					if (roomdata[j].playtime == 0)
					{
						font->Render_Font(_T("대기방"), 17 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 37.62, d3dViewport.Height / 100 * 63.75, 0xffffffff);
					}
					else
					{
						font->Render_Font(roomdata[j].text, 17 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 15.62, d3dViewport.Height / 100 * 63.75, 0xffffffff);
					}
					break;
				case 5:
					font->Render_Font(roomdata[j].name, 20 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 51.56, d3dViewport.Height / 100 * 58.75, 0xffffffff);
					if (roomdata[j].playtime == 0)
					{
						font->Render_Font(_T("대기방"), 17 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 78.56, d3dViewport.Height / 100 * 63.75, 0xffffffff);
					}
					else
					{
						font->Render_Font(roomdata[j].text, 17 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 56.56, d3dViewport.Height / 100 * 63.75, 0xffffffff);
					}
					break;
				default:
					break;
				}

			}
			////	font->Render_Font(roomdata[0].name, 20, 68, 120, 0xffffffff);
			//font->Render_Font(roomdata[0].name, 20, 330, 120, 0xffffffff);
			//	font->Render_Font(roomdata[0].name, 20, 68, 200, 0xffffffff);
			//font->Render_Font(roomdata[0].name, 20, 330, 200, 0xffffffff);
			//font->Render_Font(roomdata[0].name, 20, 68, 282, 0xffffffff);
			//	font->Render_Font(roomdata[0].name, 20, 330, 282, 0xffffffff);
		}
		switch (selectroomindex)
		{
		case 1:
			font->Render_Font(_T("-----------------------------------------"), 18 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 9.37, d3dViewport.Height / 100 * 22.5, 0xff00ffff);
			font->Render_Font(_T("-----------------------------------------"), 18 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 9.37, d3dViewport.Height / 100 * 36.04, 0xff00ffff);
			break;
		case 2:
			font->Render_Font(_T("-----------------------------------------"), 18 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 50.62, d3dViewport.Height / 100 * 22.5, 0xff00ffff);
			font->Render_Font(_T("-----------------------------------------"), 18 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 50.62, d3dViewport.Height / 100 * 36.04, 0xff00ffff);
			break;
		case 3:
			font->Render_Font(_T("-----------------------------------------"), 18 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 9.37, d3dViewport.Height / 100 * 38.95, 0xff00ffff);
			font->Render_Font(_T("-----------------------------------------"), 18 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 9.37, d3dViewport.Height / 100 * 53.12, 0xff00ffff);
			break;
		case 4:
			font->Render_Font(_T("-----------------------------------------"), 18 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 50.62, d3dViewport.Height / 100 * 38.95, 0xff00ffff);
			font->Render_Font(_T("-----------------------------------------"), 18 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 50.62, d3dViewport.Height / 100 * 53.12, 0xff00ffff);
			break;
		case 5:
			font->Render_Font(_T("-----------------------------------------"), 18 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 9.37, d3dViewport.Height / 100 * 56.25, 0xff00ffff);
			font->Render_Font(_T("-----------------------------------------"), 18 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 9.37, d3dViewport.Height / 100 * 70, 0xff00ffff);
			break;
		case 6:
			font->Render_Font(_T("-----------------------------------------"), 18 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 50.62, d3dViewport.Height / 100 * 56.25, 0xff00ffff);
			font->Render_Font(_T("-----------------------------------------"), 18 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 50.62, d3dViewport.Height / 100 * 70, 0xff00ffff);
			break;
		default:
			break;
		}
		font->Render_Font(roomindexnum, 18 * d3dViewport.Width / 640, d3dViewport.Width / 100 * 91.40, d3dViewport.Height / 100 * 45, 0xff00ffff);
	}
		m_pDXGISwapChain->Present(0, 0);
	m_GameTimer.GetFrameRate(m_pszBuffer + 12, 37);
	::SetWindowText(m_hWnd, m_pszBuffer);
}
