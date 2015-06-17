#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"

using namespace ATL;

// CColumnRects

class ATL_NO_VTABLE CColumnsInfoItem :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CColumnsInfoItem, &CLSID_ColumnsInfoItem>,
	public IColumnsInfoItem
{
public:
	CColumnsInfoItem()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CColumnsInfoItem)
		COM_INTERFACE_ENTRY(IColumnsInfoItem)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()
	void FinalRelease(){}
	HRESULT FinalConstruct();
private:
	CRect m_rect;
	CComPtr<IColumnsInfo> m_pChildItems;
	CComPtr<IVariantObject> m_pVariantObject;

public:
	STDMETHOD(Clear)();
	STDMETHOD(GetVariantValue)(BSTR key, VARIANT* v);
	STDMETHOD(SetVariantValue)(BSTR key, VARIANT* v);
	STDMETHOD(RemoveVariantValue)(BSTR key);
	STDMETHOD(CopyTo)(IVariantObject* pVariantObject);
	STDMETHOD(GetCount)(UINT_PTR* puiCount);
	STDMETHOD(GetKeyByIndex)(UINT_PTR uiIndex, BSTR* pbstrKey);

	STDMETHOD(SetVariantValueRecursive)(BSTR key, VARIANT* v);
	STDMETHOD(GetChildItems)(IColumnsInfo** ppColumnsInfo);
	STDMETHOD(SetRectStringProp)(BSTR bstrKey, BSTR bstrValue);
	STDMETHOD(GetRectStringProp)(BSTR bstrKey, BSTR* bstrValue);
	STDMETHOD(SetRectBoolProp)(BSTR bstrKey, BOOL bValue);
	STDMETHOD(GetRectBoolProp)(BSTR bstrKey, BOOL* pbValue);
	STDMETHOD(GetRect)(RECT* rect);
	STDMETHOD(SetRect)(RECT rect);
};

OBJECT_ENTRY_AUTO(__uuidof(ColumnsInfoItem), CColumnsInfoItem)
