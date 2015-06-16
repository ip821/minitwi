#include "stdafx.h"
#include "LayoutManager.h"

HRESULT CLayoutManager::FinalConstruct()
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_LayoutBuilder, &m_pLayoutBuilder));
	return S_OK;
}

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

STDMETHODIMP CLayoutManager::BuildLayout(HDC hdc, RECT* pRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo)
{
	RETURN_IF_FAILED(m_pLayoutBuilder->BuildLayout(hdc, pRect, pLayoutObject, pValueObject, pColumnInfo));
	return S_OK;
}

STDMETHODIMP CLayoutManager::EraseBackground(HDC hdc, IColumnsInfo* pColumnInfo)
{
	CHECK_E_POINTER(pColumnInfo);
	return S_OK;
}

STDMETHODIMP CLayoutManager::PaintLayout(HDC hdc, IColumnsInfo* pColumnInfo)
{
	CHECK_E_POINTER(pColumnInfo);
	return S_OK;
}
