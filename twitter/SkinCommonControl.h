// SkinCommonControl.h : Declaration of the CSkinCommonControl

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"

using namespace ATL;
using namespace std;

// CSkinCommonControl

class ATL_NO_VTABLE CSkinCommonControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSkinCommonControl, &CLSID_SkinCommonControl>,
	public ISkinCommonControl
{
public:
	CSkinCommonControl();

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CSkinCommonControl)
		COM_INTERFACE_ENTRY(ISkinCommonControl)
	END_COM_MAP()

	BEGIN_MSG_MAP(CSkinCommonControl)
	END_MSG_MAP()

private:
	CComPtr<IThemeColorMap> m_pThemeColorMap;
	CComPtr<IThemeFontMap> m_pThemeFontMap;

public:

	STDMETHOD(SetColorMap)(IThemeColorMap* pThemeColorMap);
	STDMETHOD(SetFontMap)(IThemeFontMap* pThemeFontMap);
	STDMETHOD(RegisterControl)(HWND hWnd);
	STDMETHOD(UnregisterControl)(HWND hWnd);

};

OBJECT_ENTRY_AUTO(__uuidof(SkinCommonControl), CSkinCommonControl)
