#pragma once
#include "resource.h"       // main symbols
#include "twmdl_i.h"
#include "asyncmdl_contract_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"
#include "..\twiconn\Plugins.h"

using namespace ATL;
using namespace std;
using namespace IP;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

class ATL_NO_VTABLE CTimelineRelativeTimeService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineRelativeTimeService, &CLSID_TimelineImageService>,
	public ITimelineRelativeTimeService,
	public IPluginSupportNotifications,
	public IInitializeWithControlImpl,
	public IThreadServiceEventSink
{
public:
	CTimelineRelativeTimeService()
	{
	}

	DECLARE_NO_REGISTRY()


	BEGIN_COM_MAP(CTimelineRelativeTimeService)
		COM_INTERFACE_ENTRY(ITimelineRelativeTimeService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
	END_COM_MAP()

private:
	CComPtr<ITimelineControl> m_pTimelineControl;
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<IThreadService> m_pThreadServiceShowMoreService;

	DWORD m_dwAdviceThreadService = 0;
	DWORD m_dwAdviceThreadServiceShowMoreService = 0;

	HRESULT UpdateRelativeTimeForTwit(IVariantObject* pVariantObject);
	HRESULT UpdateRelativeTime(IObjArray* pObjectArray);
public:

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	METHOD_EMPTY(STDMETHOD(OnStart)(IVariantObject *pResult));
	METHOD_EMPTY(STDMETHOD(OnRun)(IVariantObject *pResult));
	STDMETHOD(OnFinish)(IVariantObject *pResult);
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineRelativeTimeService), CTimelineRelativeTimeService)
