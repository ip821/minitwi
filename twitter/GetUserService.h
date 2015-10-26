#pragma once
#include "resource.h"       // main symbols
#include "twmdl_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"
#include "..\twiconn\Plugins.h"
#include "asyncsvc_contract_i.h"
#include "twconn_contract_i.h"

using namespace ATL;
using namespace std;
using namespace IP;

class ATL_NO_VTABLE CGetUserService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CGetUserService, &CLSID_FormsService>,
	public IPluginSupportNotifications,
	public IInitializeWithVariantObject,
	public IInitializeWithSettings,
	public IThreadServiceEventSink
{
public:

	CGetUserService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CGetUserService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
	END_COM_MAP()

private:
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<ISettings> m_pSettings;
	CComPtr<IVariantObject> m_pVariantObject;

	boost::mutex m_mutex;
	DWORD m_dwAdvice = 0;

public:
	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(SetVariantObject)(IVariantObject *pVariantObject);

	STDMETHOD(Load)(ISettings* pSettings);

	METHOD_EMPTY(STDMETHOD(OnStart)(IVariantObject *pResult));
	STDMETHOD(OnRun)(IVariantObject *pResult);
	METHOD_EMPTY(STDMETHOD(OnFinish)(IVariantObject *pResult));
};

OBJECT_ENTRY_AUTO(__uuidof(GetUserService), CGetUserService)