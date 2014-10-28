// TimelineService.h : Declaration of the CTimelineService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "asyncsvc_contract_i.h"
#include "twitconn_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"
#include <mutex>

#pragma warning(push)
#pragma warning(disable:4245)
#include <boost\date_time.hpp>
#include <boost\date_time\c_local_time_adjustor.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#pragma warning(pop)

using namespace ATL;
using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

// CTimelineService

class ATL_NO_VTABLE CTimelineService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineService, &CLSID_TimelineService>,
	public ITimelineService,
	public IThreadServiceEventSink,
	public IInitializeWithSettings,
	public IPluginSupportNotifications,
	public IInitializeWithControlImpl,
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
		COM_INTERFACE_ENTRY(ITimelineControlEventSink)
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
	mutex m_mutex;
	TIME_ZONE_INFORMATION m_tz;
	BOOL m_bShowMoreRunning = 0;
	CComBSTR m_bstrUser;

	STDMETHOD(UpdateRelativeTime)(IObjArray* pObjectArray);

public:
	STDMETHOD(Load)(ISettings *pSettings);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(OnColumnClick)(BSTR bstrColumnName, DWORD dwColumnIndex, IColumnRects* pColumnRects, IVariantObject* pVariantObject);
	METHOD_EMPTY(STDMETHOD(OnItemRemoved)(IVariantObject *pItemObject));
	STDMETHOD(SetTimelineControl)(ITimelineControl* pTimelineControl);
	STDMETHOD(SetUserId)(BSTR bstrUser);
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineService), CTimelineService)
