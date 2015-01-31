#pragma once

using namespace ATL;
using namespace std;
using namespace IP;

#include "twitter_i.h"
#include "..\model-libs\asyncsvc\asyncsvc_contract_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"
#include "..\twiconn\Plugins.h"

class ATL_NO_VTABLE CListsTimelineService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CListsTimelineService, &CLSID_TwitViewRepliesService>,
	public IPluginSupportNotifications,
	public IThreadServiceEventSink,
	public IInitializeWithSettings,
	public ITimelineService,
	public IInitializeWithControlImpl
{
public:
	CListsTimelineService()
	{

	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CListsTimelineService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ITimelineService)
	END_COM_MAP()

private:
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<ISettings> m_pSettings;
	CComPtr<ITimelineControl> m_pTimelineControl;

	DWORD m_dwAdvice = 0;
	boost::mutex m_mutex;

public:
	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(Load)(ISettings* pSettings);

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);
};

OBJECT_ENTRY_AUTO(__uuidof(ListsTimelineService), CListsTimelineService)