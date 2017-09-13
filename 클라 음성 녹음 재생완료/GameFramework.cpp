//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"
#include "Shader.h"


#include <CommDlg.h>
#include <MMSystem.h>
#include <fstream>
#include <cstdlib>


#ifndef RESOURCE_H_INCLUDED
#define RESOURCE_H_INCLUDED
#define INP_BUFFER_SIZE 16384
#define NUM 20000
#define APP_SAVE 1003
#define APP_EXIT 1004
#endif // RESOURCE_H_INCLUDED

SOCKET g_mysocket;
WSABUF	send_wsabuf;
char 	send_buffer[BUF_SIZE];
WSABUF	recv_wsabuf;
char	recv_buffer[BUF_SIZE];
char	packet_buffer[BUF_SIZE];
DWORD		in_packet_size = 0;
int		saved_packet_size = 0;
int		g_myid;
HWND hendle;
CGameTimer						m_GameTimer;
CPlayer	*m_pPlayer;
DWORD ddwDirection;
float LcxDelta;
bool movechek = 0;
bool rotachek = 0;
extern CInstancedObjectsShader			**m_ppInstancingShaders;
extern  CHeightMapTerrain *pTerrain;
CScene							*m_pScene;
struct Overlap_ex {
	WSAOVERLAPPED original_overlap;
	int operation;
	WSABUF wsabuf;
	unsigned char iocp_buffer[MAX_BUFF_SIZE];
};



int number, length, byte_samp, byte_sec, bit_samp;
static int sampleRate = 11025;
const int NUMPTS = 11025 * 10;
bool mono = TRUE;
bool PLAY = FALSE;
errno_t wavfile;
char * filename;
int s_rate = 11025;
double limit = 10000.0;
FILE * stream;
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
int readSample(int number, bool leftchannel);
void Wav(char *c, HWND hWnd);
char szAppName[] = "Recorder";
bool f5key = false;

static HMENU hMenu;
static HPEN hPen;
static BOOL bRecording, bPlaying, bEnding, bTerminating;
static DWORD dwDataLength, dwRepetitions = 1;
static HWAVEIN hWaveIn;
static HWAVEOUT hWaveOut;
static PBYTE pBuffer1, pBuffer2, pSaveBuffer, pNewBuffer;
static PWAVEHDR pWaveHdr1, pWaveHdr2;
static TCHAR szOpenError[] = TEXT("Error opening waveform audio!");
static TCHAR szMemError[] = TEXT("Error allocating memory!");
static WAVEFORMATEX waveform;

HDC hDC;
POINT pt[NUM];
BOOL fSuccess = FALSE;



int readSample(int number, bool leftchannel)
{
	/*
			      Reads sample number, returns it as an int, if
				        this.mono==false we look at the leftchannel bool
						      to determine which to return.
							         number is in the range [0,length/byte_samp]
									        returns 0xefffffff on failure
											    */
		if (number >= 0 && number<length / byte_samp)
			
		{
				// go to beginning of the file
				rewind(stream);
				// we start reading at sample_number * sample_size + header length
				int offset = number * 1 + 44;
				// unless this is a stereo file and the rightchannel is requested.
				if (!mono && !leftchannel)
				{
						offset += byte_samp / 2;
				
				}
				// read this many bytes;
				int amount;
				amount = byte_samp;
				fseek(stream, offset, SEEK_CUR);
				short sample = 0;
				fread((void *)&sample, 1, amount, stream);
				return sample;
		}
		else
		{
				return (int)0xefffffff;
		}

}

void Wav(char *c, HWND hWnd)
{
		filename = new char[strlen(c) + 1];
		strcpy_s(filename, strlen(c) + 1, c);
		wavfile = fopen_s(&stream, filename, "r");
		if (stream == NULL)
		{
		}
		else
		{
			
				// declare a char buff to store some values i
				char *buff = new char[5];
				buff[4] = '\0';
				// read the first 4 bytes
				fread((void *)buff, 1, 4, stream);
				// the first four bytes should be 'RIFF'
				if (strcmp((char *)buff, "RIFF") == 0)
				{

						// read byte 8,9,10 and 11
						fseek(stream, 4, SEEK_CUR);
						fread((void *)buff, 1, 4, stream);
						// this should read "WAVE"
						if (strcmp((char *)buff, "WAVE") == 0)
						{
								// read byte 12,13,14,15
								fread((void *)buff, 1, 4, stream);
								// this should read "fmt "
								if (strcmp((char *)buff, "fmt ") == 0)
								{
										fseek(stream, 20, SEEK_CUR);
										// final one read byte 36,37,38,39
										fread((void *)buff, 1, 4, stream);
										if (strcmp((char *)buff, "data") == 0)
										{
												// Now we know it is a wav file, rewind the stream
												rewind(stream);
												// now is it mono or stereo ?
												fseek(stream, 22, SEEK_CUR);
												fread((void *)buff, 1, 2, stream);
												if (buff[0] == 0x02)
												{
														mono = false;
												}
												else
												{
														mono = true;
												}
												// read the sample rate
												
												fread((void *)&s_rate, 1, 4, stream);
											
												fread((void *)&byte_sec, 1, 4, stream);
											
												byte_samp = 0;
											
												fread((void *)&byte_samp, 1, 2, stream);
											
												bit_samp = 0;
											
												fread((void *)&bit_samp, 1, 2, stream);
											
												fseek(stream, 4, SEEK_CUR);
											
												fread((void *)&length, 1, 4, stream);
											
										}
									
								}
							
						}
					
				}
			
				delete buff;
		}
}

CGameFramework::CGameFramework()
{
	AllocConsole();
	freopen("CONOUT$", "wt", stdout);
	freopen("CONIN$", "rb", stdin);
	m_pd3dDevice = NULL;
	m_pDXGISwapChain = NULL;
	m_pd3dRenderTargetView = NULL;
	m_pd3dDeviceContext = NULL;

	m_pd3dDepthStencilBuffer = NULL;
	m_pd3dDepthStencilView = NULL;

	m_pScene = NULL;

	m_pPlayer = NULL;
	m_pCamera = NULL;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	_tcscpy_s(m_pszBuffer, _T("LabProject ("));

	pWaveHdr1 = reinterpret_cast <PWAVEHDR> (malloc(sizeof(WAVEHDR)));
	pWaveHdr2 = reinterpret_cast <PWAVEHDR> (malloc(sizeof(WAVEHDR)));
	// Allocate memory for save buffer
	pSaveBuffer = reinterpret_cast <PBYTE> (malloc(1));
	srand(timeGetTime());
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
	
	BuildObjects();


	return(true);
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
	case WM_RBUTTONDOWN:
		SetCapture(hWnd);
		GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	//	ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		
		break;
	default:
		ReleaseCapture();
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
			::PostQuitMessage(0);
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
			break;
		}
		case VK_F10:
			break;
		case VK_F5:
			_fcloseall();
			bEnding = TRUE;
			f5key = false;
			break;
		case VK_F6:
			bPlaying = TRUE;
			waveform.wFormatTag = WAVE_FORMAT_PCM;
			waveform.nChannels = 1;
			waveform.nSamplesPerSec = 11025;
			waveform.nAvgBytesPerSec = 11025;
			waveform.nBlockAlign = 1;
			waveform.wBitsPerSample = 8;
			waveform.cbSize = 0;
			waveInReset(hWaveIn);
			waveOutReset(hWaveOut);
			if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveform, (DWORD)hWnd, 0, CALLBACK_WINDOW))
			{

			}
			Wav("temp.wav", hWnd);
			RECT rc;
			//GetClientRect(hWnd, &rc);
			PLAY = TRUE;
		//	InvalidateRect(hWnd, &rc, TRUE);
			
			break;
		default:
			break;
		}
		break;

	
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_F5:
			if (f5key == false) {
				f5key = true;
				waveOutReset(hWaveOut);
				waveInReset(hWaveIn);
				pBuffer1 = reinterpret_cast <PBYTE> (malloc(INP_BUFFER_SIZE));
				pBuffer2 = reinterpret_cast <PBYTE> (malloc(INP_BUFFER_SIZE));
				if (!pBuffer1 || !pBuffer2)
				{
					if (pBuffer1) free(pBuffer1);
					if (pBuffer2) free(pBuffer2);
					return;
				}
				waveform.wFormatTag = WAVE_FORMAT_PCM;
				waveform.nChannels = 1;
				waveform.nSamplesPerSec = 11025;
				waveform.nAvgBytesPerSec = 11025;
				waveform.nBlockAlign = 1;
				waveform.wBitsPerSample = 8;
				waveform.cbSize = 0;
				if (waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD)hWnd, 0, CALLBACK_WINDOW))
				{
					free(pBuffer1);
					free(pBuffer2);
				}
				pWaveHdr1->lpData = reinterpret_cast <CHAR*>(pBuffer1);
				pWaveHdr1->dwBufferLength = INP_BUFFER_SIZE;
				pWaveHdr1->dwBytesRecorded = 0;
				pWaveHdr1->dwUser = 0;
				pWaveHdr1->dwFlags = 0;
				pWaveHdr1->dwLoops = 1;
				pWaveHdr1->lpNext = NULL;
				pWaveHdr1->reserved = 0;
				waveInPrepareHeader(hWaveIn, pWaveHdr1, sizeof(WAVEHDR));
				pWaveHdr2->lpData = reinterpret_cast <CHAR*>(pBuffer2);
				pWaveHdr2->dwBufferLength = INP_BUFFER_SIZE;
				pWaveHdr2->dwBytesRecorded = 0;
				pWaveHdr2->dwUser = 0;
				pWaveHdr2->dwFlags = 0;
				pWaveHdr2->dwLoops = 1;
				pWaveHdr2->lpNext = NULL;
				pWaveHdr2->reserved = 0;
				waveInPrepareHeader(hWaveIn, pWaveHdr2, sizeof(WAVEHDR));
				printf("이거실행중");
			}
			break;

		default:
			break;
		}
	}
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
		int id = my_packet->id;
		if (first_time) {
			printf("접속성공");
			first_time = false;
			g_myid = id;
		}
		m_pPlayer->SetPosition(D3DXVECTOR3(my_packet->mtx._41, my_packet->mtx._42, my_packet->mtx._43));
		break;
	}
	case SC_POS:
	{
		sc_packet_pos *my_packet = reinterpret_cast<sc_packet_pos *>(ptr);
		int other_id = my_packet->id;

		if (other_id == g_myid) {
			m_pPlayer->SetPosition(D3DXVECTOR3(my_packet->mtx._41, my_packet->mtx._42, my_packet->mtx._43));
		}
		else {

			m_ppInstancingShaders[0]->m_ppObjects[0]->SetPosition(my_packet->mtx._41, pTerrain->GetHeight(my_packet->x, my_packet->y) + 10, my_packet->mtx._43);
			//am_ppObjects[0]->SetPosition(my_packet->x, 0, my_packet->y);
		}
	   // m_pPlayer->Move(dwDirection, 50.0f * m_GameTimer.GetTimeElapsed(), true);
		break;
	}
	case SC_ROTATE:
	{
		sc_packet_rotation *packet_rotaion = reinterpret_cast<sc_packet_rotation *>(ptr);
		int other_id = packet_rotaion->id;
		if (other_id == g_myid) {
			if (packet_rotaion->lr == CS_RIGHTROTATION)
				m_pPlayer->Rotate(0, 2, 0);
			else if(packet_rotaion->lr == CS_LEFTROTATION)
				m_pPlayer->Rotate(0, -2, 0);
		}
		else {
			if (packet_rotaion->lr == CS_RIGHTROTATION)
				m_ppInstancingShaders[0]->m_ppObjects[0]->Rotate(0, 2, 0);
			else if (packet_rotaion->lr == CS_LEFTROTATION)
				m_ppInstancingShaders[0]->m_ppObjects[0]->Rotate(0, -2, 0);
		}
		break;
	}
	case SC_CHAT:
	{
		break;
	}
	case SC_ATTACK:
	{
		
		break;
	}
	case SC_HIT:
	{
		
		break;
	}
	case SC_IDSERCH:
	{
		
		break;
	}

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
	
	hMenu = GetMenu(hWnd);

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
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case MM_WIM_OPEN:
		pSaveBuffer = reinterpret_cast <PBYTE>(realloc(pSaveBuffer, 1));
		waveInAddBuffer(hWaveIn, pWaveHdr1, sizeof(WAVEHDR));
		waveInAddBuffer(hWaveIn, pWaveHdr2, sizeof(WAVEHDR));
		bRecording = TRUE;
		bEnding = FALSE;
		dwDataLength = 0;
		waveInStart(hWaveIn);
		return TRUE;
	case MM_WIM_DATA:
	
		pNewBuffer = reinterpret_cast <PBYTE> (realloc(pSaveBuffer, dwDataLength +
			((PWAVEHDR)lParam)->dwBytesRecorded));
		if (pNewBuffer == NULL)
		{
			waveInClose(hWaveIn);
			return TRUE;
		}
		pSaveBuffer = pNewBuffer;
		CopyMemory(pSaveBuffer + dwDataLength, ((PWAVEHDR)lParam)->lpData, ((PWAVEHDR)lParam)->dwBytesRecorded);
		dwDataLength += ((PWAVEHDR)lParam)->dwBytesRecorded;
		if (bEnding)
		{
			waveInClose(hWaveIn);
			return TRUE;
		}
		waveInAddBuffer(hWaveIn, (PWAVEHDR)lParam, sizeof(WAVEHDR));
		return TRUE;
	case MM_WIM_CLOSE:
		waveInUnprepareHeader(hWaveIn, pWaveHdr1, sizeof(WAVEHDR));
		waveInUnprepareHeader(hWaveIn, pWaveHdr2, sizeof(WAVEHDR));
		free(pBuffer1);
		free(pBuffer2);
		bRecording = FALSE;
		printf("녹음 완료");
		return TRUE;
	case MM_WOM_OPEN:
		printf("이거실행해쓰요!");
		pWaveHdr1->lpData = reinterpret_cast <CHAR*>(pSaveBuffer);
		pWaveHdr1->dwBufferLength = dwDataLength;
		pWaveHdr1->dwBytesRecorded = 0;
		pWaveHdr1->dwUser = 0;
		pWaveHdr1->dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
		pWaveHdr1->dwLoops = dwRepetitions;
		pWaveHdr1->lpNext = NULL;
		pWaveHdr1->reserved = 0;
		waveOutPrepareHeader(hWaveOut, pWaveHdr1, sizeof(WAVEHDR));
		waveOutWrite(hWaveOut, pWaveHdr1, sizeof(WAVEHDR));
		bEnding = FALSE;
		bPlaying = TRUE;
		return TRUE;
	case MM_WOM_DONE:
		waveOutUnprepareHeader(hWaveOut, pWaveHdr1, sizeof(WAVEHDR));
		waveOutClose(hWaveOut);
		return TRUE;
	case MM_WOM_CLOSE:
		dwRepetitions = 1;
		bPlaying = FALSE;
		return TRUE;



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

	
	default:break;
	}
	return (DefWindowProc(hWnd, nMessageID, wParam, lParam));
}
/*
	case IDC_PLAY:  //패킷 받고나서 
	{
		bPlaying = TRUE;
		waveform.wFormatTag = WAVE_FORMAT_PCM;
		waveform.nChannels = 1;
		waveform.nSamplesPerSec = 11025;
		waveform.nAvgBytesPerSec = 11025;
		waveform.nBlockAlign = 1;
		waveform.wBitsPerSample = 8;
		waveform.cbSize = 0;
		waveInReset(hWaveIn);
		waveOutReset(hWaveOut);
		Wav("temp.wav", hWnd);
		RECT rc;
		GetClientRect(hWnd, &rc);
		PLAY = TRUE;
		InvalidateRect(hWnd, &rc, TRUE);

	}
	break;

	*/




void CGameFramework::BuildObjects()
{
	CreateShaderVariables();

	//CCubeMeshDiffused *pCubeMesh = new CCubeMeshDiffused(m_pd3dDevice, 4.0f, 12.0f, 4.0f, D3DXCOLOR(0.5f, 0.0f, 0.0f, 0.0f));
	CSkinMeshDiffused *pSkinMesh = new CSkinMeshDiffused(m_pd3dDevice, 10.0f, 12.0f, 10.0f, D3DXCOLOR(0.5f, 0.0f, 0.0f, 0.0f));
	m_pPlayer = new CTerrainPlayer(1);
	m_pPlayer->AddRef();
	m_pPlayer->SetMesh(pSkinMesh);
	m_pPlayer->ChangeCamera(m_pd3dDevice, THIRD_PERSON_CAMERA, 0.0f);
	CShader *pPlayerShader = new CShader();
	pPlayerShader->CreateShader(m_pd3dDevice, m_pPlayer->GetMeshType());
	m_pPlayer->SetShader(pPlayerShader);

	m_pScene = new CScene();
	m_pScene->SetPlayer(m_pPlayer);
	m_pScene->BuildObjects(m_pd3dDevice);

	CHeightMapTerrain *pTerrain = m_pScene->GetTerrain();
//	float fHeight = pTerrain->GetHeight(pTerrain->GetWidth()*0.5f, pTerrain->GetLength()*0.5f, false) + 100.0f;
	//m_pPlayer->SetPosition(D3DXVECTOR3(pTerrain->GetWidth()*0.5f, fHeight, pTerrain->GetLength()*0.5f));
	m_pPlayer->SetPlayerUpdatedContext(pTerrain);
	m_pPlayer->SetCameraUpdatedContext(pTerrain);

	m_pCamera = m_pPlayer->GetCamera();
	m_pCamera->SetViewport(m_pd3dDeviceContext, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	m_pCamera->GenerateViewMatrix();

	m_pScene->SetCamera(m_pCamera);
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

void SendPacket(unsigned char *packet)
{
	Overlap_ex *over = new Overlap_ex;
	memset(over, 0, sizeof(Overlap_ex));
	over->operation =1;
	over->wsabuf.buf = reinterpret_cast<CHAR *>(over->iocp_buffer);
	over->wsabuf.len = packet[0];
	memcpy(over->iocp_buffer, packet, packet[0]);

	int ret = WSASend(g_mysocket, &over->wsabuf, 1, NULL, 0,
		&over->original_overlap, NULL);
	if (0 != ret) {
	}
}


void CGameFramework::ProcessInput()
{
	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	if (GetKeyboardState(pKeysBuffer) && m_pScene) bProcessedByScene = m_pScene->ProcessInput(pKeysBuffer);
	if (!bProcessedByScene)
	{
		DWORD dwDirection = 0;
		if (pKeysBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeysBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeysBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeysBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
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
				movechek=true;
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
				cs_packet_lookvector *my_packet = reinterpret_cast<cs_packet_lookvector *>(send_buffer);
				my_packet->size = sizeof(my_packet);
				send_wsabuf.len = sizeof(my_packet);
				DWORD iobyte;
				if (cxDelta < 0.0f)
					my_packet->type = CS_LEFTROTATION;
				else if (cxDelta > 0.0f)
					my_packet->type = CS_RIGHTROTATION;
				WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);	
		}
	
	}
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
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

void CGameFramework::AnimateObjects()
{
	float fTimeElapsed = m_GameTimer.GetTimeElapsed();

	if (m_pPlayer) m_pPlayer->Animate(fTimeElapsed, NULL);

	if (m_pScene) m_pScene->AnimateObjects(fTimeElapsed);
}

//#define _WITH_PLAYER_TOP

void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick();

	ProcessInput();

	AnimateObjects();

	if (m_pScene) m_pScene->OnPreRender(m_pd3dDeviceContext);

	float fClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	if (m_pd3dRenderTargetView) m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fClearColor);
	if (m_pd3dDepthStencilView) m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);

	if (m_pPlayer) m_pPlayer->UpdateShaderVariables(m_pd3dDeviceContext);
	m_pCamera->SetViewport(m_pd3dDeviceContext);

	if (m_pScene) m_pScene->Render(m_pd3dDeviceContext, m_pCamera);

#ifdef _WITH_PLAYER_TOP
	m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
#endif
	if (m_pPlayer) m_pPlayer->Render(m_pd3dDeviceContext, m_pCamera);

	m_pDXGISwapChain->Present(0, 0);

	m_GameTimer.GetFrameRate(m_pszBuffer + 12, 37);
	::SetWindowText(m_hWnd, m_pszBuffer);
}
