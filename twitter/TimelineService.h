// TimelineService.h : Declaration of the CTimelineService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "asyncsvc_contract_i.h"
#include "twitconn_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"
#include <mutex>

using namespace ATL;

// CTimelineService

class ATL_NO_VTABLE CTimelineService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineService, &CLSID_TimelineService>,
	public ITimelineService,
	public IThreadServiceEventSink,
	public IInitializeWithSettings,
	public IPluginSupportNotifications,
	public IInitializeWithControlImpl,
	public IDownloadServiceEventSink,
	public ITimerServiceEventSink,
	public ITimelineControlEventSink
{
public:
	CTimelineService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_TIMELINESERVICE)

	BEGIN_COM_MAP(CTimelineService)
		COM_INTERFACE_ENTRY(ITimelineService)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IDownloadServiceEventSink)
		COM_INTERFACE_ENTRY(ITimerServiceEventSink)
		COM_INTERFACE_ENTRY(ITimelineControlEventSink)
	END_COM_MAP()

private:
	CComPtr<ISettings> m_pSettings;
	CComPtr<IThreadService> m_pThreadServiceUpdateService;
	CComPtr<IThreadService> m_pThreadServiceShowMoreService;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComQIPtr<ITimelineControl> m_pTimelineControl;
	CComPtr<IDownloadService> m_pDownloadService;
	CComPtr<IImageManagerService> m_pImageManagerService;
	CComPtr<ITimerService> m_pTimerService;
	DWORD m_dwAdviceThreadServiceUpdateService = 0;
	DWORD m_dwAdviceThreadServiceShowMoreService = 0;
	DWORD m_dwAdviceTimerService = 0;
	DWORD m_dwAdviceDownloadService = 0;
	DWORD m_dwAdviceTimelineControl = 0;
	std::hash_set<std::wstring> m_idsToUpdate;
	std::mutex m_mutex;
	TIME_ZONE_INFORMATION m_tz;
	BOOL m_bShowMoreRunning = 0;

	STDMETHOD(ProcessUrls)(IObjArray* pObjectArray);
	STDMETHOD(UpdateRelativeTime)(IObjArray* pObjectArray);
	STDMETHOD(ProcessAllItems)();

public:
	STDMETHOD(Load)(ISettings *pSettings);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(OnTimer)();

	STDMETHOD(OnDownloadComplete)(IVariantObject *pResult);

	STDMETHOD(OnColumnClick)(BSTR bstrColumnName, DWORD dwColumnIndex, IColumnRects* pColumnRects, IVariantObject* pVariantObject);
	METHOD_EMPTY(STDMETHOD(OnItemRemoved)(IVariantObject *pItemObject));

	static HRESULT GetTimelineControl(IControl* pControl, CComQIPtr<ITimelineControl>& pTimelineControl);
	static HRESULT GetUrls(IVariantObject* pItemObject, std::vector<std::wstring>& urls);
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineService), CTimelineService)
