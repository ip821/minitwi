#pragma once

#include "twview_i.h"
#include "AnimationTimerSupport.h"

class CCustomTabControl;

class CScrollControl : 
	public CWindowImpl<CScrollControl>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CScrollControl, &CLSID_ScrollControl>,
	public IScrollControl,
	public IPluginSupportNotifications,
	public IAnimationServiceEventSink
{
public:

	CScrollControl();
	~CScrollControl();

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CScrollControl)
		COM_INTERFACE_ENTRY(IScrollControl)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IAnimationServiceEventSink)
	END_COM_MAP()

	BEGIN_MSG_MAP(CScrollControl)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_ANIMATION_TIMER, OnAnimationTimer)
	END_MSG_MAP()

private:
	CBitmap m_bitmap;
	CComPtr<ICustomTabControlInternal> m_pCustomTabControl;
	CComPtr<IAnimationService> m_pAnimationService;

	int m_dx = 0;
	const DWORD STEPS = 10;
	BOOL m_bFromRightToLeft = FALSE;
	DWORD m_dwAdvice = 0;

	LRESULT OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnAnimationTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	STDMETHOD(Scroll)(BOOL bFromRightToLeft);
	STDMETHOD(SetBitmap)(HBITMAP hBitmap);
	STDMETHOD(SetTabControl)(ICustomTabControlInternal* pCustomTabControl);
	STDMETHOD(ShowWindow)(DWORD dwCommand);

	STDMETHOD(GetHWND)(HWND *hWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnAnimationStep)(IAnimationService *pAnimationService, DWORD dwValue, DWORD dwStep);
};

OBJECT_ENTRY_AUTO(__uuidof(ScrollControl), CScrollControl)