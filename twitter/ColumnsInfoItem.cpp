#include "stdafx.h"
#include "ColumnsInfoItem.h"

STDMETHODIMP CColumnsInfoItem::GetRect(RECT* rect)
{
	*rect = m_rect;
	return S_OK;
}

STDMETHODIMP CColumnsInfoItem::SetRect(RECT rect)
{
	m_rect = rect;
	return S_OK;
}

STDMETHODIMP CColumnsInfoItem::Clear()
{
	m_rect.SetRectEmpty();
	m_rectBoolProps.clear();
	m_rectStringProps.clear();
	return S_OK;
}

STDMETHODIMP CColumnsInfoItem::SetRectStringProp(BSTR bstrKey, BSTR bstrValue)
{
	m_rectStringProps[bstrKey] = bstrValue;
	return S_OK;
}

STDMETHODIMP CColumnsInfoItem::GetRectStringProp(BSTR bstrKey, BSTR* bstrValue)
{
	if (m_rectStringProps.find(bstrKey) != m_rectStringProps.end())
	{
		*bstrValue = CComBSTR(m_rectStringProps[bstrKey]).Detach();
	}
	return S_OK;
}

STDMETHODIMP CColumnsInfoItem::SetRectBoolProp(BSTR bstrKey, BOOL bValue)
{
	m_rectBoolProps[bstrKey] = bValue;
	return S_OK;
}

STDMETHODIMP CColumnsInfoItem::GetRectBoolProp(BSTR bstrKey, BOOL* pbValue)
{
	if (m_rectBoolProps.find(bstrKey) != m_rectBoolProps.end())
	{
		*pbValue = m_rectBoolProps[bstrKey];
	}
	return S_OK;
}

