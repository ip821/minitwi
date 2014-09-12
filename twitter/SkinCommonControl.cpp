#include "stdafx.h"
#include "SkinCommonControl.h"
#include "Plugins.h"

static CBrush m_bkColor;
map<HWND, LONG_PTR> m_procs;

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

LRESULT WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (Msg == WM_ERASEBKGND && m_procs.find(hWnd) != m_procs.end())
	{
		CRect rect;
		GetClientRect(hWnd, &rect);
		::FillRect((HDC)wParam, &rect, m_bkColor);
		return 0;
	}

	if (Msg == WM_CTLCOLORSTATIC && m_procs.find(hWnd) != m_procs.end())
	{
		HDC hDC = (HDC)wParam;
		SetBkMode(hDC, TRANSPARENT);
		return (LRESULT)m_bkColor.m_hBrush;
	}

	return CallWindowProc((WNDPROC)m_procs[hWnd], hWnd, Msg, wParam, lParam);
}

STDMETHODIMP CSkinCommonControl::RegisterControl(HWND hWnd)
{
	ATLASSERT(hWnd);
	if (m_procs.find(hWnd) != m_procs.end())
	{
		m_refs[hWnd]++;
		return S_OK;
	}

	m_procs[hWnd] = SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)&WndProc);
	HWND hWndParent = ::GetParent(hWnd);

	if (m_procs.find(hWndParent) != m_procs.end())
	{
		m_refs[hWndParent]++;
		return S_OK;
	}

	m_procs[hWndParent] = SetWindowLongPtr(hWndParent, GWLP_WNDPROC, (LONG_PTR)&WndProc);
	return S_OK;
}

STDMETHODIMP CSkinCommonControl::UnregisterControl(HWND hWnd)
{
	m_refs[hWnd]--;
	if (m_refs[hWnd] == 0)
	{
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, m_procs[hWnd]);
		m_procs.erase(hWnd);
		m_refs.erase(hWnd);
	}
	HWND hWndParent = ::GetParent(hWnd);
	m_refs[hWndParent]--;
	if (m_refs[hWndParent] == 0)
	{
		SetWindowLongPtr(hWndParent, GWLP_WNDPROC, m_procs[hWndParent]);
		m_procs.erase(hWndParent);
		m_refs.erase(hWndParent);
	}
	return S_OK;
}

