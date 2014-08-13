// SkinTimeline.h : Declaration of the CSkinTimeline

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"

using namespace ATL;

// CSkinTimeline

class ATL_NO_VTABLE CSkinTimeline :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSkinTimeline, &CLSID_SkinTimeline>,
	public ISkinTimeline
{
public:
	CSkinTimeline()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_SKINTIMELINE)

	BEGIN_COM_MAP(CSkinTimeline)
		COM_INTERFACE_ENTRY(ISkinTimeline)
	END_COM_MAP()

public:

	STDMETHOD(DrawItem)(HWND hwndControl, IVariantObject* pItemObject, TDRAWITEMSTRUCT* lpdi);
	STDMETHOD(MeasureItem)(HWND hwndControl, IVariantObject* pItemObject, TMEASUREITEMSTRUCT* lpMeasureItemStruct);
	METHOD_EMPTY(STDMETHOD(SetColorMap)(IThemeColorMap* pThemeColorMap));
};

OBJECT_ENTRY_AUTO(__uuidof(SkinTimeline), CSkinTimeline)
