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
	CSkinTimeline();

	DECLARE_REGISTRY_RESOURCEID(IDR_SKINTIMELINE)

	BEGIN_COM_MAP(CSkinTimeline)
		COM_INTERFACE_ENTRY(ISkinTimeline)
	END_COM_MAP()

private:
	CComPtr<IThemeColorMap> m_pThemeColorMap;

	Gdiplus::Font m_FontNormal;
	Gdiplus::Font m_FontNormalUnderlined;
	Gdiplus::Font m_FontBold;
	Gdiplus::Font m_FontBoldUnderlined;

	std::map<CString, Gdiplus::Font*> m_pFonts;

	Gdiplus::SizeF CSkinTimeline::AddColumn(Gdiplus::Graphics& gfx, IColumnRects* pColumnRects, CString& strColumnName, CString& strDisplayText, CString& strValue, int x, int y, Gdiplus::SizeF& size, BOOL bIsUrl, int flags);
	void GetValue(IVariantObject* pItemObject, CComBSTR& bstrColumnName, CString& strValue);
public:

	STDMETHOD(DrawItem)(HWND hwndControl, IColumnRects* pColumnRects, TDRAWITEMSTRUCT* lpdi, int iHoveredItem, int iHoveredColumn);
	STDMETHOD(MeasureItem)(HWND hwndControl, IVariantObject* pItemObject, TMEASUREITEMSTRUCT* lpMeasureItemStruct, IColumnRects* pColumnRects);
	STDMETHOD(SetColorMap)(IThemeColorMap* pThemeColorMap);
};

OBJECT_ENTRY_AUTO(__uuidof(SkinTimeline), CSkinTimeline)
