#pragma once

#include "resource.h"       // main symbols
#include "twmdl_i.h"
#include "asyncmdl_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"
#include "..\ObjMdl\GUIDComparer.h"

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
	UINT m_cAnimationRefs = 0;

    struct ThreadHolder
    {
        CComPtr<IThreadService> m_pThreadService;
        DWORD m_dwAdvice = 0;
    };

    const vector<GUID> m_guids = 
    {
        SERVICE_TIMELINE_UPDATE_THREAD,
        SERVICE_TIMELINE_STREAMING_THREAD,
        SERVICE_TIMELINE_SHOWMORE_THREAD,
        SERVICE_FOLLOW_THREAD,
        SERVICE_FOLLOW_STATUS_THREAD,
        SERVICE_GETUSER_THREAD,
        SERVICE_LISTS_THREAD
    };

    map<GUID, ThreadHolder, GUIDComparer> m_advices;

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