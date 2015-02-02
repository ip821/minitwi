// PictureWindow.h : Declaration of the CPictureWindow

#pragma once
#include "resource.h"       // main symbols
#include "twview_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"
#include "AnimationTimerSupport.h"
#include "Plugins.h"

using namespace ATL;
using namespace std;
using namespace Gdiplus;

// CPictureWindow

class ATL_NO_VTABLE CPictureWindow :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPictureWindow, &CLSID_PictureWindow>,
	public CAnimationTimerSupport<CPictureWindow>,
	public CWindowImpl<CPictureWindow>,
	public IPictureWindow,
	public IMsgFilter,
	public IThemeSupport,
	public IInitializeWithControlImpl,
	public IInitializeWithVariantObject,
	public IDownloadServiceEventSink,
	public IPluginSupportNotifications,
	public IConnectionPointContainerImpl<CPictureWindow>,
	public IConnectionPointImpl<CPictureWindow, &__uuidof(IWindowEventSink)>
{
public:
	DECLARE_WND_CLASS(L"PictureWindow")
	CPictureWindow();
	virtual ~CPictureWindow();

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CPictureWindow)
		COM_INTERFACE_ENTRY(IPictureWindow)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IDownloadServiceEventSink)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(IWindow)
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
		MESSAGE_HANDLER(WM_ANIMATION_TIMER, OnAnimationTimer)
	END_MSG_MAP()

private:
	CComQIPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IImageManagerService> m_pImageManagerService;
	CComPtr<IDownloadService> m_pDownloadService;
	CComPtr<IPluginSupport> m_pPluginSupport;
	CComPtr<ICommandSupport> m_pCommandSupport;
	CComPtr<IMessageLoop> m_pMessageLoop;
	CComPtr<ITheme> m_pTheme;
	CMenu m_popupMenu;
	DWORD m_dwAdviceDownloadService = 0;
	int m_currentBitmapIndex = -1;
	vector<CComBSTR> m_bitmapsUrls;
	boost::mutex m_mutex;
	CIcon m_icon;
	HWND m_hWndParent = 0;
	CString m_strLastErrorMsg;

	BYTE m_alpha = 0;
	int m_step = 0;
	BYTE m_alphaAmount = 0;
	const BYTE STEPS = 25;

	HRESULT Fire_OnClosed(HWND hWnd);

	LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRButtomUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtomUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnAnimationTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void OnFinalMessage(HWND hWnd);
	void CalcRect(int width, int height, CRect& rect);
	STDMETHOD(StartNextDownload)(int index);
	STDMETHOD(ResetAnimation)();
	void ResizeWindow(UINT uiWidth, UINT uiHeight);
	void ResizeToCurrentBitmap();
	STDMETHOD(InitCommandSupport)(int index);
	void MoveToPicture(BOOL bForward);

public:
	void OnMMTimer();

	STDMETHOD(Show)(HWND hWndParent);
	STDMETHOD(GetHWND)(HWND* phWnd);

	STDMETHOD(SetVariantObject)(IVariantObject *pVariantObject);
	STDMETHOD(OnDownloadComplete)(IVariantObject *pResult);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *bResult);

	STDMETHOD(SetTheme)(ITheme* pTheme);
};

OBJECT_ENTRY_AUTO(__uuidof(PictureWindow), CPictureWindow)
