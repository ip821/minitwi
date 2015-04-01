#include "stdafx.h"
#include "MainWindow.h"

LRESULT CMainWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT CMainWindow::OnSetVideoHwnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_hWndVideo = (HWND)wParam;
	::SendMessage(m_hWndVideo, WM_PLAYER_STARTED, (WPARAM)_Module.GetModuleInstance(), (LPARAM)m_hWnd);
	return 0;
}

LRESULT CMainWindow::OnCopyData(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	TCHAR path[MAX_PATH] = { 0 };
	wcscpy(&path[0], (TCHAR*)pcds->lpData);
	m_strPath = path;
	MessageBox(m_strPath, L"asd", MB_OK);
	return 0;
}