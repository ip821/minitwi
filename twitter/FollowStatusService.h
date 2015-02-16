// FollowStatusService.h : Declaration of the CFollowStatusService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "asyncsvc_contract_i.h"
#include "twitconn_contract_i.h"
#include "..\twiconn\Plugins.h"
#include "Plugins.h"

using namespace ATL;
using namespace std;
using namespace IP;

// CFollowStatusService

class ATL_NO_VTABLE CFollowStatusService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CFollowStatusService, &CLSID_FollowStatusService>,
	public IPluginSupportNotifications,
	public IThreadServiceEventSink,
	public IInitializeWithVariantObject,
	public IInitializeWithSettings
{
public:
	CFollowStatusService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CFollowStatusService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
	END_COM_MAP()

private:
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<IVariantObject> m_pVariantObject;
	CComPtr<ISettings> m_pSettings;
	DWORD m_dwAdvice = 0;
	boost::mutex m_mutex;

public:
	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(SetVariantObject)(IVariantObject *pVariantObject);

	STDMETHOD(Load)(ISettings *pSettings);

	METHOD_EMPTY(STDMETHOD(OnStart)(IVariantObject *pResult));
	STDMETHOD(OnRun)(IVariantObject *pResult);
	METHOD_EMPTY(STDMETHOD(OnFinish)(IVariantObject *pResult));
};

OBJECT_ENTRY_AUTO(__uuidof(FollowStatusService), CFollowStatusService)
