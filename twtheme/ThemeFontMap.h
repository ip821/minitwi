#pragma once
#include "resource.h"       // main symbols
#include "twtheme_i.h"

using namespace ATL;

class ATL_NO_VTABLE CThemeFontMap :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CThemeFontMap, &CLSID_ThemeColorMap>,
	public IThemeFontMap
{
public:
	CThemeFontMap()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CThemeFontMap)
		COM_INTERFACE_ENTRY(IThemeFontMap)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct(){ return S_OK; };
	void FinalRelease(){};

private:
	std::map<CString, CFont> m_fonts;

public:

	STDMETHOD(Initialize)(IObjCollection* pObjectCollection);
	STDMETHOD(GetFont)(BSTR bstrFontName, HFONT* phFont);
	STDMETHOD(SetFont)(BSTR bstrFontName, BSTR bstrFontFamily, DWORD dwSize, BOOL bBold, BOOL bUnderline);

};

OBJECT_ENTRY_AUTO(__uuidof(ThemeFontMap), CThemeFontMap)
