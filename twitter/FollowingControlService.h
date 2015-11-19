#pragma once

using namespace ATL;
using namespace std;
using namespace IP;

#include "twmdl_i.h"
#include "asyncmdl_contract_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"
#include "..\twiconn\Plugins.h"

class ATL_NO_VTABLE CFollowingControlService :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CFollowingControlService, &CLSID_TwitViewRepliesService>,
    public IPluginSupportNotifications,
    public IThreadServiceEventSink,
    public IInitializeWithSettings,
    public ITimelineService,
    public IInitializeWithControlImpl
{
public:
    CFollowingControlService()
    {

    }

    DECLARE_NO_REGISTRY()

    BEGIN_COM_MAP(CFollowingControlService)
        COM_INTERFACE_ENTRY(IPluginSupportNotifications)
        COM_INTERFACE_ENTRY(IThreadServiceEventSink)
        COM_INTERFACE_ENTRY(IInitializeWithSettings)
        COM_INTERFACE_ENTRY(IInitializeWithControl)
        COM_INTERFACE_ENTRY(ITimelineService)
    END_COM_MAP()

private:
    CComPtr<IServiceProvider> m_pServiceProvider;
    CComPtr<IThreadService> m_pThreadUpdateService;
    CComPtr<IThreadService> m_pThreadShowMoreService;
    CComPtr<ISettings> m_pSettings;
    CComPtr<ITimelineControl> m_pTimelineControl;
    CComPtr<IThreadService> m_pThreadServiceQueueService;
    CComPtr<ITimelineQueueService> m_pTimelineQueueService;

    DWORD m_dwAdviceUpdate = 0;
    DWORD m_dwAdviceShowMore = 0;
    boost::mutex m_mutex;

public:
    STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
    STDMETHOD(OnShutdown)();

    STDMETHOD(Load)(ISettings* pSettings);

    STDMETHOD(OnStart)(IVariantObject *pResult);
    STDMETHOD(OnRun)(IVariantObject *pResult);
    STDMETHOD(OnFinish)(IVariantObject *pResult);
};

OBJECT_ENTRY_AUTO(__uuidof(FollowingControlService), CFollowingControlService)