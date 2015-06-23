#pragma once

#include <atlcomcli.h>
#include "..\model-libs\objmdl\Macros.h"
#include "objmdl_contract_i.h"
#include "Metadata.h"

static inline HRESULT HrLayoutFindItemByName(IVariantObject* pElement, BSTR bstrName, IVariantObject** ppItem)
{
	CHECK_E_POINTER(pElement);
	CHECK_E_POINTER(bstrName);
	CHECK_E_POINTER(ppItem);

	*ppItem = nullptr;

	CComVar vName;
	pElement->GetVariantValue(IP::Twitter::Themes::Metadata::Element::Name, &vName);
	if (vName.vt == VT_BSTR && CComBSTR(vName.bstrVal) == bstrName)
	{
		RETURN_IF_FAILED(pElement->QueryInterface(ppItem));
		return S_OK;
	}

	CComVar vElements;
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
			RETURN_IF_FAILED(HrLayoutFindItemByName(pChild, bstrName, ppItem));
			if (*ppItem)
				return S_OK;
		}
	}
	return S_OK;
}

static inline HRESULT HrLayoutSetVariantValueRecursive(IVariantObject* pElement, BSTR bstrName, CComVar* pV)
{
	CHECK_E_POINTER(pElement);
	CHECK_E_POINTER(bstrName);
	CHECK_E_POINTER(pV);

	RETURN_IF_FAILED(pElement->SetVariantValue(bstrName, pV));

	CComVar vElements;
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
			RETURN_IF_FAILED(HrLayoutSetVariantValueRecursive(pChild, bstrName, pV));
		}
	}
	return S_OK;
}