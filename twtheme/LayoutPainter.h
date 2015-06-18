#pragma once

#include "twtheme_i.h"

using namespace ATL;

class ATL_NO_VTABLE CLayoutPainter :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLayoutPainter, &CLSID_LayoutPainter>,
	public ILayoutPainter
{
public:
	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CLayoutPainter)
		COM_INTERFACE_ENTRY(ILayoutPainter)
	END_COM_MAP()

private:
	CComPtr<IThemeFontMap> m_pThemeFontMap;
	CComPtr<IThemeColorMap> m_pThemeColorMap;

	STDMETHOD(PaintContainer)(HDC hdc, POINT* ptOrigin, IColumnsInfoItem* pColumnInfoItem);
	STDMETHOD(PaintTextColumn)(HDC hdc, POINT* ptOrigin, IColumnsInfoItem* pColumnInfoItem);
	STDMETHOD(PaintImageColumn)(HDC hdc, POINT* ptOrigin, IImageManagerService* pImageManagerService, IColumnsInfoItem* pColumnInfoItem);
	STDMETHOD(PaintMarqueeProgressColumn)(HDC hdc, POINT* ptOrigin, IColumnsInfoItem* pColumnInfoItem);
public:
	STDMETHOD(EraseBackground)(HDC hdc, IColumnsInfo* pColumnInfo);
	STDMETHOD(PaintLayout)(HDC hdc, POINT* ptOrigin, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, BSTR bstrItemName);

	STDMETHOD(SetColorMap)(IThemeColorMap* pThemeColorMap);
	STDMETHOD(SetFontMap)(IThemeFontMap* pThemeFontMap);
};

OBJECT_ENTRY_AUTO(__uuidof(LayoutPainter), CLayoutPainter)