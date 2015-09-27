// ViewControllerService.h : Declaration of the CViewControllerService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "asyncsvc_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;

// CViewControllerService

class ATL_NO_VTABLE CViewControllerService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CViewControllerService, &CLSID_ViewControllerService>,
	public IViewControllerService,
	public IPluginSupportNotifications,
	public IPluginSupportNotifications2,
	public IInitializeWithControlImpl,
	public IInitializeWithSettings,
	public IInfoControlEventSink,
	public ITabbedControlEventSink,
	public IMsgFilter,
	public IUpdateServiceEventSink,
	public ICustomTabControlEventSink
{
public:
	CViewControllerService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_VIEWCONTROLLERSERVICE)

	BEGIN_COM_MAP(CViewControllerService)
		COM_INTERFACE_ENTRY(IViewControllerService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications2)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IInfoControlEventSink)
		COM_INTERFACE_ENTRY(ITabbedControlEventSink)
		COM_INTERFACE_ENTRY(IMsgFilter)
		COM_INTERFACE_ENTRY(IUpdateServiceEventSink)
		COM_INTERFACE_ENTRY(ICustomTabControlEventSink)
	END_COM_MAP()

private:
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IThreadPoolService> m_pThreadPoolService;
	CComPtr<IUpdateService> m_pUpdateService;
	CComQIPtr<ICustomTabControl> m_pTabbedControl;
	CComPtr<ISettings> m_pSettings;
	CComPtr<IFormsService> m_pFormsService;
	CComQIPtr<IMessageLoop> m_pMessageLoop;

	vector<CComPtr<IControl>> m_controlsStack;

	DWORD m_dwControlsCount = 0;
	BOOL m_bUpdateAvailable = FALSE;
	DWORD m_dwAdviceTabbedControl = 0;
	DWORD m_dwAdviceTabbedControl2 = 0;
	DWORD m_dwAdviceUpdateService = 0;
	DWORD m_dwAdviceCustomtabControl = 0;

public:

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	METHOD_EMPTY(STDMETHOD(OnInitializing)(IServiceProvider *pServiceProvider));
	STDMETHOD(OnInitCompleted());
	STDMETHOD(OnShutdown)();

	STDMETHOD(Load)(ISettings* pSettings);

	STDMETHOD(SetTheme)(ITheme* pTheme);
	STDMETHOD(StartAnimation)();
	STDMETHOD(StopAnimation)();
	STDMETHOD(ShowInfo)(HRESULT hr, BOOL bError, BOOL bInfoImageEnableClick, BSTR bstrMessage);
	STDMETHOD(HideInfo)();

	STDMETHOD(OnLinkClick)(HWND hWnd);

	METHOD_EMPTY(STDMETHOD(OnClose)(IControl* pControl));
	STDMETHOD(OnActivate)(IControl* pControl);
	STDMETHOD(OnDeactivate)(IControl* pControl);
	STDMETHOD(OnTabHeaderClick)(IControl* pControl);

	STDMETHOD(PreTranslateMessage(MSG *pMsg, BOOL *bResult));

	STDMETHOD(OnUpdateAvailable)();

	STDMETHOD(OnBackButtonClicked)();
};

OBJECT_ENTRY_AUTO(__uuidof(ViewControllerService), CViewControllerService)
