#pragma once

#include <gdiplus.h>
#include "twtheme_i.h"
#include "Plugins.h"

using namespace ATL;
using namespace std;
using namespace Gdiplus;
using namespace IP;

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
	CComPtr<IImageManagerService> m_pImageManagerService;
	CComPtr<ILayoutManager> m_pLayoutManager;
	CComPtr<IVariantObject> m_pLayout;
	CComPtr<IColumnsInfo> m_pColumnsInfo;

public:

	STDMETHOD(SetTheme)(ITheme* pTheme);
	STDMETHOD(SetImageManagerService)(IImageManagerService* pImageManagerService);
	STDMETHOD(EraseBackground)(HDC hdc);
	STDMETHOD(Draw)(HDC hdc);
	STDMETHOD(StartAnimation)(SkinUserAccountControlAnimationType animationType);
	STDMETHOD(AnimationSetValue)(SkinUserAccountControlAnimationType animationType, DWORD dwValue);
	STDMETHOD(Measure)(HDC hdc, LPRECT lpRect, IVariantObject * pVariantObject, BOOL bIsFollowing, BOOL bFollowButtonDisabled);
	STDMETHOD(GetRects)(RECT* pRectUserImage, RECT* pRectFollowButton);
};

OBJECT_ENTRY_AUTO(__uuidof(SkinUserAccountControl), CSkinUserAccountControl)