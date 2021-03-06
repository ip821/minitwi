#pragma once

using namespace ATL;
using namespace std;
using namespace IP;

#include "twmdl_i.h"
#include "asyncmdl_contract_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"
#include "..\twiconn\Plugins.h"

class ATL_NO_VTABLE CTwitViewRepliesService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTwitViewRepliesService, &CLSID_TwitViewRepliesService>,
	public IPluginSupportNotifications,
	public IThreadServiceEventSink,
	public IInitializeWithSettings,
	public ITwitViewRepliesService,
	public IInitializeWithVariantObject,
	public IInitializeWithControlImpl
{
public:
	CTwitViewRepliesService()
	{

	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CTwitViewRepliesService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(ITwitViewRepliesService)
		COM_INTERFACE_ENTRY(ITimelineService)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
	END_COM_MAP()

private:
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<ISettings> m_pSettings;
	CComPtr<ITimelineControl> m_pTimelineControl;
	CComPtr<IVariantObject> m_pVariantObject;
	CComPtr<IThreadService> m_pThreadServiceQueueService;
	CComPtr<ITimelineQueueService> m_pTimelineQueueService;

	DWORD m_dwAdvice = 0;
	boost::mutex m_mutex;

	bool m_bParentRetrieved = false;

public:
	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(Load)(ISettings* pSettings);

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(SetVariantObject)(IVariantObject* pVariantObject);
};

OBJECT_ENTRY_AUTO(__uuidof(TwitViewRepliesService), CTwitViewRepliesService)