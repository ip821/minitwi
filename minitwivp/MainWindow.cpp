#include "stdafx.h"
#include "MainWindow.h"

LRESULT CMainWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetTimer(1, 1000);
	return 0;
}

LRESULT CMainWindow::OnSetVideoHwnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_hWndVideo = (HWND)wParam;
	NOTIFY_IF_FAILED(m_hWndVideo, MFPCreateMediaPlayer(NULL, FALSE, 0, this, m_hWndVideo, &m_pPlayer));
	::SendMessage(m_hWndVideo, WM_PLAYER_STARTED, (WPARAM)_Module.GetModuleInstance(), (LPARAM)m_hWnd);
	return S_OK;
}

LRESULT CMainWindow::OnCopyData(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	m_strPath = (TCHAR*)pcds->lpData;
	NOTIFY_IF_FAILED(m_hWndVideo, m_pPlayer->CreateMediaItemFromURL(m_strPath, FALSE, 0, NULL));
	return S_OK;
}

LRESULT CMainWindow::OnPlayerPlay(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	NOTIFY_IF_FAILED(m_hWndVideo, m_pPlayer->Play());
	return S_OK;
}

LRESULT CMainWindow::OnPlayerClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	PostQuitMessage(0);
	return 0;
}

void STDMETHODCALLTYPE CMainWindow::OnMediaPlayerEvent(MFP_EVENT_HEADER *pEventHeader)
{
	if (FAILED(pEventHeader->hrEvent))
	{
		NOTIFY_IF_FAILED(m_hWndVideo, pEventHeader->hrEvent);
		return;
	}

	switch (pEventHeader->eEventType)
	{
	case MFP_EVENT_TYPE_MEDIAITEM_CREATED:
		NOTIFY_IF_FAILED(m_hWndVideo, m_pPlayer->SetMediaItem(MFP_GET_MEDIAITEM_CREATED_EVENT(pEventHeader)->pMediaItem));
		break;
	
	case MFP_EVENT_TYPE_MEDIAITEM_SET:
		NOTIFY_IF_FAILED(m_hWndVideo, m_pPlayer->Play());
		break;

	case MFP_EVENT_TYPE_PLAYBACK_ENDED:
		m_bPlaying = FALSE;
		::SendMessage(m_hWndVideo, WM_PLAYER_FINISHED, 0, 0);
		break;
	}
}

LRESULT CMainWindow::OnPlayerUpdate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (m_pPlayer && m_bPlaying)
		NOTIFY_IF_FAILED(m_hWndVideo, m_pPlayer->UpdateVideo());
	return S_OK;
}

LRESULT CMainWindow::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (!::IsWindow(m_hWndVideo))
		PostQuitMessage(0);
	return 0;
}