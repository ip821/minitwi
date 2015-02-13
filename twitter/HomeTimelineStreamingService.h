// HomeTimelineStreamingService.h : Declaration of the CHomeTimelineStreamingService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "asyncsvc_contract_i.h"
#include "twitconn_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;
using namespace IP;

// CHomeTimelineStreamingService

class ATL_NO_VTABLE CHomeTimelineStreamingService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CHomeTimelineStreamingService, &CLSID_HomeTimelineStreamingService>,
	public IThreadServiceEventSink,
	public IInitializeWithSettings,
	public IPluginSupportNotifications,
	public ITwitterStreamingConnectionEventSink
{
public:
	CHomeTimelineStreamingService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CHomeTimelineStreamingService)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(ITwitterStreamingConnectionEventSink)
	END_COM_MAP()

private:
	CComPtr<ISettings> m_pSettings;
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<ITimelineQueueService> m_pTimelineQueueService;
	CComPtr<IServiceProvider> m_pServiceProvider;
	DWORD m_dwAdviceThreadService = 0;
	boost::mutex m_mutex;

public:
	STDMETHOD(Load)(ISettings *pSettings);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(OnMessages)(IObjArray *pObjectArray);
};

OBJECT_ENTRY_AUTO(__uuidof(HomeTimelineStreamingService), CHomeTimelineStreamingService)
