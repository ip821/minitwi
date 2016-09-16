#pragma once
#include "resource.h"       // main symbols
#include "twmdl_i.h"
#include "asyncmdl_contract_i.h"
#include "twconn_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;
using namespace IP;

// ListTimelineStreamingServiceEventListener

class ATL_NO_VTABLE CListTimelineStreamingServiceEventListener :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CListTimelineStreamingServiceEventListener, &CLSID_ListTimelineStreamingServiceEventListener>,
    public IPluginSupportNotifications,
    public IHomeTimelineStreamingServiceEventSink
{
public:
    CListTimelineStreamingServiceEventListener()
    {
    }

    DECLARE_NO_REGISTRY()

    BEGIN_COM_MAP(CListTimelineStreamingServiceEventListener)
        COM_INTERFACE_ENTRY(IPluginSupportNotifications)
        COM_INTERFACE_ENTRY(IHomeTimelineStreamingServiceEventSink)
    END_COM_MAP()

private:
    CComPtr<IServiceProvider> m_pServiceProvider;
    CComPtr<IUnknown> m_pStreamingServicUnk;
    CComPtr<IListTimelineControlService> m_pListTimelineService;
    DWORD m_dwAdvice = 0;
    unordered_set<wstring> m_userIds;
    boost::mutex m_mutex;

    HRESULT Fire_OnMessages(IObjArray* pMessageArray);

public:
    STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
    STDMETHOD(OnShutdown)();

    STDMETHOD(OnMessages)(IObjArray *pObjectArray);
};

OBJECT_ENTRY_AUTO(__uuidof(ListTimelineStreamingServiceEventListener), CListTimelineStreamingServiceEventListener)
