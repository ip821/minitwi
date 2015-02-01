#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"
#include "..\twiconn\Plugins.h"
#include "asyncsvc_contract_i.h"
#include "twitconn_contract_i.h"

using namespace ATL;
using namespace IP;

class ATL_NO_VTABLE CLoginService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLoginService, &CLSID_FormsService>,
	public IPluginSupportNotifications,
	public IInitializeWithControlImpl,
	public IInitializeWithSettings,
	public IThreadServiceEventSink
{
public:

	CLoginService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CLoginService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
	END_COM_MAP()

private:
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<IFormManager> m_pFormManager;
	CComPtr<IViewControllerService> m_pViewControllerService;
	CComPtr<ISettings> m_pSettings;

	DWORD m_dwAdvice = 0;
public:
	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(Load)(ISettings* pSettings);

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);
};

OBJECT_ENTRY_AUTO(__uuidof(LoginService), CLoginService)