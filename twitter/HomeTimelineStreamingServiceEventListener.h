#pragma once
#include "resource.h"       // main symbols
#include "twmdl_i.h"
#include "asyncmdl_contract_i.h"
#include "twconn_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;
using namespace IP;

// HomeTimelineStreamingServiceEventListener

class ATL_NO_VTABLE CHomeTimelineStreamingServiceEventListener :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CHomeTimelineStreamingServiceEventListener, &CLSID_HomeTimelineStreamingServiceEventListener>,
    public IPluginSupportNotifications,
    public IHomeTimelineStreamingServiceEventSink
{
public:
    CHomeTimelineStreamingServiceEventListener()
    {
    }

    DECLARE_NO_REGISTRY()

    BEGIN_COM_MAP(CHomeTimelineStreamingServiceEventListener)
        COM_INTERFACE_ENTRY(IPluginSupportNotifications)
        COM_INTERFACE_ENTRY(IHomeTimelineStreamingServiceEventSink)
    END_COM_MAP()

private:
    CComPtr<IServiceProvider> m_pServiceProvider;
    CComPtr<IUnknown> m_pStreamingServicUnk;
    DWORD m_dwAdvice = 0;
    boost::mutex m_mutex;

    HRESULT Fire_OnMessages(IObjArray* pMessageArray);

public:
    STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
    STDMETHOD(OnShutdown)();

    STDMETHOD(OnMessages)(IObjArray *pObjectArray);
};

OBJECT_ENTRY_AUTO(__uuidof(HomeTimelineStreamingServiceEventListener), CHomeTimelineStreamingServiceEventListener)
