#include "stdafx.h"
#include "LayoutManager.h"

STDMETHODIMP CLayoutManager::BuildLayout(HWND hWnd, RECT* pRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo)
{
	return S_OK;
}

STDMETHODIMP CLayoutManager::PaintLayout(HDC hdc, IColumnsInfo* pColumnInfo)
{
	return S_OK;
}