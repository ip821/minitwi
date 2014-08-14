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

public:

	STDMETHOD(AddRect)(RECT rect);
	STDMETHOD(Clear)();
	STDMETHOD(GetRect)(UINT uiIndex, RECT* rect);
	STDMETHOD(GetCount)(UINT* puiCount);

};

OBJECT_ENTRY_AUTO(__uuidof(ColumnRects), CColumnRects)
