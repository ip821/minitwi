// PictureWindow.h : Declaration of the CPictureWindow

#pragma once
#include "resource.h"       // main symbols
#include "twview_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"
#include "Plugins.h"

using namespace ATL;
using namespace std;
using namespace Gdiplus;

#define PICTURE_WINDOW_SETTINGS_PATH L"PictureWindow"
#define PICTURE_WINDOW_SETTINGS_DISABLE_MONITOR_SNAP_KEY L"DisableMonitorSnap"

// CPictureWindow

class ATL_NO_VTABLE CPictureWindow :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPictureWindow, &CLSID_PictureWindow>,
	public CWindowImpl<CPictureWindow>,
	public IPictureWindow,
	public IThemeSupport,
	public IInitializeWithControlImpl,
	public IInitializeWithSettings,
	public IInitializeWithVariantObject,
	public IDownloadServiceEventSink,
	public IPluginSupportNotifications,
	public IConnectionPointContainerImpl<CPictureWindow>,
	public IConnectionPointImpl<CPictureWindow, &__uuidof(IWindowEventSink)>
{
public:
	DECLARE_WND_CLASS(L"PictureWindow")
	CPictureWindow();
	~CPictureWindow();

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CPictureWindow)
		COM_INTERFACE_ENTRY(IPictureWindow)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IDownloadServiceEventSink)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(IWindow)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IThemeSupport)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CPictureWindow)
		CONNECTION_POINT_ENTRY(__uuidof(IWindowEventSink))
	END_CONNECTION_POINT_MAP()

	BEGIN_MSG_MAP(CPictureWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtomUp)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtomUp)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_UPDATE_VIEW_CONTROL, OnUpdateViewControl)
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
	CComPtr<ISettings> m_pSettings;
	CComPtr<IControl> m_pViewControl;
	CMenu m_popupMenu;
	DWORD m_dwAdviceDownloadService = 0;
	int m_currentBitmapIndex = -1;
	vector<CComBSTR> m_bitmapsUrls;
	vector<CComBSTR> m_videoUrls;
	vector<CComBSTR> m_videoFilePaths;
	vector<CSize> m_sizes;
	boost::mutex m_mutex;
	CIcon m_icon;
	HWND m_hWndParent = 0;
	HWND m_hWndViewControl = 0;
	CString m_strLastErrorMsg;

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
	LRESULT OnUpdateViewControl(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void OnFinalMessage(HWND hWnd);
	void CalcRect(int width, int height, CRect& rect);
	STDMETHOD(StartNextDownload)(int index);
	void ResizeWindow(UINT uiWidth, UINT uiHeight);
	void ResizeToCurrentBitmap();
	HMONITOR GetHMonitor();
	STDMETHOD(InitCommandSupport)(int index);
	STDMETHOD(ShutdownViewControl());
	STDMETHOD(LoadViewControl());
	void MoveToPicture(BOOL bForward);
	void AdjustSize();

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
};

OBJECT_ENTRY_AUTO(__uuidof(PictureWindow), CPictureWindow)
