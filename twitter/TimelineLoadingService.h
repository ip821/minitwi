// TimelineLoadingService.h : Declaration of the CTimelineLoadingService

#pragma once
#include "resource.h"       // main symbols
#include "twmdl_i.h"
#include "asyncsvc_contract_i.h"
#include "twconn_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;
using namespace IP;

// CTimelineLoadingService

class ATL_NO_VTABLE CTimelineLoadingService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineLoadingService, &CLSID_TimelineLoadingService>,
	public IThreadServiceEventSink,
	public IPluginSupportNotifications,
	public IInitializeWithControlImpl,
	public ITimelineLoadingService
{
public:
	CTimelineLoadingService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CTimelineLoadingService)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(ITimelineLoadingService)
	END_COM_MAP()

private:
	CComPtr<IThreadService> m_pThreadServiceUpdateService;
	CComPtr<IThreadService> m_pThreadServiceShowMoreService;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComQIPtr<ITimelineControl> m_pTimelineControl;
	DWORD m_dwAdviceThreadServiceUpdateService = 0;
	DWORD m_dwAdviceThreadServiceShowMoreService = 0;
	CString m_strText;

public:
	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	METHOD_EMPTY(STDMETHOD(OnRun)(IVariantObject *pResult));
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(SetText)(BSTR bstrText);
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineLoadingService), CTimelineLoadingService)
