// SkinTimeline.h : Declaration of the CSkinTimeline

#pragma once

#include <gdiplus.h>

#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "Plugins.h"

using namespace ATL;

// CSkinTimeline

class ATL_NO_VTABLE CSkinTimeline :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSkinTimeline, &CLSID_SkinTimeline>,
	public ISkinTimeline
{
public:
	CSkinTimeline() :
		m_FontNormal(Gdiplus::FontFamily::GenericSansSerif(), 10, Gdiplus::FontStyle::FontStyleRegular),
		m_FontBold(Gdiplus::FontFamily::GenericSansSerif(), 10, Gdiplus::FontStyle::FontStyleBold),
		m_FontBoldUnderlined(Gdiplus::FontFamily::GenericSansSerif(), 10, Gdiplus::FontStyle::FontStyleBold | Gdiplus::FontStyle::FontStyleUnderline)
	{
		m_pFonts[VAR_NAME] = &m_FontBold;
		m_pFonts[CString(VAR_NAME) + CString(VAR_SELECTED_POSTFIX)] = &m_FontBoldUnderlined;
		m_pFonts[VAR_TEXT] = &m_FontNormal;
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_SKINTIMELINE)

	BEGIN_COM_MAP(CSkinTimeline)
		COM_INTERFACE_ENTRY(ISkinTimeline)
	END_COM_MAP()

private:
	CComPtr<IThemeColorMap> m_pThemeColorMap;

	Gdiplus::Font m_FontNormal;
	Gdiplus::Font m_FontBold;
	Gdiplus::Font m_FontBoldUnderlined;

	std::map<CString, Gdiplus::Font*> m_pFonts;
public:

	STDMETHOD(DrawItem)(HWND hwndControl, IColumnRects* pColumnRects, TDRAWITEMSTRUCT* lpdi, int iHoveredItem, int iHoveredColumn);
	STDMETHOD(MeasureItem)(HWND hwndControl, IVariantObject* pItemObject, TMEASUREITEMSTRUCT* lpMeasureItemStruct, IColumnRects* pColumnRects);
	STDMETHOD(SetColorMap)(IThemeColorMap* pThemeColorMap);
};

OBJECT_ENTRY_AUTO(__uuidof(SkinTimeline), CSkinTimeline)
