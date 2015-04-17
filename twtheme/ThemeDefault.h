// SkinDefault.h : Declaration of the CSkinDefault

#pragma once
#include "resource.h"       // main symbols
#include "twtheme_i.h"

using namespace ATL;
using namespace std;

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

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CThemeDefault)
		COM_INTERFACE_ENTRY(ITheme)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease(){};

private:
	CComPtr<ISkinTabControl> m_pSkinTabControl;
	CComPtr<ISkinCommonControl> m_pSkinCommonControl;
	CComPtr<IThemeColorMap> m_pThemeColorMap;
	CComPtr<IThemeFontMap> m_pThemeFontMap;
	CComPtr<IImageManagerService> m_pImageManagerService;
	CComPtr<IVariantObject> pColorTableObject;
	CComPtr<ILayoutManager> m_pLayoutManager;
	map<CComBSTR, CComPtr<IVariantObject>> m_layoutsMap;
		
public:

	STDMETHOD(LoadThemeFromStream)(IStream* pStream);
	STDMETHOD(GetTimelineSkin)(ISkinTimeline** ppSkinTimeline);
	STDMETHOD(GetTabControlSkin)(ISkinTabControl** pSkinTabControl);
	STDMETHOD(GetCommonControlSkin)(ISkinCommonControl** pSkinCommonControl);
	STDMETHOD(GetColorMap)(IThemeColorMap** ppThemeColorMap);
	STDMETHOD(GetFontMap)(IThemeFontMap** ppThemeFontMap);
	STDMETHOD(GetSkinUserAccountControl)(ISkinUserAccountControl** ppSkinUserAccountControl);
	STDMETHOD(GetLayout)(BSTR bstrLayoutName, IVariantObject** ppVariantObject);
	STDMETHOD(GetLayoutManager)(ILayoutManager** ppLayoutManager);
};

OBJECT_ENTRY_AUTO(__uuidof(ThemeDefault), CThemeDefault)
