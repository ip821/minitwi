#include "stdafx.h"
#include "SkinCommonControl.h"

CSkinCommonControl::CSkinCommonControl()
{

}

STDMETHODIMP CSkinCommonControl::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	CHECK_E_POINTER(pThemeColorMap);
	m_pThemeColorMap = pThemeColorMap;
	return S_OK;
}

STDMETHODIMP CSkinCommonControl::SetFontMap(IThemeFontMap* pThemeFontMap)
{
	CHECK_E_POINTER(pThemeFontMap);
	m_pThemeFontMap = pThemeFontMap;
	return S_OK;
}

map<HWND, LONG_PTR> m_procs;
LRESULT WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (Msg == WM_ERASEBKGND && m_procs.find(hWnd) != m_procs.end())
		return 0;
	return CallWindowProc((WNDPROC)m_procs[hWnd], hWnd, Msg, wParam, lParam);
}

STDMETHODIMP CSkinCommonControl::RegisterControl(HWND hWnd)
{
	m_procs[hWnd] = SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)&WndProc);
	return S_OK;
}

STDMETHODIMP CSkinCommonControl::UnregisterControl(HWND hWnd)
{
	SetWindowLongPtr(hWnd, GWLP_WNDPROC, m_procs[hWnd]);
	m_procs.erase(hWnd);
	return S_OK;
}

