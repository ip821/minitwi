// TimelineCleanupService.h : Declaration of the CTimelineCleanupService

#pragma once
#include "resource.h"       // main symbols
#include "twmdl_i.h"
#include "asyncmdl_contract_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"

using namespace ATL;

// CTimelineCleanupService

class ATL_NO_VTABLE CTimelineCleanupService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineCleanupService, &CLSID_TimelineCleanupService>,
	public ITimelineCleanupService,
	public IPluginSupportNotifications,
	public ITimerServiceEventSink,
	public IInitializeWithControlImpl
{
public:
	CTimelineCleanupService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_TIMELINECLEANUPSERVICE)


	BEGIN_COM_MAP(CTimelineCleanupService)
		COM_INTERFACE_ENTRY(ITimelineCleanupService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(ITimerServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
	END_COM_MAP()

private:
	CComQIPtr<ITimelineControl> m_pTimelineControl;
	CComPtr<ITimerService> m_pTimerServiceCleanup;

	DWORD m_dwAdviceTimerServiceCleanup = 0;
	DWORD m_counter = 0;

public:

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnTimer)(ITimerService* pTimerService);
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineCleanupService), CTimelineCleanupService)
