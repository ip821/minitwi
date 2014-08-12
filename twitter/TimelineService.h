// TimelineService.h : Declaration of the CTimelineService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "NotificationServices_contract_i.h"
#include <mutex>

using namespace ATL;

// CTimelineService

class ATL_NO_VTABLE CTimelineService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineService, &CLSID_TimelineService>,
	public ITimelineService,
	public IThreadServiceEventSink,
	public IInitializeWithSettings,
	public IPluginSupportNotifications
{
public:
	CTimelineService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_TIMELINESERVICE)

	BEGIN_COM_MAP(CTimelineService)
		COM_INTERFACE_ENTRY(ITimelineService)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
	END_COM_MAP()

private:
	CComPtr<ISettings> m_pSettings;
	CComPtr<IThreadService> m_pThreadService;
	DWORD m_dwAdvice = 0;
	std::mutex m_mutex;

public:
	STDMETHOD(Load)(ISettings *pSettings);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineService), CTimelineService)
