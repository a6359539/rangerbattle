#ifndef FontMgr_h__
#define FontMgr_h__

#include "Include.h"

class CFont;

class CFontMgr
{
	DECLARE_SINGLETON(CFontMgr)

private:
	explicit CFontMgr(void);
			~CFontMgr(void);

private:
	map<const _tchar*, CFont*>			m_mapFont;
	typedef map<const _tchar*, CFont*>	MAPFONT;

private:
	CFont* Find_Font(const _tchar* pFontTag);

public:
	HRESULT Ready_Font(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, const _tchar* pFontName);
	void	Render_Font(const _tchar* pFontName, const _tchar* pString, FLOAT fSize, _float fPosX, _float fPosY, _uint uiColor);


public:
	void Release(void);

};




#endif
