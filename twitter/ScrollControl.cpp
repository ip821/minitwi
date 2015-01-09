#include "stdafx.h"
#include "ScrollControl.h"
#include "CustomTabControl.h"
#include "Mmsystem.h"
#include "GdilPlusUtils.h"
#pragma comment(lib, "Winmm.lib")

#define TARGET_RESOLUTION 1
#define TARGET_INTERVAL 12

CScrollControl::CScrollControl()
{
}

CScrollControl::~CScrollControl()
{
	DestroyWindow();
}

void CScrollControl::SetBitmap(HBITMAP hBitmap)
{
	m_bitmap = hBitmap;
	Invalidate();
	RedrawWindow(0, 0, RDW_UPDATENOW);
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
	Invalidate();
	RedrawWindow(0, 0, RDW_UPDATENOW);
#ifdef _DEBUG
	using namespace boost;
	CString str;
	str.Format(
		L"CScrollControl::OnAnimationTimer - rectUpdate = (%s, %s - %s, %s)\n",
		lexical_cast<wstring>(rectUpdate.left).c_str(),
		lexical_cast<wstring>(rectUpdate.top).c_str(),
		lexical_cast<wstring>(rectUpdate.right).c_str(),
		lexical_cast<wstring>(rectUpdate.bottom).c_str()
		);
	OutputDebugString(str);
#endif

	RedrawWindow(rectUpdate, 0, RDW_UPDATENOW | RDW_NOERASE);

	if (m_step == STEPS)
	{
		m_pCustomTabControl->OnEndScroll();
		return 0;
	}
	StartAnimationTimer(TARGET_INTERVAL);
	return 0;
}

LRESULT CScrollControl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bitmap.IsNull())
		return 0;

	PAINTSTRUCT ps = { 0 };
	CDCHandle cdc(BeginPaint(&ps));

	CRect rect = ps.rcPaint;
#ifdef _DEBUG
	using namespace boost;
	CString str;
	str.Format(
		L"CScrollControl::OnPaint - ps.rcPaint = (%s, %s - %s, %s)\n",
		lexical_cast<wstring>(rect.left).c_str(),
		lexical_cast<wstring>(rect.top).c_str(),
		lexical_cast<wstring>(rect.right).c_str(),
		lexical_cast<wstring>(rect.bottom).c_str()
		);
	OutputDebugString(str);
#endif

	CDC cdcBitmap;
	cdcBitmap.CreateCompatibleDC(cdc);
	CDCSelectBitmapScope cdcSelectBitmapScope(cdcBitmap, m_bitmap);

	cdc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), cdcBitmap, m_dx + rect.left, 0, SRCCOPY);

	EndPaint(&ps);
	return 0;
}

void CScrollControl::SetTabWindow(CCustomTabControl* pCustomTabControl)
{
	m_pCustomTabControl = pCustomTabControl;
}