// VideoViewControl.h : Declaration of the CVideoViewControl

#pragma once
#include "resource.h"       // main symbols
#include "twview_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"
#include "Plugins.h"

using namespace ATL;
using namespace std;
using namespace Gdiplus;

// CVideoViewControl

class ATL_NO_VTABLE CVideoViewControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CVideoViewControl, &CLSID_VideoViewControl>,
	public CWindowImpl<CVideoViewControl>,
	public IControl,
	public IInitializeWithControlImpl,
	public IInitializeWithVariantObject,
	public IPluginSupportNotifications,
	public IThemeSupport
{
public:
	DECLARE_WND_CLASS(L"VideoViewControl")
	CVideoViewControl();
	~CVideoViewControl();

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CVideoViewControl)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IThemeSupport)
	END_COM_MAP()

	BEGIN_MSG_MAP(CVideoViewControl)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnForwardMessage)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnForwardMessage)
		MESSAGE_HANDLER(WM_PLAYER_STARTED, OnPlayerStarted)
		MESSAGE_HANDLER(WM_PLAYER_FINISHED, OnPlayerFinished)
	END_MSG_MAP()

private:
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IVariantObject> m_pVariantObject;
	CComPtr<ITheme> m_pTheme;
	CComBSTR m_bstrPath;

	CString m_strLastErrorMsg;
	BOOL m_bPlayerStarted = FALSE;
	BOOL m_bPlaying = TRUE;
	HANDLE m_hProcess = 0;
	HWND m_hWndPlayer = 0;

	LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnForwardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPlayerStarted(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPlayerFinished(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	STDMETHOD(GetHWND)(HWND* phWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *bResult);

	STDMETHOD(SetVariantObject)(IVariantObject *pVariantObject);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(SetTheme)(ITheme *pTheme);
};

OBJECT_ENTRY_AUTO(__uuidof(VideoViewControl), CVideoViewControl)
