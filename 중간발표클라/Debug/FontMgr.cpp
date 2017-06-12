#include "stdafx.h"
#include "FontMgr.h"
#include "Font.h"

IMPLEMENT_SINGLETON(CFontMgr)

CFontMgr::CFontMgr(void)
{
}


CFontMgr::~CFontMgr(void)
{
}

CFont * CFontMgr::Find_Font(const _tchar * pFontTag)
{
	MAPFONT::iterator	iter = find_if(m_mapFont.begin(), m_mapFont.end(), CTag_Finder(pFontTag));
	
	if (iter == m_mapFont.end())
		return NULL;

	return iter->second;
}

HRESULT CFontMgr::Ready_Font(ID3D11Device * pGraphicDev, ID3D11DeviceContext * pContext, const _tchar * pFontName)
{
	CFont*			pFont = NULL;

	pFont = Find_Font(pFontName);

	if (pFont != NULL)
		return E_FAIL;

	pFont = CFont::Create(pGraphicDev, pContext, pFontName);
	NULL_CHECK_RETURN(pFont, E_FAIL);

	m_mapFont.insert(MAPFONT::value_type(pFontName, pFont));


	return S_OK;
}

void CFontMgr::Render_Font(const _tchar * pFontName, const _tchar * pString, FLOAT fSize, _float fPosX, _float fPosY, _uint uiColor)
{
	CFont*	pFont = Find_Font(pFontName);

	if (pFont == NULL)
		return;

	pFont->Render_Font(pString, fSize, fPosX, fPosY, uiColor);
}

void CFontMgr::Release(void)
{
	for_each(m_mapFont.begin(), m_mapFont.end(), CRelease_Pair());
	m_mapFont.clear();

	delete this;
}
