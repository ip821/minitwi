// SkinTabControl.h : Declaration of the CSkinTabControl

#pragma once
#include "resource.h"       // main symbols
#include "twtheme_i.h"

using namespace ATL;
using namespace std;

// CSkinTabControl

class ATL_NO_VTABLE CSkinTabControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSkinTabControl, &CLSID_SkinTabControl>,
	public ISkinTabControl,
	public IThemeSupport
{
public:
	CSkinTabControl();

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CSkinTabControl)
		COM_INTERFACE_ENTRY(ISkinTabControl)
		COM_INTERFACE_ENTRY(IThemeSupport)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()
	void FinalRelease();
	HRESULT FinalConstruct();
private:
	CComPtr<ITheme> m_pTheme;
	CComPtr<IThemeColorMap> m_pThemeColorMap;
	CComPtr<ILayoutManager> m_pLayoutManager;
	CComPtr<IVariantObject> m_pLayoutObject;
	shared_ptr<Gdiplus::Bitmap> m_pBitmapHome;
	shared_ptr<Gdiplus::Bitmap> m_pBitmapSearch;
	shared_ptr<Gdiplus::Bitmap> m_pBitmapLists;
	shared_ptr<Gdiplus::Bitmap> m_pBitmapSettings;
	shared_ptr<Gdiplus::Bitmap> m_pBitmapError;
	shared_ptr<Gdiplus::Bitmap> m_pBitmapInfo;
	CRect m_rectHeader;
	CRect m_rectInfoImage;
	int m_iFrameCount = 0;
	CToolTipCtrl m_wndTooltip;
	HWND m_hWnd;

	STDMETHOD(InitImageFromResource)(int nId, LPCTSTR lpType, shared_ptr<Gdiplus::Bitmap>& pBitmap);

public:

	STDMETHOD(SetTheme)(ITheme* pTheme);
	STDMETHOD(MeasureHeader)(HWND hWnd, IObjArray* pObjArray, IColumnsInfo* pColumnsInfo, RECT* clientRect, UINT* puiHeight);
	STDMETHOD(EraseBackground)(IColumnsInfo* pColumnsInfo, HDC hdc);
	STDMETHOD(DrawHeader)(IColumnsInfo* pColumnsInfo, HDC hdc, RECT rect);
	STDMETHOD(DrawAnimation)(HDC hdc);
	STDMETHOD(DrawInfoImage)(HDC hdc, BOOL bError, BSTR bstrMessage);
	STDMETHOD(GetInfoRect)(RECT* pRect);
	STDMETHOD(StartInfoImage)();
	STDMETHOD(StopInfoImage)();
	STDMETHOD(AnimationGetParams)(UINT* puiMilliseconds);
	STDMETHOD(AnimationNextFrame)();

};

OBJECT_ENTRY_AUTO(__uuidof(SkinTabControl), CSkinTabControl)
