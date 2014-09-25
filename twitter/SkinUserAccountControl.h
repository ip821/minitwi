#pragma once

#include <gdiplus.h>
#include "twitter_i.h"
#include "Plugins.h"

using namespace ATL;
using namespace std;
using namespace Gdiplus;

class CSkinUserAccountControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSkinUserAccountControl, &CLSID_SkinUserAccountControl>,
	public ISkinUserAccountControl
{
public:
	CSkinUserAccountControl()
	{

	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CSkinUserAccountControl)
		COM_INTERFACE_ENTRY(ISkinUserAccountControl)
	END_COM_MAP()

private:
	CComPtr<IThemeColorMap> m_pThemeColorMap;
	CComPtr<IThemeFontMap> m_pThemeFontMap;
	CComPtr<IImageManagerService> m_pImageManagerService;

	int DrawCounter(HDC hdc, int x, int y, int width, IVariantObject* pVariantObject, BSTR bstrName, BSTR bstrMessage);
public:

	STDMETHOD(SetColorMap)(IThemeColorMap* pThemeColorMap);
	STDMETHOD(SetFontMap)(IThemeFontMap* pThemeFontMap);
	STDMETHOD(SetImageManagerService)(IImageManagerService* pImageManagerService);
	STDMETHOD(EraseBackground)(HDC hdc, LPRECT lpRect, IVariantObject* pObject);
	STDMETHOD(Draw)(HDC hdc, LPRECT lpRect, IVariantObject* pObject);
};

OBJECT_ENTRY_AUTO(__uuidof(SkinUserAccountControl), CSkinUserAccountControl)