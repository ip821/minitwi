#include "stdafx.h"
#include "VideoPlayerProcess.h"
#include "Plugins.h"

void CVideoPlayerProcess::Start(HWND hWndVideo)
{
	m_hWndVideo = hWndVideo;
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	auto wstrParams = std::to_wstring((long long)hWndVideo);
	CString strCmdLine(L"minitwivp.exe");
	strCmdLine += CString(L" ") + wstrParams.c_str();
	TCHAR lpszCmdLine[100];
	wcscpy(lpszCmdLine, strCmdLine);
	auto res = CreateProcess(
		NULL,
		lpszCmdLine,
		NULL,
		NULL,
		FALSE,
		NULL,
		NULL,
		NULL,
		&si,
		&pi
		); 
	res;
	m_hProcess = pi.hProcess;
}

void CVideoPlayerProcess::SetProcessData(HINSTANCE hProcess, HWND hWndPlayer)
{
	m_hWndPlayer = hWndPlayer;
}

void CVideoPlayerProcess::Play(CString& strPath)
{
	TCHAR path[MAX_PATH] = { 0 };
	wcscpy(path, strPath);
	COPYDATASTRUCT cds = { 0 };
	cds.cbData = MAX_PATH * sizeof(TCHAR);
	cds.dwData = (ULONG_PTR)&path[0];
	cds.lpData = &path[0];
	::SendMessage(m_hWndPlayer, WM_COPYDATA, (WPARAM)m_hWndVideo, (LPARAM)&cds);
	::SendMessage(m_hWndPlayer, WM_PLAYER_PLAY, 0, 0);
}

void CVideoPlayerProcess::Shutdown()
{
	::SendMessage(m_hWndPlayer, WM_PLAYER_CLOSE, 0, 0);
	::WaitForSingleObject(m_hProcess, INFINITE);
}

void CVideoPlayerProcess::UpdateVideo()
{
	::SendMessage(m_hWndPlayer, WM_PLAYER_UPDATE, 0, 0);
}