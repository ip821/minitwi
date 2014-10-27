#pragma once

using namespace ATL;
using namespace std;

#include "twitter_i.h"
#include "..\model-libs\asyncsvc\asyncsvc_contract_i.h"

class ATL_NO_VTABLE CTwitViewRepliesService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTwitViewRepliesService, &CLSID_TwitViewRepliesService>,
	public IPluginSupportNotifications,
	public IThreadServiceEventSink,
	public IInitializeWithSettings,
	public ITwitViewRepliesService,
	public IInitializeWithVariantObject
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
	END_COM_MAP()

private:
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<ISettings> m_pSettings;
	CComPtr<ITimelineControl> m_pTimelineControl;

	DWORD m_dwAdvice = 0;
	mutex m_mutex;

public:
	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(SetTimelineControl)(ITimelineControl *pTimelineControl);

	STDMETHOD(Load)(ISettings* pSettings);

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	METHOD_EMPTY(STDMETHOD(SetVariantObject)(IVariantObject* pVariantObject));
};

OBJECT_ENTRY_AUTO(__uuidof(TwitViewRepliesService), CTwitViewRepliesService)