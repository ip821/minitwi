// ThemeColorMap.h : Declaration of the CThemeColorMap

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"

using namespace ATL;

// CThemeColorMap

class ATL_NO_VTABLE CThemeColorMap :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CThemeColorMap, &CLSID_ThemeColorMap>,
	public IThemeColorMap
{
public:
	CThemeColorMap()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_THEMECOLORMAP)


	BEGIN_COM_MAP(CThemeColorMap)
		COM_INTERFACE_ENTRY(IThemeColorMap)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease(){};

private:
	std::map<CString, DWORD> m_colors;

public:

	STDMETHOD(GetColor)(BSTR bstrColorName, DWORD* dwColor);
	STDMETHOD(SetColor)(BSTR bstrColorName, DWORD dwColor);

};

OBJECT_ENTRY_AUTO(__uuidof(ThemeColorMap), CThemeColorMap)
