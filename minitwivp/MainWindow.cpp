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
	wcscpy_s(&path[0], MAX_PATH, (TCHAR*)pcds->lpData);
	m_strPath = path;
	CPlayer::CreateInstance(m_hWndVideo, m_hWnd, &m_pPlayer);
	m_pPlayer->OpenURL(m_strPath);
	m_pPlayer->Play();
	return 0;
}

LRESULT CMainWindow::OnPlayerPlay(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT CMainWindow::OnPlayerClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	PostQuitMessage(0);
	return 0;
}

STDMETHODIMP CMainWindow::QueryInterface(REFIID riid, void** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(CMainWindow, IMFPMediaPlayerCallback),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppv);
}

STDMETHODIMP_(ULONG) CMainWindow::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CMainWindow::Release()
{
	ULONG count = InterlockedDecrement(&m_cRef);
	if (count == 0)
	{
		delete this;
		return 0;
	}
	return count;
}

void STDMETHODCALLTYPE CMainWindow::OnMediaPlayerEvent(MFP_EVENT_HEADER *pEventHeader)
{
	if (FAILED(pEventHeader->hrEvent))
	{
		return;
	}

	//switch (pEventHeader->eEventType)
	//{
	//case MFP_EVENT_TYPE_MEDIAITEM_CREATED:
	//	m_pPlayer->SetMediaItem(MFP_GET_MEDIAITEM_CREATED_EVENT(pEventHeader)->pMediaItem);
	//	break;
	//
	//case MFP_EVENT_TYPE_MEDIAITEM_SET:
	//	SetTimer(1, 1000);

	//case MFP_EVENT_TYPE_PLAYBACK_ENDED:
	//	m_bPlaying = TRUE;
	//	m_pPlayer->Play();
	//	break;
	//}
}

LRESULT CMainWindow::OnPlayerUpdate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (m_pPlayer && m_bPlaying)
		m_pPlayer->Repaint();
	return 0;
}

LRESULT CMainWindow::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (!::IsWindow(m_hWndVideo))
		PostQuitMessage(0);
	return 0;
}

LRESULT CMainWindow::OnPlayerEvent(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_pPlayer->HandleEvent(wParam);
	return 0;
}
