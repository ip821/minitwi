// TimelineQueueService.h : Declaration of the CTimelineQueueService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "asyncsvc_contract_i.h"
#include "twitconn_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;
using namespace IP;

// CTimelineQueueService

class ATL_NO_VTABLE CTimelineQueueService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineQueueService, &CLSID_TimelineQueueService>,
	public IThreadServiceEventSink,
	public IPluginSupportNotifications,
	public IInitializeWithControlImpl,
	public ITimelineQueueService
{
public:
	CTimelineQueueService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CTimelineQueueService)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(ITimelineQueueService)
	END_COM_MAP()

private:
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComQIPtr<ITimelineControl> m_pTimelineControl;
	boost::mutex m_mutex;
	DWORD m_dwAdviceThreadService = 0;
	queue<CComPtr<IVariantObject>> m_queue;

public:
	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(AddToQueue)(IVariantObject *pResult);
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineQueueService), CTimelineQueueService)
