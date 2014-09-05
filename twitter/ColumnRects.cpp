// ColumnRects.cpp : Implementation of CColumnRects

#include "stdafx.h"
#include "ColumnRects.h"


// CColumnRects

STDMETHODIMP CColumnRects::AddRect(RECT rect, UINT* puiIndex)
{
	m_rects.push_back(rect);
	m_rectStringProps.push_back(std::map<CString, CString>());
	m_rectBoolProps.push_back(std::map<CString, BOOL>());
	*puiIndex = m_rectStringProps.size() - 1;
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
	m_rectBoolProps.clear();
	m_rectStringProps.clear();
	return S_OK;
}

STDMETHODIMP CColumnRects::SetRectStringProp(UINT uiIndex, BSTR bstrKey, BSTR bstrValue)
{
	m_rectStringProps[uiIndex][bstrKey] = bstrValue;
	return S_OK;
}

STDMETHODIMP CColumnRects::GetRectStringProp(UINT uiIndex, BSTR bstrKey, BSTR* bstrValue)
{
	*bstrValue = CComBSTR(m_rectStringProps[uiIndex][bstrKey]).Detach();
	return S_OK;
}

STDMETHODIMP CColumnRects::SetRectBoolProp(UINT uiIndex, BSTR bstrKey, BOOL bValue)
{
	m_rectBoolProps[uiIndex][bstrKey] = bValue;
	return S_OK;
}

STDMETHODIMP CColumnRects::GetRectBoolProp(UINT uiIndex, BSTR bstrKey, BOOL* pbValue)
{
	*pbValue = m_rectBoolProps[uiIndex][bstrKey];
	return S_OK;
}

STDMETHODIMP CColumnRects::IsDisabledSelection(BOOL* pbDisabled)
{
	*pbDisabled = m_bDisabledSelection;
	return S_OK;
}

STDMETHODIMP CColumnRects::DisableSelection(BOOL bDisabled)
{
	m_bDisabledSelection = bDisabled;
	return S_OK;
}
