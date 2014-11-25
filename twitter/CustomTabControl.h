#pragma once

#include "twitter_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"
#include "ScrollControl.h"

using namespace ATL;
using namespace std;
using namespace Gdiplus;

class CCustomTabControl;
typedef IConnectionPointImpl <CCustomTabControl, &__uuidof(IInfoControlEventSink)> IConnectionPointImpl_IInfoControlEventSink;
typedef IConnectionPointImpl<CCustomTabControl, &__uuidof(ITabbedControlEventSink)> IConnectionPointImpl_ITabbedControlEventSink;

class CCustomTabControl :
	public CWindowImpl<CCustomTabControl>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCustomTabControl, &CLSID_CustomTabControl>,
	public ICustomTabControl,
	public IControl,
	public IInitializeWithControlImpl,
	public IPersistSettings,
	public IPluginSupportNotifications,
	public IConnectionPointContainerImpl<CCustomTabControl>,
	public IConnectionPointImpl_ITabbedControlEventSink,
	public IConnectionPointImpl_IInfoControlEventSink
{

public:
	DECLARE_WND_CLASS(L"CustomTabControl")

	CCustomTabControl()
	{
		m_handCursor.LoadSysCursor(IDC_HAND);
		m_arrowCursor.LoadSysCursor(IDC_ARROW);
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CCustomTabControl)
		COM_INTERFACE_ENTRY(ICustomTabControl)
		COM_INTERFACE_ENTRY(ITabbedControl)
		COM_INTERFACE_ENTRY(IContainerControl)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IPersistSettings)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
	END_COM_MAP()

	BEGIN_MSG_MAP(CCustomTabControl)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus);
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	END_MSG_MAP()

	BEGIN_CONNECTION_POINT_MAP(CCustomTabControl)
		CONNECTION_POINT_ENTRY(__uuidof(ITabbedControlEventSink))
		CONNECTION_POINT_ENTRY(__uuidof(IInfoControlEventSink))
	END_CONNECTION_POINT_MAP()

private:
	CComPtr<IObjCollection> m_pControls;
	const int INVALID_PAGE_INDEX = -1;
	int m_selectedPageIndex = INVALID_PAGE_INDEX;
	int m_nextSelectedPageIndex = INVALID_PAGE_INDEX;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<ISettings> m_pSettings;
	CComPtr<ISkinTabControl> m_pSkinTabControl;
	CComPtr<IColumnRects> m_pColumnRects;
	UINT m_cAnimationRefs = 0;

	CRect m_rectChildControlArea;
	CRect m_rectInfoImage;
	CCursor m_handCursor;
	CCursor m_arrowCursor;

	BOOL m_bInfoImageEnableClick = FALSE;
	BOOL m_bShowInfoImage = FALSE;
	BOOL m_bInfoImageIsError = FALSE;
	CComBSTR m_bstrInfoMessage;

	CScrollControl m_wndScroll;
	CBitmap m_scrollBitmap;

	void SelectPage(DWORD dwIndex);

	void UpdateChildControlAreaRect();
	void UpdateSizes();
	void AdjustSize();
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	HRESULT Fire_OnLinkClick();
	HRESULT Fire_OnActivate(IControl* pControl);
	HRESULT Fire_OnDeactivate(IControl* pControl);
	HRESULT Fire_OnClose(IControl* pControl);
	HRESULT Fire_OnTabHeaderClick(IControl* pControl);

public:
	void OnEndScroll();

	STDMETHOD(GetHWND)(HWND *hWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult);

	STDMETHOD(UpdateControl)(IControl *pControl);

	STDMETHOD(SetSkinTabControl)(ISkinTabControl* pSkinTabControl);

	STDMETHOD(EnableCommands)(BOOL bEnabled);
	STDMETHOD(AddPage)(IControl *pControl);
	STDMETHOD(GetCurrentPage)(IControl **ppControl);
	STDMETHOD(RemovePage)(IControl *pControl);
	STDMETHOD(ActivatePage)(IControl *pControl);
	STDMETHOD(GetPageCount)(DWORD *pdwCount);
	STDMETHOD(GetPage)(DWORD dwIndex, IControl **ppControl);

	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(Load)(ISettings* pSettings);
	STDMETHOD(Save)(ISettings* pSettings);
	METHOD_EMPTY(STDMETHOD(Reset)(ISettings* /*pSettings*/));

	STDMETHOD(StartAnimation)();
	STDMETHOD(StopAnimation)(UINT* puiRefs);

	STDMETHOD(ShowInfo)(BOOL bError, BOOL bInfoImageEnableClick, BSTR bstrMessage);
	STDMETHOD(HideInfo)();

};

OBJECT_ENTRY_AUTO(__uuidof(CustomTabControl), CCustomTabControl)
