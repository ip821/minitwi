#pragma once

#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "asyncsvc_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;

class ATL_NO_VTABLE CTabbedControlStatusService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTabbedControlStatusService, &CLSID_ViewControllerService>,
	public IPluginSupportNotifications,
	public IThreadServiceEventSink
{
public:
	CTabbedControlStatusService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CTabbedControlStatusService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
	END_COM_MAP()

private:
	CComQIPtr<IViewControllerService> m_pViewControllerService;
	CComPtr<IThreadService> m_pThreadServiceShowMoreTimeline;
	CComPtr<IThreadService> m_pThreadServiceUpdateTimeline;
	CComPtr<IThreadService> m_pThreadServiceFollow;

	DWORD m_dwAdviceUpdateTimeline = 0;
	DWORD m_dwAdviceShowMoreTimeline = 0;
	DWORD m_dwAdviceFollow = 0;

	STDMETHOD(StopAnimation)();
public:
	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	METHOD_EMPTY(STDMETHOD(OnRun)(IVariantObject *pResult));
	STDMETHOD(OnFinish)(IVariantObject *pResult);
};

OBJECT_ENTRY_AUTO(__uuidof(TabbedControlStatusService), CTabbedControlStatusService)