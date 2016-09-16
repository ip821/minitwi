// HomeTimelineStreamingService.h : Declaration of the CHomeTimelineStreamingService

#pragma once
#include "resource.h"       // main symbols
#include "twmdl_i.h"
#include "asyncmdl_contract_i.h"
#include "twconn_contract_i.h"
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
	public ITwitterStreamingConnectionEventSink,
	public IHomeTimelineStreamingService,
    public IConnectionPointContainerImpl<CHomeTimelineStreamingService>,
    public IConnectionPointImpl<CHomeTimelineStreamingService, &__uuidof(IHomeTimelineStreamingServiceEventSink)>
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
		COM_INTERFACE_ENTRY(IHomeTimelineStreamingService)
        COM_INTERFACE_ENTRY(IConnectionPointContainer)
	END_COM_MAP()

    BEGIN_CONNECTION_POINT_MAP(CHomeTimelineStreamingService)
        CONNECTION_POINT_ENTRY(__uuidof(IHomeTimelineStreamingServiceEventSink))
    END_CONNECTION_POINT_MAP()


private:
	CComPtr<ISettings> m_pSettings;
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<ITwitterStreamingConnection> m_pTwitterStreamingConnection;
	DWORD m_dwAdviceThreadService = 0;
	boost::mutex m_mutex;

    HRESULT Fire_OnMessages(IObjArray* pMessageArray);

public:
	STDMETHOD(Load)(ISettings *pSettings);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(OnMessages)(IObjArray *pObjectArray);

	STDMETHOD(Stop)();
};

OBJECT_ENTRY_AUTO(__uuidof(HomeTimelineStreamingService), CHomeTimelineStreamingService)
