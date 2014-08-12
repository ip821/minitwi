// ViewControllerService.h : Declaration of the CViewControllerService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "NotificationServices_contract_i.h"

using namespace ATL;

// CViewControllerService

class ATL_NO_VTABLE CViewControllerService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CViewControllerService, &CLSID_ViewControllerService>,
	public IViewControllerService,
	public IPluginSupportNotifications
{
public:
	CViewControllerService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_VIEWCONTROLLERSERVICE)

	BEGIN_COM_MAP(CViewControllerService)
		COM_INTERFACE_ENTRY(IViewControllerService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
	END_COM_MAP()

private:
	CComPtr<ITimerService> m_pTimerService;

public:

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

};

OBJECT_ENTRY_AUTO(__uuidof(ViewControllerService), CViewControllerService)
