// HomeTimelineControllerService.h : Declaration of the CHomeTimelineControllerService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "asyncsvc_contract_i.h"
#include "twitconn_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;
using namespace IP;

// CHomeTimelineControllerService

class ATL_NO_VTABLE CHomeTimelineControllerService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CHomeTimelineControllerService, &CLSID_HomeTimelineControllerService>,
	public IThreadServiceEventSink,
	public IPluginSupportNotifications,
	public IPluginSupportNotifications2,
	public IHomeTimelineControllerService
{
public:
	CHomeTimelineControllerService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CHomeTimelineControllerService)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications2)
		COM_INTERFACE_ENTRY(IHomeTimelineControllerService)
	END_COM_MAP()

private:
	CComPtr<IThreadService> m_pThreadServiceStreamingService;
	CComPtr<IThreadService> m_pThreadServiceUpdateService;
	CComPtr<ITimelineQueueService> m_pTimelineQueueService;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<ITimerService> m_pTimerService;
	DWORD m_dwAdviceThreadServiceStreamingService = 0;
	DWORD m_dwAdviceThreadServiceUpdateService = 0;
	boost::mutex m_mutex;

public:
	METHOD_EMPTY(STDMETHOD(OnInitializing)(IServiceProvider *pServiceProvider));
	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnInitCompleted)();
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(StartConnection)();
	STDMETHOD(StopConnection)();
};

OBJECT_ENTRY_AUTO(__uuidof(HomeTimelineControllerService), CHomeTimelineControllerService)
