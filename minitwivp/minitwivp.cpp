// minitwivp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MainWindow.h"

CAppModule _Module;
using namespace std;

int Run(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOW)
{
	if (!lpstrCmdLine)
	{
		MessageBox(NULL, L"Empty cmd line is not allowed", L"minitwivp", MB_OK);
		return 1;
	}
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

	CComObject<CMainWindow>* pMainWindow;
	NOTIFY_IF_FAILED(hWndVideo, CComObject<CMainWindow>::CreateInstance(&pMainWindow));
	HWND hWnd = pMainWindow->Create(NULL, 0, 0, WS_BORDER);
	if (!hWnd)
	{
		NOTIFY_IF_FAILED(hWndVideo, HRESULT_FROM_WIN32(GetLastError()));
	}
	
	if (!pMainWindow->ShowWindow(SW_HIDE))
	{
		NOTIFY_IF_FAILED(hWndVideo, HRESULT_FROM_WIN32(GetLastError()));
	}

	NOTIFY_IF_FAILED(hWndVideo, pMainWindow->SendMessage(WM_SET_VIDEO_HWND, (WPARAM)hWndVideo));

	int nRet = theLoop.Run();

	pMainWindow->DestroyWindow();

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

