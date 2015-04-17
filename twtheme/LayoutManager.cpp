#include "stdafx.h"
#include "LayoutManager.h"

STDMETHODIMP CLayoutManager::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	CHECK_E_POINTER(pThemeColorMap);
	m_pThemeColorMap = pThemeColorMap;
	return S_OK;
}

STDMETHODIMP CLayoutManager::SetFontMap(IThemeFontMap* pThemeFontMap)
{
	CHECK_E_POINTER(pThemeFontMap);
	m_pThemeFontMap = pThemeFontMap;
	return S_OK;
}

STDMETHODIMP CLayoutManager::BuildLayout(HWND hWnd, RECT* pRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo)
{
	return S_OK;
}

STDMETHODIMP CLayoutManager::PaintLayout(HDC hdc, IColumnsInfo* pColumnInfo)
{
	return S_OK;
}