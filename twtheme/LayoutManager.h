#pragma once

#include "twtheme_i.h"
#include "ElementType.h"

using namespace ATL;

class ATL_NO_VTABLE CLayoutManager :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLayoutManager, &CLSID_LayoutManager>,
	public ILayoutManager,
	public ILayoutManagerInternal
{
public:
	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CLayoutManager)
		COM_INTERFACE_ENTRY(ILayoutManager)
		COM_INTERFACE_ENTRY(ILayoutManagerInternal)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT();

	void FinalRelease(){}
	HRESULT FinalConstruct();
private:
	CComPtr<ILayoutBuilder> m_pLayoutBuilder;
	CComPtr<ILayoutPainter> m_pLayoutPainter;

public:
	STDMETHOD(BuildLayout)(HDC hdc, RECT* pSourceRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo);
	STDMETHOD(EraseBackground)(HDC hdc, IColumnsInfo* pColumnInfo);
	STDMETHOD(PaintLayout)(HDC hdc, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, BSTR bstrItemName);
	
	STDMETHOD(SetColorMap)(IThemeColorMap* pThemeColorMap);
	STDMETHOD(SetFontMap)(IThemeFontMap* pThemeFontMap);
};

OBJECT_ENTRY_AUTO(__uuidof(LayoutManager), CLayoutManager)