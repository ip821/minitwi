#pragma once

#include "twview_i.h"
#include "AnimationTimerSupport.h"

class CCustomTabControl;

class CScrollControl : 
	public CWindowImpl<CScrollControl>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CScrollControl, &CLSID_ScrollControl>,
	public CAnimationTimerSupport<CScrollControl>,
	public IScrollControl
{
public:

	CScrollControl();
	~CScrollControl();

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CScrollControl)
		COM_INTERFACE_ENTRY(IScrollControl)
		COM_INTERFACE_ENTRY(IControl)
	END_COM_MAP()

	BEGIN_MSG_MAP(CScrollControl)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ANIMATION_TIMER, OnAnimationTimer)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
	END_MSG_MAP()

private:
	CBitmap m_bitmap;
	CComPtr<ICustomTabControlInternal> m_pCustomTabControl;

	int m_dx = 0;
	int m_step = 0;
	int m_scrollAmount = 0;
	const int STEPS = 10;
	BOOL m_bFromRightToLeft = FALSE;

	LRESULT OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnAnimationTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	STDMETHOD(Scroll)(BOOL bFromRightToLeft);
	STDMETHOD(SetBitmap)(HBITMAP hBitmap);
	STDMETHOD(SetTabControl)(ICustomTabControlInternal* pCustomTabControl);
	STDMETHOD(ShowWindow)(DWORD dwCommand);

	STDMETHOD(GetHWND)(HWND *hWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult);
};

OBJECT_ENTRY_AUTO(__uuidof(ScrollControl), CScrollControl)