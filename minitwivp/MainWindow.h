#pragma once

class CMainWindow :	public CWindowImpl<CMainWindow>
{
public:
	DECLARE_WND_CLASS(L"MainWindow")
	
	BEGIN_MSG_MAP(CMainWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SET_VIDEO_HWND, OnSetVideoHwnd)
	END_MSG_MAP()

private:
	HWND m_hWndVideo = 0;

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetVideoHwnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};