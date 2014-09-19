#include "stdafx.h"
#include "ScrollControl.h"
#include "CustomTabControl.h"
#include "Mmsystem.h"
#pragma comment(lib, "Winmm.lib")

#define TARGET_RESOLUTION 1
#define TARGET_INTERVAL 5

CScrollControl::CScrollControl()
{
}

void CScrollControl::SetBitmap(HBITMAP hBitmap)
{
	m_bitmap = hBitmap;
	Invalidate();
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

	StartAnimationTimer(TARGET_INTERVAL);
}

LRESULT CScrollControl::OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CScrollControl::OnAnimationTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_dx += m_scrollAmount;
	m_step++;
	CRect rectUpdate;
	ScrollWindowEx(m_scrollAmount, 0, SW_INVALIDATE, 0, 0, 0, &rectUpdate);
	if (m_step == STEPS)
	{
		m_pCustomTabControl->OnEndScroll();
		return 0;
	}
	StartAnimationTimer(TARGET_INTERVAL);
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

	CRect rect = ps.rcPaint;

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