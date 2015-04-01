#include "stdafx.h"
#include "MainWindow.h"

LRESULT CMainWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT CMainWindow::OnSetVideoHwnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_hWndVideo = (HWND)wParam;
	return 0;
}