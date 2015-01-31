// TimelineService.h : Declaration of the CTimelineService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "asyncsvc_contract_i.h"
#include "twitconn_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;
using namespace IP;

// CTimelineService

class ATL_NO_VTABLE CTimelineService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineService, &CLSID_TimelineService>,
	public ITimelineService,
	public IThreadServiceEventSink,
	public IInitializeWithSettings,
	public IPluginSupportNotifications,
	public IInitializeWithControlImpl,
	public ITimelineControlEventSink,
	public IInitializeWithVariantObject
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
		COM_INTERFACE_ENTRY(ITimelineControlEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
	END_COM_MAP()

private:
	CComPtr<ISettings> m_pSettings;
	CComPtr<IThreadService> m_pThreadServiceUpdateService;
	CComPtr<IThreadService> m_pThreadServiceShowMoreService;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComQIPtr<ITimelineControl> m_pTimelineControl;
	DWORD m_dwAdviceThreadServiceUpdateService = 0;
	DWORD m_dwAdviceThreadServiceShowMoreService = 0;
	DWORD m_dwAdviceTimelineControl = 0;
	boost::mutex m_mutex;
	BOOL m_bShowMoreRunning = 0;
	CComBSTR m_bstrUser;


public:
	STDMETHOD(Load)(ISettings *pSettings);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(OnColumnClick)(IColumnsInfoItem* pColumnsInfoItem, IVariantObject* pVariantObject);
	METHOD_EMPTY(STDMETHOD(OnItemRemoved)(IVariantObject *pItemObject));
	METHOD_EMPTY(STDMETHOD(OnItemDoubleClick)(IVariantObject* pVariantObject));
	
	STDMETHOD(SetVariantObject)(IVariantObject* pVariantObject);
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineService), CTimelineService)
