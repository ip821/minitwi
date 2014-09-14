// SkinTabControl.h : Declaration of the CSkinTabControl

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"

using namespace ATL;
using namespace std;

// CSkinTabControl

class ATL_NO_VTABLE CSkinTabControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSkinTabControl, &CLSID_SkinTabControl>,
	public ISkinTabControl
{
public:
	CSkinTabControl();

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CSkinTabControl)
		COM_INTERFACE_ENTRY(ISkinTabControl)
	END_COM_MAP()

private:
	CComPtr<IThemeColorMap> m_pThemeColorMap;
	CComPtr<IThemeFontMap> m_pThemeFontMap;
	shared_ptr<Gdiplus::Bitmap> m_pBitmapHome;
	shared_ptr<Gdiplus::Bitmap> m_pBitmapSettings;
	shared_ptr<Gdiplus::Bitmap> m_pBitmapError;
	shared_ptr<Gdiplus::Bitmap> m_pBitmapInfo;
	CRect m_rectHeader;
	CRect m_rectInfoImage;
	int m_iFrameCount = 0;
	CToolTipCtrl m_wndTooltip;
	HWND m_hWnd;

	STDMETHOD(InitImageFromResource)(int nId, LPCTSTR lpType, shared_ptr<Gdiplus::Bitmap>& pBitmap);
	STDMETHOD(DrawTabs)(IColumnRects* pColumnRects, CDCHandle& cdc, RECT rect, int selectedPageIndex);

public:

	STDMETHOD(SetColorMap)(IThemeColorMap* pThemeColorMap);
	STDMETHOD(SetFontMap)(IThemeFontMap* pThemeFontMap);
	STDMETHOD(GetColorMap)(IThemeColorMap** ppThemeColorMap);

	STDMETHOD(MeasureHeader)(HWND hWnd, IObjArray* pObjArray, IColumnRects* pColumnRects, RECT* clientRect, UINT* puiHeight);
	STDMETHOD(EraseBackground)(HDC hdc);
	STDMETHOD(DrawHeader)(IColumnRects* pColumnRects, HDC hdc, RECT rect, int selectedPageIndex);
	STDMETHOD(DrawAnimation)(HDC hdc);
	STDMETHOD(DrawInfoImage)(HDC hdc, BOOL bError, BSTR bstrMessage);
	STDMETHOD(Notify)(TabControlNotifyReason reason);
	STDMETHOD(GetInfoRect)(RECT* pRect);
};

OBJECT_ENTRY_AUTO(__uuidof(SkinTabControl), CSkinTabControl)
