#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"

using namespace ATL;

class ATL_NO_VTABLE COpenUrlService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<COpenUrlService, &CLSID_FormsService>,
	public IOpenUrlService
{
public:

	COpenUrlService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(COpenUrlService)
		COM_INTERFACE_ENTRY(IOpenUrlService)
	END_COM_MAP()

public:
	STDMETHOD(OpenColumnAsUrl)(BSTR bstrColumnName, DWORD dwColumnIndex, IColumnRects* pColumnRects, IVariantObject* pVariantObject);
};

OBJECT_ENTRY_AUTO(__uuidof(OpenUrlService), COpenUrlService)