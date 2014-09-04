// TimelineCleanupService.h : Declaration of the CTimelineCleanupService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "asyncsvc_contract_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;

// CTimelineCleanupService

class ATL_NO_VTABLE CTimelineCleanupService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineCleanupService, &CLSID_TimelineCleanupService>,
	public ITimelineCleanupService,
	public IPluginSupportNotifications,
	public ITimerServiceEventSink,
	public IInitializeWithControlImpl,
	public ITimelineControlEventSink
{
public:
	CTimelineCleanupService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_TimelineCleanupService)


	BEGIN_COM_MAP(CTimelineCleanupService)
		COM_INTERFACE_ENTRY(ITimelineCleanupService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ITimelineControlEventSink)
		COM_INTERFACE_ENTRY(ITimerServiceEventSink)
	END_COM_MAP()

private:
	CComQIPtr<ITimelineControl> m_pTimelineControl;
	CComPtr<ITimerService> m_pTimerService;
	CComPtr<IImageManagerService> m_pImageManagerService;
	DWORD m_dwAdviceTimerService = 0;
	DWORD m_dwAdviceTimelineControl = 0;

	DWORD m_counter = 0;
public:

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnTimer)();

	STDMETHOD(OnItemRemoved)(IVariantObject *pItemObject);
	METHOD_EMPTY(STDMETHOD(OnColumnClick)(BSTR bstrColumnName, DWORD dwColumnIndex, IColumnRects* pColumnRects, IVariantObject* pVariantObject));
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineCleanupService), CTimelineCleanupService)
