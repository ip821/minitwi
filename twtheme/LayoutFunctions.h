#pragma once

#include <atlcomcli.h>
#include "..\model-libs\objmdl\Macros.h"
#include "objmdl_contract_i.h"
#include "Metadata.h"

static inline HRESULT LayoutFindItemByName(IVariantObject* pElement, BSTR bstrName, IVariantObject** ppItem)
{
	CHECK_E_POINTER(ppItem);
	*ppItem = nullptr;

	CComVariant vName;
	pElement->GetVariantValue(IP::Twitter::Themes::Metadata::Element::Name, &vName);
	if (vName.vt == VT_BSTR && CComBSTR(vName.bstrVal) == bstrName)
	{
		RETURN_IF_FAILED(pElement->QueryInterface(ppItem));
		return S_OK;
	}

	CComVariant vElements;
	RETURN_IF_FAILED(pElement->GetVariantValue(IP::Twitter::Themes::Metadata::Element::Elements, &vElements));

	if (vElements.vt == VT_UNKNOWN)
	{
		CComQIPtr<IObjArray> pChildItems = vElements.punkVal;
		UINT uiCount = 0;
		RETURN_IF_FAILED(pChildItems->GetCount(&uiCount));
		for (size_t i = 0; i < uiCount; i++)
		{
			CComPtr<IVariantObject> pChild;
			RETURN_IF_FAILED(pChildItems->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pChild));
			RETURN_IF_FAILED(LayoutFindItemByName(pChild, bstrName, ppItem));
			if (*ppItem)
				return S_OK;
		}
	}
	return S_OK;
}