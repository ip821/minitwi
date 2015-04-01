// minitwivp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MainWindow.h"

CAppModule _Module;
using namespace std;

int Run(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOW)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	HWND hWndVideo = 0;

	if (sizeof(INT_PTR) == 4)
	{
		hWndVideo = (HWND)stoi(wstring(lpstrCmdLine));
	}
	else if (sizeof(INT_PTR) == 8)
	{
		hWndVideo = (HWND)wcstoll(lpstrCmdLine, nullptr, 10);
	}

	CMainWindow window;
	HWND hWnd = window.Create(NULL, 0, 0, WS_BORDER);
	auto err = GetLastError();
	window.ShowWindow(SW_HIDE);

	window.SendMessage(WM_SET_VIDEO_HWND, (WPARAM)hWndVideo);

	int nRet = theLoop.Run();
	_Module.RemoveMessageLoop();
	return nRet;
}

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	if (lpstrCmdLine == L"")
		return 0;

	CCoInitializeScope comScope;

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
#pragma warning(suppress: 6387)
	::DefWindowProc(NULL, 0, 0, 0L);

	HRESULT hRes = _Module.Init(NULL, hInstance);
	hRes;
	ATLASSERT(SUCCEEDED(hRes));

	AtlAxWinInit();

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	return nRet;
}

