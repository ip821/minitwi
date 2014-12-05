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

private:
	CRect m_rect;
	std::map<CString, CString> m_rectStringProps;
	std::map<CString, BOOL> m_rectBoolProps;

public:

	STDMETHOD(SetRectStringProp)(BSTR bstrKey, BSTR bstrValue);
	STDMETHOD(GetRectStringProp)(BSTR bstrKey, BSTR* bstrValue);
	STDMETHOD(SetRectBoolProp)(BSTR bstrKey, BOOL bValue);
	STDMETHOD(GetRectBoolProp)(BSTR bstrKey, BOOL* pbValue);
	STDMETHOD(GetRect)(RECT* rect);
	STDMETHOD(SetRect)(RECT rect);
	STDMETHOD(Clear)();
};

OBJECT_ENTRY_AUTO(__uuidof(ColumnsInfoItem), CColumnsInfoItem)
