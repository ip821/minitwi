// HomeTimelineService.h : Declaration of the CHomeTimelineService

#pragma once
#include "resource.h"       // main symbols
#include "twmdl_i.h"
#include "asyncsvc_contract_i.h"
#include "twconn_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;
using namespace IP;

// CHomeTimelineService

class ATL_NO_VTABLE CHomeTimelineService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CHomeTimelineService, &CLSID_HomeTimelineService>,
	public ITimelineService,
	public IThreadServiceEventSink,
	public IInitializeWithSettings,
	public IPluginSupportNotifications,
	public IInitializeWithControlImpl
{
public:
	CHomeTimelineService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CHomeTimelineService)
		COM_INTERFACE_ENTRY(ITimelineService)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
	END_COM_MAP()

private:
	CComPtr<ISettings> m_pSettings;
	CComPtr<IThreadService> m_pThreadServiceUpdateService;
	CComPtr<IThreadService> m_pThreadServiceShowMoreService;
	CComPtr<IThreadService> m_pThreadServiceQueueService;
	CComPtr<ITimelineQueueService> m_pTimelineQueueService;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComQIPtr<ITimelineControl> m_pTimelineControl;
	DWORD m_dwAdviceThreadServiceUpdateService = 0;
	DWORD m_dwAdviceThreadServiceShowMoreService = 0;
	boost::mutex m_mutex;

public:
	STDMETHOD(Load)(ISettings *pSettings);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);
};

OBJECT_ENTRY_AUTO(__uuidof(HomeTimelineService), CHomeTimelineService)
