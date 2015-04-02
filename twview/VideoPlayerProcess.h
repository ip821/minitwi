#pragma once

class CVideoPlayerProcess
{
private:
	HANDLE m_hProcess = 0;
	HWND m_hWndPlayer = 0;
	HWND m_hWndVideo = 0;

public:
	void Start(HWND hWndVideo);
	void SetProcessData(HINSTANCE hProcess, HWND hWndPlayer);
	void Play(CString& strPath);
	void Shutdown();
	void UpdateVideo();
};