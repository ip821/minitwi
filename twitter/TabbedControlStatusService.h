#pragma once

#include "resource.h"       // main symbols
#include "twmdl_i.h"
#include "asyncmdl_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;
using namespace IP;

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
	CComPtr<IThreadService> m_pThreadServiceFollowStatus;
	CComPtr<IThreadService> m_pThreadServiceStreamingTimeline;
	CComPtr<IThreadService> m_pThreadServiceGetUser;

	DWORD m_dwAdviceUpdateTimeline = 0;
	DWORD m_dwAdviceShowMoreTimeline = 0;
	DWORD m_dwAdviceFollow = 0;
	DWORD m_dwAdviceFollowStatus = 0;
	DWORD m_dwAdviceStreamingTimeline = 0;
	DWORD m_dwAdviceGetUser = 0;
	UINT m_cAnimationRefs = 0;

	STDMETHOD(StartAnimation)();
	STDMETHOD(StopAnimation)();
public:
	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	METHOD_EMPTY(STDMETHOD(OnRun)(IVariantObject *pResult));
	STDMETHOD(OnFinish)(IVariantObject *pResult);
};

OBJECT_ENTRY_AUTO(__uuidof(TabbedControlStatusService), CTabbedControlStatusService)