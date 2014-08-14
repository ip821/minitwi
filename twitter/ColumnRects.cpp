// ColumnRects.cpp : Implementation of CColumnRects

#include "stdafx.h"
#include "ColumnRects.h"


// CColumnRects

STDMETHODIMP CColumnRects::AddRect(RECT rect, UINT* puiIndex)
{
	m_rects.push_back(rect);
	m_rectProps.push_back(std::map<CString, CString>());
	*puiIndex = m_rectProps.size() - 1;
	return S_OK;
}

STDMETHODIMP CColumnRects::GetRect(UINT uiIndex, RECT* rect)
{
	*rect = m_rects[uiIndex];
	return S_OK;
}

STDMETHODIMP CColumnRects::GetCount(UINT* puiCount)
{
	*puiCount = m_rects.size();
	return S_OK;
}

STDMETHODIMP CColumnRects::Clear()
{
	m_rects.clear();
	m_rectProps.clear();
	return S_OK;
}

STDMETHODIMP CColumnRects::SetRectProp(UINT uiIndex, BSTR bstrKey, BSTR bstrValue)
{
	m_rectProps[uiIndex][bstrKey] = bstrValue;
	return S_OK;
}

STDMETHODIMP CColumnRects::GetRectProp(UINT uiIndex, BSTR bstrKey, BSTR* bstrValue)
{
	*bstrValue = CComBSTR(m_rectProps[uiIndex][bstrKey]).Detach();
	return S_OK;
}
