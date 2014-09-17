#pragma once

class CCustomTabControl;

class CScrollControl : public CWindowImpl < CScrollControl >
{
public:

	CScrollControl();

	BEGIN_MSG_MAP(CScrollControl)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_HSCROLL, OnScroll)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
	END_MSG_MAP()

private:
	CBitmap m_bitmap;
	CCustomTabControl* m_pCustomTabControl;

	int m_dx = 0;
	int m_step = 0;
	int m_scrollAmount = 0;
	const int STEPS = 25;
	BOOL m_bFromRightToLeft = FALSE;
	TIMECAPS m_tc;
	UINT m_wTimerRes = 0;
	UINT m_uiTimerId = 0;

public:
	void SetBitmap(HBITMAP hBitmap);
	LRESULT OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void Scroll(BOOL bFromRightToLeft);
	void SetTabWindow(CCustomTabControl* pCustomTabControl);
};
