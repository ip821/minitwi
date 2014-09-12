// ViewControllerService.h : Declaration of the CViewControllerService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "asyncsvc_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;

// CViewControllerService

class ATL_NO_VTABLE CViewControllerService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CViewControllerService, &CLSID_ViewControllerService>,
	public IViewControllerService,
	public IPluginSupportNotifications,
	public IThreadServiceEventSink,
	public IInitializeWithControlImpl,
	public IInitializeWithSettings,
	public IInfoControlEventSink
{
public:
	CViewControllerService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_VIEWCONTROLLERSERVICE)

	BEGIN_COM_MAP(CViewControllerService)
		COM_INTERFACE_ENTRY(IViewControllerService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IInfoControlEventSink)
	END_COM_MAP()

private:
	CComPtr<ITimerService> m_pTimerService;
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<IInfoControlService> m_pInfoControlService;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IThreadPoolService> m_pThreadPoolService;
	CComPtr<IUpdateService> m_pUpdateService;
	CComPtr<ITheme> m_pTheme;

	BOOL m_bUpdateAvailable = FALSE;
	DWORD m_dwAdvice = 0;
	DWORD m_dwInfoControlAdvice = 0;
	CComPtr<ISettings> m_pSettings;
	CComQIPtr<ITimelineControl> m_pTimelineControl;
	HWND m_hWndInfoControl = 0;

	STDMETHOD(ShowControl)(BSTR bstrMessage, BOOL bError);
	STDMETHOD(HideControl)();
	void ApplyThemeToInfoControl(HWND hWndInfoControl);
	void UnapplyThemeToInfoControl();
public:

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(Load)(ISettings* pSettings);

	STDMETHOD(OnStart)(IVariantObject *pResult);
	METHOD_EMPTY(STDMETHOD(OnRun)(IVariantObject *pResult));
	STDMETHOD(OnFinish)(IVariantObject *pResult);
	STDMETHOD(StartTimers)();
	STDMETHOD(StopTimers)();
	STDMETHOD(SetTheme)(ITheme* pTheme);

	STDMETHOD(OnLinkClick)(HWND hWnd);
};

OBJECT_ENTRY_AUTO(__uuidof(ViewControllerService), CViewControllerService)
