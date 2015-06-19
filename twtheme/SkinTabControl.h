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

private:
	CComPtr<ITheme> m_pTheme;
	CComPtr<IThemeColorMap> m_pThemeColorMap;
	CComPtr<ILayoutManager> m_pLayoutManager;
	CComPtr<IVariantObject> m_pLayoutObject;
	CComPtr<IImageManagerService> m_pImageManagerService;
	CComPtr<IColumnsInfo> m_pColumnsInfo;
	CRect m_rectHeader;
	CRect m_rectInfoImage;
	CToolTipCtrl m_wndTooltip;
	HWND m_hWnd = 0;
	BOOL m_bError = FALSE;
	BOOL m_bAnimation = FALSE;
	CComBSTR m_bstrMessage;

	STDMETHOD(InitImageFromResource)(int nId, LPCTSTR lpType, shared_ptr<Gdiplus::Bitmap>& pBitmap);
	STDMETHOD(GetResourceStream)(int nId, LPCTSTR lpType, IStream** ppStream);

public:

	STDMETHOD(SetTheme)(ITheme* pTheme);
	STDMETHOD(SetColumnsInfo)(IColumnsInfo* pColumnsInfo);
	STDMETHOD(MeasureHeader)(HDC hdc, IObjArray* pObjArray, RECT* clientRect, UINT* puiHeight);
	STDMETHOD(EraseBackground)(HDC hdc);
	STDMETHOD(DrawHeader)(HDC hdc);
	STDMETHOD(SetErrorInfo)(HWND hWnd, BOOL bError, BSTR bstrMessage);
	STDMETHOD(SetSelectedIndex)(UINT uiIndex);
	STDMETHOD(GetInfoRect)(RECT* pRect);
	STDMETHOD(AnimationGetParams)(UINT* puiMilliseconds);
	STDMETHOD(AnimationNextFrame)();
	STDMETHOD(AnimationStart)();
	STDMETHOD(AnimationStop)();
};

OBJECT_ENTRY_AUTO(__uuidof(SkinTabControl), CSkinTabControl)
