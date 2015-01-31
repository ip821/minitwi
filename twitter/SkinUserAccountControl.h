#pragma once

#include <gdiplus.h>
#include "twitter_i.h"
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
	CComPtr<IThemeColorMap> m_pThemeColorMap;
	CComPtr<IThemeFontMap> m_pThemeFontMap;
	CComPtr<IImageManagerService> m_pImageManagerService;

	const BYTE STEPS = 25;
	BYTE m_alpha = 0;
	BYTE m_step = 0;
	BYTE m_alphaAmount = 255 / STEPS;

	int DrawCounter(HDC hdc, int x, int y, int width, IVariantObject* pVariantObject, BSTR bstrName, BSTR bstrMessage);
	HRESULT MeasureInternal(HDC hdc, RECT clientRect, IVariantObject* pVariantObject, LPRECT lpRectScreenName, LPRECT lpRectDisplayName, LPRECT lpRectUserImage, LPRECT lpRectFollowButton);

	const CString m_strFollow = L"Follow";
	const CString m_strFollowing = L"Following";

public:

	static void DrawRoundedRect(CDCHandle& cdc, CRect rectText, bool strictRect, COLORREF colorRefBrush = 0);

	STDMETHOD(SetColorMap)(IThemeColorMap* pThemeColorMap);
	STDMETHOD(SetFontMap)(IThemeFontMap* pThemeFontMap);
	STDMETHOD(SetImageManagerService)(IImageManagerService* pImageManagerService);
	STDMETHOD(EraseBackground)(HDC hdc, LPRECT lpRect, IVariantObject* pObject);
	STDMETHOD(Draw)(HDC hdc, LPRECT lpRect, IVariantObject* pObject, IColumnsInfo* pColumnsInfo);
	STDMETHOD(AnimationStart)();
	STDMETHOD(AnimationGetParams)(UINT* puiMilliseconds);
	STDMETHOD(AnimationNextFrame)(BOOL* pbContinueAnimation);
	STDMETHOD(Measure)(HWND hWnd, LPRECT lpRect, IColumnsInfo* pColumnsInfo, IVariantObject* pVariantObject);
};

OBJECT_ENTRY_AUTO(__uuidof(SkinUserAccountControl), CSkinUserAccountControl)