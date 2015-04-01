#pragma once

class CMainWindow :
	public CWindowImpl<CMainWindow>,
	public IMFPMediaPlayerCallback
{
public:
	DECLARE_WND_CLASS(L"MainWindow")

	BEGIN_MSG_MAP(CMainWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SET_VIDEO_HWND, OnSetVideoHwnd)
		MESSAGE_HANDLER(WM_COPYDATA, OnCopyData)
		MESSAGE_HANDLER(WM_PLAYER_PLAY, OnPlayerPlay)
		MESSAGE_HANDLER(WM_PLAYER_CLOSE, OnPlayerClose)
	END_MSG_MAP()

private:
	HWND m_hWndVideo = 0;
	CString m_strPath;
	CComPtr<IMFPMediaPlayer> m_pPlayer;

	long m_cRef = 1;

	STDMETHOD(QueryInterface)(REFIID riid, void** ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	void STDMETHODCALLTYPE OnMediaPlayerEvent(MFP_EVENT_HEADER *pEventHeader);
	void OnMediaItemCreated(MFP_MEDIAITEM_CREATED_EVENT *pEvent);
	void OnMediaItemSet(MFP_MEDIAITEM_SET_EVENT * /*pEvent*/);

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetVideoHwnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCopyData(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPlayerPlay(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPlayerClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

