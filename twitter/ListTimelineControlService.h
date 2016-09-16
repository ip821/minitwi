#pragma once

using namespace ATL;
using namespace std;

#include "twmdl_i.h"
#include "asyncmdl_contract_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"
#include "..\twiconn\Plugins.h"

class ATL_NO_VTABLE CListTimelineControlService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CListTimelineControlService, &CLSID_ListTimelineControlService>,
	public IPluginSupportNotifications,
	public IThreadServiceEventSink,
	public IInitializeWithSettings,
	public ITimelineService,
	public IInitializeWithControlImpl,
	public IInitializeWithVariantObject
{
public:
	CListTimelineControlService()
	{

	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CListTimelineControlService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ITimelineService)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
	END_COM_MAP()

private:
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<ISettings> m_pSettings;
	CComPtr<ITimelineControl> m_pTimelineControl;
	CComPtr<IVariantObject> m_pVariantObject;
	CComPtr<IThreadService> m_pThreadServiceQueueService;
	CComPtr<ITimelineQueueService> m_pTimelineQueueService;

	DWORD m_dwAdvice = 0;
	boost::mutex m_mutex;

public:
	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(SetVariantObject)(IVariantObject* pVariantObject);

	STDMETHOD(Load)(ISettings* pSettings);

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);
};

OBJECT_ENTRY_AUTO(__uuidof(ListTimelineControlService), CListTimelineControlService)