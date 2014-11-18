// TimelineImageService.h : Declaration of the CTimelineImageService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "asyncsvc_contract_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"
#include "..\twiconn\Plugins.h"
#include <boost/thread/condition_variable.hpp>

using namespace ATL;
using namespace std;

// CTimelineImageService

class ATL_NO_VTABLE CTimelineImageService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineImageService, &CLSID_TimelineImageService>,
	public ITimelineImageService,
	public IPluginSupportNotifications,
	public ITimerServiceEventSink,
	public IInitializeWithControlImpl,
	public ITimelineControlEventSink,
	public IThreadServiceEventSink,
	public IDownloadServiceEventSink
{
public:
	CTimelineImageService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_TimelineImageService)


	BEGIN_COM_MAP(CTimelineImageService)
		COM_INTERFACE_ENTRY(ITimelineImageService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ITimelineControlEventSink)
		COM_INTERFACE_ENTRY(ITimerServiceEventSink)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IDownloadServiceEventSink)
	END_COM_MAP()

private:
	CComQIPtr<ITimelineControl> m_pTimelineControl;
	CComPtr<ITimerService> m_pTimerServiceUpdate;
	CComPtr<IImageManagerService> m_pImageManagerService;
	CComPtr<IThreadService> m_pThreadServiceUpdateService;
	CComPtr<IThreadService> m_pThreadServiceShowMoreService;
	CComPtr<IDownloadService> m_pDownloadService;

	DWORD m_dwAdviceDownloadService = 0;
	DWORD m_dwAdviceThreadServiceUpdateService = 0;
	DWORD m_dwAdviceThreadServiceShowMoreService = 0;
	DWORD m_dwAdviceTimerServiceUpdate = 0;
	DWORD m_dwAdviceTimelineControl = 0;

	hash_set<IVariantObject*> m_idsToUpdate;
	boost::mutex m_mutex;
	map<wstring, hash_set<IVariantObject*>> m_imageRefs;

	STDMETHOD(ProcessUrls)(IObjArray* pObjectArray);
	static HRESULT GetUrls(IVariantObject* pItemObject, vector<wstring>& urls);

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
	METHOD_EMPTY(STDMETHOD(OnItemDoubleClick)(IVariantObject* pVariantObject));

	STDMETHOD(SetTimelineControl)(ITimelineControl* pTimelineControl);
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineImageService), CTimelineImageService)
