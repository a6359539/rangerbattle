// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// LabProject13-3.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

void TRACE(_TCHAR *pString)
{
	_TCHAR pszBuffer[256];
	_stprintf_s(pszBuffer, 256, _T("%s\n"), pString);
	OutputDebugString(pszBuffer);
}

void TRACE(char *pString)
{
	_TCHAR pszBuffer[256];
	_stprintf_s(pszBuffer, 256, _T("%s\n"), pString);
	OutputDebugString(pszBuffer);
}

void TRACE(_TCHAR *pString, UINT uValue)
{
	_TCHAR pszBuffer[256];
	_stprintf_s(pszBuffer, 256, _T("%s%d\n"), pString, uValue);
	OutputDebugString(pszBuffer);
}

void TRACE(_TCHAR *pString, int nValue)
{
	_TCHAR pszBuffer[256];
	_stprintf_s(pszBuffer, 256, _T("%s%d\n"), pString, nValue);
	OutputDebugString(pszBuffer);
}

void TRACE(_TCHAR *pString, int nValue0, int nValue1)
{
	_TCHAR pszBuffer[256];
	_stprintf_s(pszBuffer, 256, _T("%s %d %d\n"), pString, nValue0, nValue1);
	OutputDebugString(pszBuffer);
}

void TRACE(_TCHAR *pString, float fValue)
{
	_TCHAR pszBuffer[256];
	_stprintf_s(pszBuffer, 256, _T("%s%f\n"), pString, fValue);
	OutputDebugString(pszBuffer);
}

