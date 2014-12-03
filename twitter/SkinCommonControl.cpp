#include "stdafx.h"
#include "SkinCommonControl.h"
#include "Plugins.h"
#include <Uxtheme.h>
#include <Vsstyle.h>
#pragma comment(lib, "UxTheme.lib")

CBrush CSkinCommonControl::m_bkColor;
map<HWND, CSkinCommonControl::WindowDescriptor> CSkinCommonControl::m_procs;

CSkinCommonControl::CSkinCommonControl()
{

}

STDMETHODIMP CSkinCommonControl::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	CHECK_E_POINTER(pThemeColorMap);
	m_pThemeColorMap = pThemeColorMap;
	DWORD dwBkColor = 0;
	m_pThemeColorMap->GetColor(VAR_BRUSH_BACKGROUND, &dwBkColor);
	m_bkColor.CreateSolidBrush(dwBkColor);
	return S_OK;
}

STDMETHODIMP CSkinCommonControl::SetFontMap(IThemeFontMap* pThemeFontMap)
{
	CHECK_E_POINTER(pThemeFontMap);
	m_pThemeFontMap = pThemeFontMap;
	return S_OK;
}

LRESULT CSkinCommonControl::WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (Msg == WM_ERASEBKGND && m_procs.find(hWnd) != m_procs.end() && !m_procs[hWnd].isParent)
	{
		if (m_procs[hWnd].controlType != ControlType::Button)
		{
			CRect rect;
			GetClientRect(hWnd, &rect);
			::FillRect((HDC)wParam, &rect, m_bkColor);
		}
		return 0;
	}

	if (Msg == WM_CTLCOLORSTATIC && m_procs.find(hWnd) != m_procs.end())
	{
		HDC hDC = (HDC)wParam;
		SetBkMode(hDC, TRANSPARENT);
		return (LRESULT)m_bkColor.m_hBrush;
	}

	if (Msg == WM_CTLCOLORBTN && m_procs.find(hWnd) != m_procs.end())
	{
		HDC hDC = (HDC)wParam;
		SetBkMode(hDC, TRANSPARENT);
		return (LRESULT)m_bkColor.m_hBrush;
	}

	if (Msg == WM_DRAWITEM && m_procs.find(hWnd) != m_procs.end())
	{
		auto lpdis = (LPDRAWITEMSTRUCT)lParam;
		if (m_procs.find(lpdis->hwndItem) != m_procs.end() && m_procs[lpdis->hwndItem].controlType == ControlType::Button)
			DrawButton(hWnd, lpdis);
	}

	return CallWindowProc((WNDPROC)m_procs[hWnd].pWndProc, hWnd, Msg, wParam, lParam);
}

STDMETHODIMP CSkinCommonControl::RegisterStaticControl(HWND hWnd)
{
	ATLASSERT(hWnd);
	if (m_procs.find(hWnd) != m_procs.end())
	{
		m_refs[hWnd]++;
		return S_OK;
	}

	WindowDescriptor wd = { 0 };
	wd.pWndProc = SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)&WndProc);
	wd.controlType = ControlType::Static;
	wd.hWnd = hWnd;

	m_procs[hWnd] = wd;
	m_refs[hWnd]++;
	HWND hWndParent = ::GetParent(hWnd);

	if (m_procs.find(hWndParent) != m_procs.end())
	{
		m_refs[hWndParent]++;
		return S_OK;
	}

	WindowDescriptor wdParent = { 0 };
	wdParent.pWndProc = SetWindowLongPtr(hWndParent, GWLP_WNDPROC, (LONG_PTR)&WndProc);
	wdParent.controlType = ControlType::Unknown;
	wdParent.hWnd = hWnd;
	wdParent.isParent = true;

	m_procs[hWndParent] = wdParent;
	m_refs[hWndParent]++;
	return S_OK;
}

STDMETHODIMP CSkinCommonControl::UnregisterStaticControl(HWND hWnd)
{
	ATLASSERT(m_refs.find(hWnd) != m_refs.end());
	m_refs[hWnd]--;
	if (m_refs[hWnd] == 0)
	{
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, m_procs[hWnd].pWndProc);
		m_procs.erase(hWnd);
		m_refs.erase(hWnd);
	}
	HWND hWndParent = ::GetParent(hWnd);
	ATLASSERT(m_refs.find(hWndParent) != m_refs.end());
	m_refs[hWndParent]--;
	if (m_refs[hWndParent] == 0)
	{
		SetWindowLongPtr(hWndParent, GWLP_WNDPROC, m_procs[hWndParent].pWndProc);
		m_procs.erase(hWndParent);
		m_refs.erase(hWndParent);
	}
	return S_OK;
}

STDMETHODIMP CSkinCommonControl::RegisterButtonControl(HWND hWnd)
{
	auto style = GetWindowLongPtr(hWnd, GWL_STYLE);
	style |= BS_OWNERDRAW;
	SetWindowLongPtr(hWnd, GWL_STYLE, style);
	RegisterStaticControl(hWnd);
	m_procs[hWnd].controlType = ControlType::Button;
	return S_OK;
}

STDMETHODIMP CSkinCommonControl::UnregisterButtonControl(HWND hWnd)
{
	auto style = GetWindowLongPtr(hWnd, GWL_STYLE);
	style &= ~BS_OWNERDRAW;
	SetWindowLongPtr(hWnd, GWL_STYLE, style);
	UnregisterStaticControl(hWnd);
	return S_OK;
}

void CSkinCommonControl::DrawButton(HWND hWndParent, LPDRAWITEMSTRUCT lpdis)
{
	ATLASSERT(lpdis->CtlType == ODT_BUTTON);
	CDCHandle cdc(lpdis->hDC);
	cdc.SetBkMode(TRANSPARENT);

	auto colorRect = Color(ARGB(0x00569DE5)).ToCOLORREF();
	auto colorText = GetSysColor(COLOR_BTNTEXT);

	CPen penRect;
	penRect.CreatePen(PS_COSMETIC | PS_SOLID, 1, colorRect);
	cdc.SelectPen(penRect);
	cdc.Rectangle(&lpdis->rcItem);

	CBrush brushRect;
	if ((lpdis->itemState & ODS_SELECTED) || (lpdis->itemState & ODS_CHECKED))
	{
		colorRect = Color(ARGB(0x00D4E8FC)).ToCOLORREF();
		brushRect.CreateSolidBrush(colorRect);
		cdc.SelectBrush(brushRect);
		cdc.Rectangle(&lpdis->rcItem);
	}

	CWindow wndButton(lpdis->hwndItem);
	CString strText;
	wndButton.GetWindowText(strText);

	cdc.SetTextColor(colorText);
	cdc.DrawText(strText, strText.GetLength(), &lpdis->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}