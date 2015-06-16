#pragma once

#include "twtheme_i.h"
#include "ElementType.h"

using namespace ATL;

class ATL_NO_VTABLE CLayoutBuilder :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLayoutBuilder, &CLSID_LayoutBuilder>,
	public ILayoutBuilder
{
public:
	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CLayoutBuilder)
		COM_INTERFACE_ENTRY(ILayoutBuilder)
	END_COM_MAP()

private:
	CComPtr<IThemeFontMap> m_pThemeFontMap;
	CComPtr<IThemeColorMap> m_pThemeColorMap;


	STDMETHOD(GetElementType)(IVariantObject* pVariantObject, ElementType* pElementType);
	STDMETHOD(GetElements)(IVariantObject* pVariantObject, IObjArray** ppObjArray);

	STDMETHOD(BuildHorizontalContainer)(HDC hdc, RECT* pRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo);
public:
	STDMETHOD(BuildLayout)(HDC hdc, RECT* pRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo);
};

OBJECT_ENTRY_AUTO(__uuidof(LayoutBuilder), CLayoutBuilder)