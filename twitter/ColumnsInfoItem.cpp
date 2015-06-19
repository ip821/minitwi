#include "stdafx.h"
#include "ColumnsInfoItem.h"

HRESULT CColumnsInfoItem::FinalConstruct()
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ColumnsInfo, &m_pChildItems));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &m_pVariantObject));
	return S_OK;
}

STDMETHODIMP CColumnsInfoItem::GetChildItems(IColumnsInfo** ppColumnsInfo)
{
	CHECK_E_POINTER(ppColumnsInfo);
	RETURN_IF_FAILED(m_pChildItems->QueryInterface(ppColumnsInfo));
	return S_OK;
}

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
	RETURN_IF_FAILED(m_pVariantObject->Clear());
	return S_OK;
}

STDMETHODIMP CColumnsInfoItem::SetRectStringProp(BSTR bstrKey, BSTR bstrValue)
{
	RETURN_IF_FAILED(m_pVariantObject->SetVariantValue(bstrKey, &CComVariant(bstrValue)));
	return S_OK;
}

STDMETHODIMP CColumnsInfoItem::GetRectStringProp(BSTR bstrKey, BSTR* bstrValue)
{
	RETURN_IF_FAILED(HrVariantObjectGetBSTR(m_pVariantObject, bstrKey, bstrValue));
	return S_OK;
}

STDMETHODIMP CColumnsInfoItem::SetRectBoolProp(BSTR bstrKey, BOOL bValue)
{
	RETURN_IF_FAILED(m_pVariantObject->SetVariantValue(bstrKey, &CComVariant(bValue)));
	return S_OK;
}

STDMETHODIMP CColumnsInfoItem::GetRectBoolProp(BSTR bstrKey, BOOL* pbValue)
{
	CComVariant vValue;
	RETURN_IF_FAILED(m_pVariantObject->GetVariantValue(bstrKey, &vValue));
	if (vValue.vt == VT_I4)
		*pbValue = vValue.intVal;
	else if (vValue.vt == VT_BOOL)
		*pbValue = vValue.boolVal;
	return S_OK;
}

STDMETHODIMP CColumnsInfoItem::GetVariantValue(BSTR key, VARIANT* v)
{
	RETURN_IF_FAILED(m_pVariantObject->GetVariantValue(key, v));
	return S_OK;
}

STDMETHODIMP CColumnsInfoItem::SetVariantValue(BSTR key, VARIANT* v)
{
	RETURN_IF_FAILED(m_pVariantObject->SetVariantValue(key, v));
	return S_OK;
}

STDMETHODIMP CColumnsInfoItem::RemoveVariantValue(BSTR key)
{
	RETURN_IF_FAILED(m_pVariantObject->RemoveVariantValue(key));
	return S_OK;
}

STDMETHODIMP CColumnsInfoItem::CopyTo(IVariantObject* pVariantObject)
{
	RETURN_IF_FAILED(m_pVariantObject->CopyTo(pVariantObject));
	return S_OK;
}

STDMETHODIMP CColumnsInfoItem::GetCount(UINT_PTR* puiCount)
{
	RETURN_IF_FAILED(m_pVariantObject->GetCount(puiCount));
	return S_OK;
}

STDMETHODIMP CColumnsInfoItem::GetKeyByIndex(UINT_PTR uiIndex, BSTR* pbstrKey)
{
	RETURN_IF_FAILED(m_pVariantObject->GetKeyByIndex(uiIndex, pbstrKey));
	return S_OK;
}

STDMETHODIMP CColumnsInfoItem::SetVariantValueRecursive(BSTR key, VARIANT* v)
{
	RETURN_IF_FAILED(SetVariantValue(key, v));
	UINT uiCount = 0;
	RETURN_IF_FAILED(m_pChildItems->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(m_pChildItems->GetItem(i, &pColumnsInfoItem));
		RETURN_IF_FAILED(pColumnsInfoItem->SetVariantValueRecursive(key, v));
	}
	return S_OK;
}