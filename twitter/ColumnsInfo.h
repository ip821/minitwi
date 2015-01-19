// ColumnRects.h : Declaration of the CColumnRects

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"

using namespace ATL;
using namespace std;

// CColumnRects

class ATL_NO_VTABLE CColumnsInfo :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CColumnsInfo, &CLSID_ColumnsInfo>,
	public IColumnsInfo
{
public:
	CColumnsInfo()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_COLUMNRECTS)
	
	BEGIN_COM_MAP(CColumnsInfo)
		COM_INTERFACE_ENTRY(IColumnsInfo)
	END_COM_MAP()

private:
	vector<CAdapt<CComPtr<IColumnsInfoItem>>> m_pItems;
	BOOL m_bDisabledSelection = FALSE;

public:
	STDMETHOD(AddItem)(IColumnsInfoItem** ppColumnsInfoItem);
	STDMETHOD(GetItem)(UINT uiIndex, IColumnsInfoItem** ppColumnsInfoItem);
	STDMETHOD(GetCount)(UINT* puiCount);
	STDMETHOD(Clear)();
	STDMETHOD(IsDisabledSelection)(BOOL* pbDisabled);
	STDMETHOD(DisableSelection)(BOOL bDisabled);
};

OBJECT_ENTRY_AUTO(__uuidof(ColumnsInfo), CColumnsInfo)
