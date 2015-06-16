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
	RETURN_IF_FAILED(m_pLayoutBuilder->SetColorMap(pThemeColorMap));
	return S_OK;
}

STDMETHODIMP CLayoutManager::SetFontMap(IThemeFontMap* pThemeFontMap)
{
	CHECK_E_POINTER(pThemeFontMap);
	RETURN_IF_FAILED(m_pLayoutBuilder->SetFontMap(pThemeFontMap));
	return S_OK;
}

STDMETHODIMP CLayoutManager::BuildLayout(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo)
{
	RETURN_IF_FAILED(m_pLayoutBuilder->BuildLayout(hdc, pSourceRect, pDestRect, pLayoutObject, pValueObject, pImageManagerService, pColumnInfo));
	return S_OK;
}

STDMETHODIMP CLayoutManager::EraseBackground(HDC hdc, IColumnsInfo* pColumnInfo)
{
	CHECK_E_POINTER(pColumnInfo);
	return S_OK;
}

STDMETHODIMP CLayoutManager::PaintLayout(HDC hdc, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo)
{
	CHECK_E_POINTER(pColumnInfo);
	return S_OK;
}
