#include "stdafx.h"
#include "Font.h"

CFont::CFont(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
: m_pGraphicDev(pGraphicDev)
, m_pContext(pContext)
, m_pFW1Factory(NULL)
, m_pFontWrapper(NULL)
{
}

CFont::~CFont(void)
{
}

HRESULT CFont::Ready_Font(const _tchar * pFontName)
{
	FAILED_CHECK(FW1CreateFactory(FW1_VERSION, &m_pFW1Factory));
	FAILED_CHECK(m_pFW1Factory->CreateFontWrapper(m_pGraphicDev, pFontName, &m_pFontWrapper));

	return S_OK;
}

void CFont::Render_Font(const _tchar * pString, _float fSize, _float fPosX, _float fPosY, _uint uiColor)
{
	m_pFontWrapper->DrawString(m_pContext, pString, fSize, fPosX, fPosY, uiColor, FW1_RESTORESTATE);
}

CFont * CFont::Create(ID3D11Device * pGraphicDev, ID3D11DeviceContext * pContext, const _tchar * pFontName)
{
	CFont* pFont = new CFont(pGraphicDev, pContext);

	if (FAILED(pFont->Ready_Font(pFontName)))
	{
		MSG_BOX(L"CFont Create Failed");
		Safe_Release(pFont);
	}

	return pFont;
}

void CFont::Release(void)
{
	m_pFontWrapper->Release();
	m_pFW1Factory->Release();

	delete this;
}
