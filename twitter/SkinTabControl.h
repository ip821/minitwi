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
	CRect m_rectHeader;

	STDMETHOD(InitImageFromResource)(int nId, LPCTSTR lpType, shared_ptr<Gdiplus::Bitmap>& pBitmap);

public:

	STDMETHOD(SetColorMap)(IThemeColorMap* pThemeColorMap);
	STDMETHOD(SetFontMap)(IThemeFontMap* pThemeFontMap);

	STDMETHOD(MeasureHeader)(HWND hWnd, IObjArray* pObjArray, IColumnRects* pColumnRects, RECT* clientRect, UINT* puiHeight);
	STDMETHOD(EraseBackground)(HDC hdc);
	STDMETHOD(DrawHeader)(IColumnRects* pColumnRects, HDC hdc, RECT rect);

};

OBJECT_ENTRY_AUTO(__uuidof(SkinTabControl), CSkinTabControl)
