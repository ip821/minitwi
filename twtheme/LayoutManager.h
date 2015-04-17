#pragma once

#include "twtheme_i.h"

using namespace ATL;

class ATL_NO_VTABLE CLayoutManager :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLayoutManager, &CLSID_LayoutManager>,
	public ILayoutManager
{
public:
	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CLayoutManager)
		COM_INTERFACE_ENTRY(ILayoutManager)
	END_COM_MAP()

private:
public:
	STDMETHOD(BuildLayout)(HWND hWnd, RECT* pRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo);
	STDMETHOD(PaintLayout)(HDC hdc, IColumnsInfo* pColumnInfo);
};

OBJECT_ENTRY_AUTO(__uuidof(LayoutManager), CLayoutManager)