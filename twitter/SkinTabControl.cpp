// SkinTabControl.cpp : Implementation of CSkinTabControl

#include "stdafx.h"
#include "SkinTabControl.h"


// CSkinTabControl

STDMETHODIMP CSkinTabControl::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	CHECK_E_POINTER(pThemeColorMap);
	m_pThemeColorMap = pThemeColorMap;
	return S_OK;
}

STDMETHODIMP CSkinTabControl::SetFontMap(IThemeFontMap* pThemeFontMap)
{
	CHECK_E_POINTER(pThemeFontMap);
	m_pThemeFontMap = pThemeFontMap;
	return S_OK;
}

STDMETHODIMP CSkinTabControl::MeasureHeader(IObjArray* pObjArray, IColumnRects* pColumnRects, UINT* puiHeight)
{
	*puiHeight = 20;
	return S_OK;
}

STDMETHODIMP CSkinTabControl::EraseBackground(HDC hdc)
{
	return S_OK;
}

STDMETHODIMP CSkinTabControl::DrawHeader(IColumnRects* pColumnRects, HDC hdc, RECT rect)
{
	return S_OK;
}
