// TimelineUpdateImageService.h : Declaration of the CTimelineUpdateImageService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"

using namespace ATL;

// CTimelineUpdateImageService

class ATL_NO_VTABLE CTimelineUpdateImageService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineUpdateImageService, &CLSID_TimelineUpdateImageService>,
	public ITimelineUpdateImageService,
	public IPluginSupportNotifications,
	public IDownloadServiceEventSink
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
	END_COM_MAP()

private:
	DWORD m_dwAdvice = 0;
	CComPtr<IDownloadService> m_pDownloadService;

public:

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnDownloadComplete)(BSTR bstrUrl, BSTR bstrFilePath);
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineUpdateImageService), CTimelineUpdateImageService)
