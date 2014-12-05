// ColumnRects.cpp : Implementation of CColumnsInfo

#include "stdafx.h"
#include "ColumnsInfo.h"


// CColumnsInfo

STDMETHODIMP CColumnsInfo::AddRect(RECT rect, UINT* puiIndex)
{
	m_rects.push_back(rect);
	m_rectStringProps.push_back(std::map<CString, CString>());
	m_rectBoolProps.push_back(std::map<CString, BOOL>());
	*puiIndex = m_rectStringProps.size() - 1;
	return S_OK;
}

STDMETHODIMP CColumnsInfo::GetRect(UINT uiIndex, RECT* rect)
{
	*rect = m_rects[uiIndex];
	return S_OK;
}

STDMETHODIMP CColumnsInfo::SetRect(UINT uiIndex, RECT rect)
{
	m_rects[uiIndex] = rect;
	return S_OK;
}

STDMETHODIMP CColumnsInfo::GetCount(UINT* puiCount)
{
	*puiCount = m_rects.size();
	return S_OK;
}

STDMETHODIMP CColumnsInfo::Clear()
{
	m_rects.clear();
	m_rectBoolProps.clear();
	m_rectStringProps.clear();
	return S_OK;
}

STDMETHODIMP CColumnsInfo::SetRectStringProp(UINT uiIndex, BSTR bstrKey, BSTR bstrValue)
{
	m_rectStringProps[uiIndex][bstrKey] = bstrValue;
	return S_OK;
}

STDMETHODIMP CColumnsInfo::GetRectStringProp(UINT uiIndex, BSTR bstrKey, BSTR* bstrValue)
{
	*bstrValue = CComBSTR(m_rectStringProps[uiIndex][bstrKey]).Detach();
	return S_OK;
}

STDMETHODIMP CColumnsInfo::SetRectBoolProp(UINT uiIndex, BSTR bstrKey, BOOL bValue)
{
	m_rectBoolProps[uiIndex][bstrKey] = bValue;
	return S_OK;
}

STDMETHODIMP CColumnsInfo::GetRectBoolProp(UINT uiIndex, BSTR bstrKey, BOOL* pbValue)
{
	*pbValue = m_rectBoolProps[uiIndex][bstrKey];
	return S_OK;
}

STDMETHODIMP CColumnsInfo::IsDisabledSelection(BOOL* pbDisabled)
{
	*pbDisabled = m_bDisabledSelection;
	return S_OK;
}

STDMETHODIMP CColumnsInfo::DisableSelection(BOOL bDisabled)
{
	m_bDisabledSelection = bDisabled;
	return S_OK;
}
