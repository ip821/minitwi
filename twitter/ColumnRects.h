// ColumnRects.h : Declaration of the CColumnRects

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"

using namespace ATL;

// CColumnRects

class ATL_NO_VTABLE CColumnRects :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CColumnRects, &CLSID_ColumnRects>,
	public IColumnRects
{
public:
	CColumnRects()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_COLUMNRECTS)
	
	BEGIN_COM_MAP(CColumnRects)
		COM_INTERFACE_ENTRY(IColumnRects)
	END_COM_MAP()

private:
	std::vector<CRect> m_rects;
	std::vector< std::map<CString, CString> > m_rectStringProps;
	std::vector< std::map<CString, BOOL> > m_rectBoolProps;
	BOOL m_bDisabledSelection = FALSE;

public:

	STDMETHOD(AddRect)(RECT rect, UINT* puiIndex);
	STDMETHOD(SetRectStringProp)(UINT uiIndex, BSTR bstrKey, BSTR bstrValue);
	STDMETHOD(GetRectStringProp)(UINT uiIndex, BSTR bstrKey, BSTR* bstrValue);
	STDMETHOD(SetRectBoolProp)(UINT uiIndex, BSTR bstrKey, BOOL bValue);
	STDMETHOD(GetRectBoolProp)(UINT uiIndex, BSTR bstrKey, BOOL* pbValue);
	STDMETHOD(Clear)();
	STDMETHOD(GetRect)(UINT uiIndex, RECT* rect);
	STDMETHOD(SetRect)(UINT uiIndex, RECT rect);
	STDMETHOD(GetCount)(UINT* puiCount);
	STDMETHOD(IsDisabledSelection)(BOOL* pbDisabled);
	STDMETHOD(DisableSelection)(BOOL bDisabled);
};

OBJECT_ENTRY_AUTO(__uuidof(ColumnRects), CColumnRects)
