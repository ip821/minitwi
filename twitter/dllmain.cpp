// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "twitter_i.h"
#include "dllmain.h"
#include "xdlldata.h"

CtwitterModule _AtlModule;
ULONG_PTR g_gdiPlusToken;
Gdiplus::GdiplusStartupInput g_GdiplusStartupInput = { 0 };
// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(hInstance, dwReason, lpReserved))
		return FALSE;
#endif

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		g_GdiplusStartupInput.GdiplusVersion = 2;
		Gdiplus::GdiplusStartup(&g_gdiPlusToken, &g_GdiplusStartupInput, NULL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		Gdiplus::GdiplusShutdown(g_gdiPlusToken);
	}

	hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
