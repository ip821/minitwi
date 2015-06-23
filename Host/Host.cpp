// WinDbgUITest.cpp : main source file for WinDbgUITest.exe
//

#include "stdafx.h"
#include "resource.h"
#include "..\twitter\Plugins.h"
#include "..\model-libs\objmdl\Functions.h"
#include "..\model-libs\objmdl\Metadata.h"

using namespace std;
using namespace IP;

CAppModule _Module;
ULONG_PTR g_gdiPlusToken;
Gdiplus::GdiplusStartupInput g_GdiplusStartupInput = { 0 };

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	int nRet = 0;
	{
		g_GdiplusStartupInput.GdiplusVersion = 1;
		Gdiplus::GdiplusStartup(&g_gdiPlusToken, &g_GdiplusStartupInput, NULL);

		TCHAR lpszCurrentDir[MAX_PATH];
		if (GetModuleFileName(NULL, lpszCurrentDir, MAX_PATH))
		{
			PathRemoveFileSpec(lpszCurrentDir);
			SetCurrentDirectory(lpszCurrentDir);
		}

		CMessageLoop theLoop;
		_Module.AddMessageLoop(&theLoop);

		CComPtr<IMessageLoop> pMessageLoop;
		RETURN_IF_FAILED(CMessageLoopWrapper::_CreatorClass::CreateInstance(NULL, IID_IMessageLoop, (LPVOID*)&pMessageLoop));
		static_cast<CMessageLoopWrapper*>(pMessageLoop.p)->SetMessageLoop(&theLoop);

		CManualComObjectLoader loader(L"ObjMdl.dll");

		CComPtr<IPluginManager> pPluginManager;
		RETURN_IF_FAILED(loader.CoCreateInstance(CLSID_PluginManager, NULL, IID_IPluginManager, (LPVOID*)&pPluginManager));
		RETURN_IF_FAILED(pPluginManager->InitializePluginLibraryByName(L"ObjMdl.dll"));
		RETURN_IF_FAILED(pPluginManager->InitializePluginLibraryByName(L"ViewMdl.dll"));
		RETURN_IF_FAILED(pPluginManager->LoadPlugins(NULL));

		CComPtr<IVariantObject> pAboutPluginInfo;
		RETURN_IF_FAILED(pPluginManager->CoCreateInstance(CLSID_VariantObject, IID_IVariantObject, (LPVOID*)&pAboutPluginInfo));
		RETURN_IF_FAILED(pAboutPluginInfo->SetVariantValue(ObjectModel::Metadata::Plugins::Object::Namespace, &CComVar(PNAMESP_HOSTFORM)));
		RETURN_IF_FAILED(pAboutPluginInfo->SetVariantValue(ObjectModel::Metadata::Plugins::Object::Type, &CComVar(PVIEWTYPE_COMMAND)));
		RETURN_IF_FAILED(pAboutPluginInfo->SetVariantValue(ObjectModel::Metadata::Plugins::Object::Id, &CComVar(COMMAND_ABOUT)));
		RETURN_IF_FAILED(pAboutPluginInfo->SetVariantValue(ObjectModel::Metadata::Plugins::Object::Clsid, &CComVar(CLSID_AboutCommand)));
		RETURN_IF_FAILED(pPluginManager->RegisterPluginInfo2(pAboutPluginInfo));

		CComPtr<IMainWindow> pWindow;
		RETURN_IF_FAILED(pPluginManager->CoCreateInstance(CLSID_MainFrame, IID_IMainWindow, (LPVOID*)&pWindow));

		CIcon icon;
		icon.LoadIcon(IDR_MAINFRAME);
		RETURN_IF_FAILED(pWindow->SetMessageLoop(pMessageLoop));

		CComPtr<ISettings> pRootSettings;
		RETURN_IF_FAILED(pPluginManager->CoCreateInstance(CLSID_RegistrySettings, IID_ISettings, (LPVOID*)&pRootSettings));
		CComPtr<ISettings> pMainWindowSettings;
		RETURN_IF_FAILED(pRootSettings->OpenSubSettings(L"Software\\IP\\MiniTwi", &pMainWindowSettings));
		CComQIPtr<IPersistSettings> pPersistSettings = pWindow;
		if (pPersistSettings)
		{
			RETURN_IF_FAILED(pPersistSettings->Load(pMainWindowSettings));
		}

		CComQIPtr<IControl> pControl = pWindow;

		HWND hwnd = NULL;
		RETURN_IF_FAILED(pControl->CreateEx(NULL, &hwnd));

		CString strCaption = L"minitwi";
		if (sizeof(INT_PTR) == 8)
			strCaption += L" x64";

#ifdef DEBUG
		strCaption += L" DEBUG";
#endif

		SetWindowText(hwnd, strCaption);
		RETURN_IF_FAILED(pWindow->SetIcon(icon, TRUE));
		RETURN_IF_FAILED(pWindow->SetIcon(icon, FALSE));

		if (hwnd == NULL)
		{
			ATLTRACE(_T("Main window creation failed!\n"));
			return 0;
		}

		BOOL bResult = FALSE;
		RETURN_IF_FAILED(pWindow->ShowWindow(nCmdShow, &bResult));

		nRet = theLoop.Run();

		_Module.RemoveMessageLoop();
		pPluginManager->Shutdown();
	}

	Gdiplus::GdiplusShutdown(g_gdiPlusToken);
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	CCoInitializeScope comScope;

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
#pragma warning(suppress: 6387)
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

	HRESULT hRes = _Module.Init(NULL, hInstance);
	hRes;
	ATLASSERT(SUCCEEDED(hRes));

	AtlAxWinInit();

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	return nRet;
}
