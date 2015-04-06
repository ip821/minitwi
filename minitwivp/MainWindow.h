#pragma once

// {3795CE30-8BC7-4AD5-A2CD-D2D0347A3C9A}
DEFINE_GUID(CLSID_MainWindow,	0x3795ce30, 0x8bc7, 0x4ad5, 0xa2, 0xcd, 0xd2, 0xd0, 0x34, 0x7a, 0x3c, 0x9a);

class CMainWindow :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMainWindow, &CLSID_MainWindow>,
	public CWindowImpl<CMainWindow>,
	public IMFPMediaPlayerCallback
{
public:
	DECLARE_WND_CLASS(L"MainWindow")

	BEGIN_COM_MAP(CMainWindow)
		COM_INTERFACE_ENTRY(IMFPMediaPlayerCallback)
	END_COM_MAP()

	BEGIN_MSG_MAP(CMainWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SET_VIDEO_HWND, OnSetVideoHwnd)
		MESSAGE_HANDLER(WM_COPYDATA, OnCopyData)
		MESSAGE_HANDLER(WM_PLAYER_PLAY, OnPlayerPlay)
		MESSAGE_HANDLER(WM_PLAYER_CLOSE, OnPlayerClose)
		MESSAGE_HANDLER(WM_PLAYER_UPDATE, OnPlayerUpdate)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
	END_MSG_MAP()

private:
	HWND m_hWndVideo = 0;
	CString m_strPath;
	CComPtr<IMFPMediaPlayer> m_pPlayer;
	BOOL m_bPlaying = FALSE;

	void STDMETHODCALLTYPE OnMediaPlayerEvent(MFP_EVENT_HEADER *pEventHeader);

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetVideoHwnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCopyData(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPlayerPlay(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPlayerClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPlayerUpdate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

