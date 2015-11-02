// UserTimelineService.h : Declaration of the CUserTimelineService

#pragma once
#include "resource.h"       // main symbols
#include "twmdl_i.h"
#include "asyncmdl_contract_i.h"
#include "twconn_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;
using namespace IP;

// CUserTimelineService

class ATL_NO_VTABLE CUserTimelineService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUserTimelineService, &CLSID_UserTimelineService>,
	public ITimelineService,
	public IThreadServiceEventSink,
	public IInitializeWithSettings,
	public IPluginSupportNotifications,
	public IInitializeWithControlImpl,
	public IInitializeWithVariantObject
{
public:
	CUserTimelineService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CUserTimelineService)
		COM_INTERFACE_ENTRY(ITimelineService)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
	END_COM_MAP()

private:
	CComPtr<ISettings> m_pSettings;
	CComPtr<IThreadService> m_pThreadServiceUpdateService;
	CComPtr<IThreadService> m_pThreadServiceShowMoreService;
	CComPtr<IThreadService> m_pThreadServiceQueueService;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComQIPtr<ITimelineControl> m_pTimelineControl;
	CComPtr<ITimelineQueueService> m_pTimelineQueueService;
	DWORD m_dwAdviceThreadServiceUpdateService = 0;
	DWORD m_dwAdviceThreadServiceShowMoreService = 0;
	boost::mutex m_mutex;
	CComBSTR m_bstrUser;


public:
	STDMETHOD(Load)(ISettings *pSettings);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(SetVariantObject)(IVariantObject* pVariantObject);
};

OBJECT_ENTRY_AUTO(__uuidof(UserTimelineService), CUserTimelineService)
