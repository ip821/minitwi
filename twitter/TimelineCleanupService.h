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
	public ITimelineControlEventSink,
	public IThreadServiceEventSink,
	public IDownloadServiceEventSink
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
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IDownloadServiceEventSink)
	END_COM_MAP()

private:
	CComQIPtr<ITimelineControl> m_pTimelineControl;
	CComPtr<ITimerService> m_pTimerServiceCleanup;
	CComPtr<ITimerService> m_pTimerServiceUpdate;
	CComPtr<IImageManagerService> m_pImageManagerService;
	CComPtr<IThreadService> m_pThreadServiceUpdateService;
	CComPtr<IThreadService> m_pThreadServiceShowMoreService;
	CComPtr<IDownloadService> m_pDownloadService;

	DWORD m_dwAdviceDownloadService = 0;
	DWORD m_dwAdviceThreadServiceUpdateService = 0;
	DWORD m_dwAdviceThreadServiceShowMoreService = 0;
	DWORD m_dwAdviceTimerServiceCleanup = 0;
	DWORD m_dwAdviceTimerServiceUpdate = 0;
	DWORD m_dwAdviceTimelineControl = 0;

	std::hash_set<std::wstring> m_idsToUpdate;
	std::mutex m_mutex;
	map<wstring, int> m_imageRefs;

	DWORD m_counter = 0;

	STDMETHOD(ProcessUrls)(IObjArray* pObjectArray);
public:

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnTimer)(ITimerService* pTimerService);

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(OnDownloadComplete)(IVariantObject *pResult);

	STDMETHOD(OnItemRemoved)(IVariantObject *pItemObject);
	METHOD_EMPTY(STDMETHOD(OnColumnClick)(BSTR bstrColumnName, DWORD dwColumnIndex, IColumnRects* pColumnRects, IVariantObject* pVariantObject));
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineCleanupService), CTimelineCleanupService)
