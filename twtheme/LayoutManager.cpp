#include "stdafx.h"
#include "LayoutManager.h"

HRESULT CLayoutManager::FinalConstruct()
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_LayoutBuilder, &m_pLayoutBuilder));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_LayoutPainter, &m_pLayoutPainter));
	return S_OK;
}

STDMETHODIMP CLayoutManager::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	CHECK_E_POINTER(pThemeColorMap);
	RETURN_IF_FAILED(m_pLayoutBuilder->SetColorMap(pThemeColorMap));
	RETURN_IF_FAILED(m_pLayoutPainter->SetColorMap(pThemeColorMap));
	return S_OK;
}

STDMETHODIMP CLayoutManager::SetFontMap(IThemeFontMap* pThemeFontMap)
{
	CHECK_E_POINTER(pThemeFontMap);
	RETURN_IF_FAILED(m_pLayoutBuilder->SetFontMap(pThemeFontMap));
	RETURN_IF_FAILED(m_pLayoutPainter->SetFontMap(pThemeFontMap));
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
	RETURN_IF_FAILED(m_pLayoutPainter->EraseBackground(hdc, pColumnInfo));
	return S_OK;
}

STDMETHODIMP CLayoutManager::PaintLayout(HDC hdc, POINT* ptOrigin, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo)
{
	CHECK_E_POINTER(pColumnInfo);
	RETURN_IF_FAILED(m_pLayoutPainter->PaintLayout(hdc, ptOrigin, pImageManagerService, pColumnInfo));
	return S_OK;
}
