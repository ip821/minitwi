// SkinDefault.h : Declaration of the CSkinDefault

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"

using namespace ATL;

// CSkinDefault

class ATL_NO_VTABLE CThemeDefault :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CThemeDefault, &CLSID_ThemeDefault>,
	public ITheme
{
public:
	CThemeDefault()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_SKINDEFAULT)

	BEGIN_COM_MAP(CThemeDefault)
		COM_INTERFACE_ENTRY(ITheme)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease(){};

private:
	CComPtr<ISkinTimeline> m_pSkinTimeline;
	CComPtr<IThemeColorMap> m_pThemeColorMap;
public:

	STDMETHOD(GetTimelineSkin)(ISkinTimeline** ppSkinTimeline);
	STDMETHOD(SetColorMap)(IThemeColorMap* pThemeColorMap);
};

OBJECT_ENTRY_AUTO(__uuidof(ThemeDefault), CThemeDefault)
