// TimelineUpdateImageService.h : Declaration of the CTimelineUpdateImageService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "asyncsvc_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;

// CTimelineUpdateImageService

class ATL_NO_VTABLE CTimelineUpdateImageService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineUpdateImageService, &CLSID_TimelineUpdateImageService>,
	public ITimelineUpdateImageService,
	public IPluginSupportNotifications,
	public IDownloadServiceEventSink,
	public ITimerServiceEventSink,
	public IInitializeWithControlImpl
{
public:
	CTimelineUpdateImageService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_TIMELINEUPDATEIMAGESERVICE)

	BEGIN_COM_MAP(CTimelineUpdateImageService)
		COM_INTERFACE_ENTRY(ITimelineUpdateImageService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IDownloadServiceEventSink)
		COM_INTERFACE_ENTRY(ITimerServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
	END_COM_MAP()

private:
	DWORD m_dwAdvice = 0;
	DWORD m_dwAdviceTimer = 0;
	CComPtr<IDownloadService> m_pDownloadService;
	CComPtr<IImageManagerService> m_pImageManagerService;
	CComPtr<ITimerService> m_pTimerService;
	CComQIPtr<ITimelineControl> m_pTimelineControl;
	hash_set<wstring> m_idsToUpdate;
	mutex m_mutex;

public:

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnDownloadComplete)(IVariantObject *pResult);
	STDMETHOD(OnTimer)();
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineUpdateImageService), CTimelineUpdateImageService)
