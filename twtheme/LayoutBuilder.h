#pragma once

#include "twtheme_i.h"
#include "ElementType.h"
#include "Metadata.h"

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

	STDMETHOD(GetElements)(IVariantObject* pVariantObject, IObjArray** ppObjArray);
	STDMETHOD(ApplyStartMargins)(IVariantObject* pElement, CRect& rect);
	STDMETHOD(ApplyEndMargins)(IVariantObject* pElement, CRect& rect);
	STDMETHOD(FitToParent)(IVariantObject* pElement, CRect& rectParent, CRect& rect);
	STDMETHOD(BuildHorizontalContainer)(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo);
	STDMETHOD(BuildTextColumn)(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo);
	STDMETHOD(BuildImageColumn)(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo);
	STDMETHOD(BuildMarqueeProgressColumn)(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo);
	STDMETHOD(SetColumnProps)(IVariantObject* pLayoutObject, IColumnsInfoItem* pColumnsInfoItem);
	STDMETHOD(GetElementType)(IVariantObject* pVariantObject, ElementType* pElementType);
	STDMETHOD(ApplyRightAlign)(IColumnsInfo* pChildItems, CRect& rectParent, CRect& rect);
	STDMETHOD(TranslateRects)(POINT* ptOrigin, IColumnsInfo* pColumnsInfo);

public:
	static HRESULT MapType(BSTR bstrType, ElementType* pElementType);

	STDMETHOD(SetColorMap)(IThemeColorMap* pThemeColorMap);
	STDMETHOD(SetFontMap)(IThemeFontMap* pThemeFontMap);

	STDMETHOD(BuildLayout)(HDC hdc, RECT* pSourceRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo);
};

OBJECT_ENTRY_AUTO(__uuidof(LayoutBuilder), CLayoutBuilder)