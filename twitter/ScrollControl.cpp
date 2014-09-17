#include "stdafx.h"
#include "ScrollControl.h"
#include "CustomTabControl.h"

void CScrollControl::SetBitmap(HBITMAP hBitmap)
{
	m_bitmap = hBitmap;
	Invalidate();
	SIZE sz = { 0 };
	m_bitmap.GetSize(sz);
	SCROLLINFO si = { 0 };
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_POS;
	si.nMin = 0;
	si.nMax = sz.cx;
	si.nPos = 0;
	SetScrollInfo(SB_HORZ, &si, TRUE);
}

void CScrollControl::Scroll(BOOL bFromRightToLeft)
{
	CRect rect;
	GetClientRect(&rect);
	m_scrollAmount = rect.Width() / STEPS;
	m_step = 0;

	if (bFromRightToLeft)
	{
		m_dx = 0;
	}
	else
	{
		m_dx = rect.Width();
		m_scrollAmount = -m_scrollAmount;
	}
	m_bFromRightToLeft = bFromRightToLeft;

	SetTimer(1, 30);
}

LRESULT CScrollControl::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_dx += m_scrollAmount;
	m_step++;
	if (m_step == STEPS)
	{
		KillTimer(1);
		m_pCustomTabControl->OnEndScroll();
		return 0;
	}
	CRect rectUpdate;
	ScrollWindowEx(m_scrollAmount, 0, SW_INVALIDATE, 0, 0, 0, &rectUpdate);
	InvalidateRect(rectUpdate);
	return 0;
}

LRESULT CScrollControl::OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CScrollControl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bitmap.IsNull())
		return 0;
	PAINTSTRUCT ps = { 0 };
	CDCHandle cdc(BeginPaint(&ps));

	CRect rect;
	GetClientRect(&rect);

	CDC cdcBitmap;
	cdcBitmap.CreateCompatibleDC(cdc);
	cdcBitmap.SelectBitmap(m_bitmap);

	BitBlt(cdc, rect.left, rect.top, rect.Width(), rect.Height(), cdcBitmap, m_dx, 0, SRCCOPY);

	EndPaint(&ps);
	return 0;
}

void CScrollControl::SetTabWindow(CCustomTabControl* pCustomTabControl)
{
	m_pCustomTabControl = pCustomTabControl;
}