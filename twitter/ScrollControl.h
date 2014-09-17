#pragma once

class CCustomTabControl;

class CScrollControl : public CWindowImpl < CScrollControl >
{
public:

	CScrollControl()
	{

	}

	BEGIN_MSG_MAP(CScrollControl)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_HSCROLL, OnScroll)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
	END_MSG_MAP()

private:
	CBitmap m_bitmap;
	CCustomTabControl* m_pCustomTabControl;

	int m_dx = 0;
	int m_step = 0;
	int m_scrollAmount = 0;
	const int STEPS = 5;
	BOOL m_bFromRightToLeft = FALSE;

public:
	void SetBitmap(HBITMAP hBitmap);
	LRESULT OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void Scroll(BOOL bFromRightToLeft);
	void SetTabWindow(CCustomTabControl* pCustomTabControl);
};
