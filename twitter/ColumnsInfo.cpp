// ColumnRects.cpp : Implementation of CColumnsInfo

#include "stdafx.h"
#include "ColumnsInfo.h"


// CColumnsInfo

#include "stdafx.h"
#include "ColumnsInfo.h"

STDMETHODIMP CColumnsInfo::AddItem(IColumnsInfoItem** ppColumnsInfoItem)
{
	CHECK_E_POINTER(ppColumnsInfoItem);
	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ColumnsInfoItem, &pColumnsInfoItem));
	m_pItems.push_back(pColumnsInfoItem);
	RETURN_IF_FAILED(pColumnsInfoItem->QueryInterface(ppColumnsInfoItem));
	return S_OK;
}

STDMETHODIMP CColumnsInfo::FindItemIndex(BSTR bstrName, UINT* puiIndex)
{
	CHECK_E_POINTER(puiIndex);
	for (size_t i = 0; i < m_pItems.size(); i++)
	{
		CComBSTR bstrColumnName;
		RETURN_IF_FAILED(m_pItems[i]->GetRectStringProp(Twitter::Metadata::Column::Name, &bstrColumnName));
		if (bstrColumnName == bstrName)
		{
			*puiIndex = i;
			return S_OK;
		}
	}
	return E_NOT_SET;
}

STDMETHODIMP CColumnsInfo::GetItem(UINT uiIndex, IColumnsInfoItem** ppColumnsInfoItem)
{
	CHECK_E_POINTER(ppColumnsInfoItem);
	if (uiIndex >= m_pItems.size())
		return E_INVALIDARG;
	RETURN_IF_FAILED(m_pItems[uiIndex]->QueryInterface(ppColumnsInfoItem));
	return S_OK;
}

STDMETHODIMP CColumnsInfo::GetCount(UINT* puiCount)
{
	CHECK_E_POINTER(puiCount);
	*puiCount = m_pItems.size();
	return S_OK;
}

STDMETHODIMP CColumnsInfo::Clear()
{
	m_pItems.clear();
	return S_OK;
}

STDMETHODIMP CColumnsInfo::IsDisabledSelection(BOOL* pbDisabled)
{
	CHECK_E_POINTER(pbDisabled);
	*pbDisabled = m_bDisabledSelection;
	return S_OK;
}

STDMETHODIMP CColumnsInfo::DisableSelection(BOOL bDisabled)
{
	m_bDisabledSelection = bDisabled;
	return S_OK;
}
