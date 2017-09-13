// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>
#include "protocol.h"

// C�� ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <D3DX10Math.h>
#include <d3dcompiler.h>

//#include <D3D9Types.h>

#include <Mmsystem.h>

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

//#define _WITH_MSAA4_MULTISAMPLING

#define FRAME_BUFFER_WIDTH				640
#define FRAME_BUFFER_HEIGHT				480

#define CUBEMAP_RENDER_TARGET_WIDTH		1024
#define CUBEMAP_RENDER_TARGET_HEIGHT	1024

#define SHADOW_RENDER_TARGET_WIDTH		1024
#define SHADOW_RENDER_TARGET_HEIGHT		1024

#define VS_CB_SLOT_CAMERA				0x00
#define VS_CB_SLOT_WORLD_MATRIX			0x01
#define VS_CB_SLOT_TEXTURE_MATRIX		0x02
#define VS_CB_SLOT_TERRAIN				0x03
#define VS_CB_SLOT_SKYBOX				0x04
#define VS_CB_SLOT_PROJECTION			0x05
#define VS_CB_SLOT_SHADOW				0x06

#define PS_CB_SLOT_LIGHT				0x00
#define PS_CB_SLOT_MATERIAL				0x01
#define PS_CB_SLOT_TERRAIN				0x03
#define PS_CB_SLOT_SKYBOX				0x04

#define PS_SLOT_TEXTURE					0x00
#define PS_SLOT_TEXTURE_TERRAIN			0x02
#define PS_SLOT_TEXTURE_SKYBOX			0x0D
#define PS_SLOT_TEXTURE_CUBEMAPPED		0x0E
#define PS_SLOT_TEXTURE_PROJECTION		0x0F
#define PS_SLOT_TEXTURE_PROJECTED_DEPTH	0x10
#define PS_SLOT_TEXTURE_SHADOW			0x11

#define PS_SLOT_SAMPLER					0x00
#define PS_SLOT_SAMPLER_DETAIL			0x01
#define PS_SLOT_SAMPLER_TERRAIN			0x02
#define PS_SLOT_SAMPLER_SKYBOX			0x04
#define PS_SLOT_SAMPLER_CUBEMAPPED		0x05
#define PS_SLOT_SAMPLER_PROJECTION		0x06
#define PS_SLOT_SAMPLER_SHADOW			0x07

#define GS_CB_SLOT_CAMERA				0x00

#define RANDOM_COLOR	D3DXCOLOR((rand() * 0xFFFFFF) / RAND_MAX)

#define _WITH_TERRAIN_PARTITION
#define _WITH_FRUSTUM_CULLING_BY_OBJECT
//#define _WITH_FRUSTUM_CULLING_BY_SUBMESH

//#define _WITH_SKYBOX_TEXTURE_ARRAY
#define _WITH_SKYBOX_TEXTURE_CUBE
//#define _WITH_TERRAIN_TEXTURE_ARRAY

extern void TRACE(_TCHAR *pString);
extern void TRACE(char *pString);
extern void TRACE(_TCHAR *pString, UINT uValue);
extern void TRACE(_TCHAR *pString, int nValue);
extern void TRACE(_TCHAR *pString, int nValue0, int nValue1);
extern void TRACE(_TCHAR *pString, float fValue);

