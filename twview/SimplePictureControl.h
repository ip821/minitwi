// SimplePictureControl.h : Declaration of the CSimplePictureControl

#pragma once
#include "resource.h"       // main symbols
#include "twview_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"
#include "Plugins.h"

using namespace ATL;
using namespace std;
using namespace Gdiplus;

// CSimplePictureControl

class ATL_NO_VTABLE CSimplePictureControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSimplePictureControl, &CLSID_SimplePictureControl>,
	public CWindowImpl<CSimplePictureControl>,
	public IControl,
	public IInitializeWithControlImpl,
	public IInitializeWithVariantObject,
	public IPluginSupportNotifications
{
public:
	DECLARE_WND_CLASS(L"SimplePictureControl")
	CSimplePictureControl();
	~CSimplePictureControl();

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CSimplePictureControl)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IControl)
	END_COM_MAP()

	BEGIN_MSG_MAP(CSimplePictureControl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtomUp)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtomUp)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		DEFAULT_MESSAGE_HANDLER(OnMessage)
	END_MSG_MAP()

private:
	CComQIPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IImageManagerService> m_pImageManagerService;
	CComPtr<IDownloadService> m_pDownloadService;
	CComPtr<IPluginSupport> m_pPluginSupport;
	CComPtr<ICommandSupport> m_pCommandSupport;
	CComPtr<IMessageLoop> m_pMessageLoop;
	CComPtr<ITheme> m_pTheme;
	CComPtr<IAnimationService> m_pAnimationService;
	CComPtr<ISettings> m_pSettings;
	CMenu m_popupMenu;
	DWORD m_dwAdviceDownloadService = 0;
	DWORD m_dwAdviceAnimationService = 0;
	int m_currentBitmapIndex = -1;
	vector<CComBSTR> m_bitmapsUrls;
	boost::mutex m_mutex;
	CIcon m_icon;
	HWND m_hWndParent = 0;
	CString m_strLastErrorMsg;

	const DWORD STEPS = 25;
	BOOL m_bInitialMonitorDetection = TRUE;
	BOOL m_bDisableMonitorSnap = FALSE;

	HRESULT Fire_OnClosed(HWND hWnd);

	LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRButtomUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtomUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void OnFinalMessage(HWND hWnd);
	void CalcRect(int width, int height, CRect& rect);
	STDMETHOD(StartNextDownload)(int index);
	void ResizeWindow(UINT uiWidth, UINT uiHeight);
	void ResizeToCurrentBitmap();
	HMONITOR GetHMonitor();
	STDMETHOD(InitCommandSupport)(int index);
	void MoveToPicture(BOOL bForward);

public:
	STDMETHOD(Show)(HWND hWndParent);

	STDMETHOD(GetHWND)(HWND* phWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *bResult);

	STDMETHOD(SetVariantObject)(IVariantObject *pVariantObject);
	STDMETHOD(OnDownloadComplete)(IVariantObject *pResult);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(SetTheme)(ITheme* pTheme);

	STDMETHOD(Load)(ISettings *pSettings);

	STDMETHOD(OnAnimationStep)(IAnimationService *pAnimationService, DWORD dwValue, DWORD dwStep);
};

OBJECT_ENTRY_AUTO(__uuidof(SimplePictureControl), CSimplePictureControl)
