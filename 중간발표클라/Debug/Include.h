#ifndef Include_h__
#define Include_h__


using namespace std;

#include "Enum.h"
#include "Value.h"
#include "Macro.h"

#include "Typedef.h"
#include "Functor.h"
#include "Function.h"

//전역변수
extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;
extern DWORD		g_dwLightIndex;
extern D3DXVECTOR3	g_vLightDir;

extern Scene_Type	m_eSceneID;

extern BOOL			m_bLogoLoading;



extern _bool		g_bFocus;

extern _bool		g_bSetAquire;

extern _bool		g_bCollisionDraw;


extern _uint		WINCX;
extern _uint		WINCY;

#endif