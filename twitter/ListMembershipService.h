#pragma once
#include "resource.h"       // main symbols
#include "twmdl_i.h"
#include "asyncmdl_contract_i.h"
#include "twconn_contract_i.h"

using namespace ATL;
using namespace std;
using namespace IP;

// ListsService

class ATL_NO_VTABLE CListMembershipService :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CListMembershipService, &CLSID_ListMembershipService>,
    public IPluginSupportNotifications,
    public IListMembershipService,
    public IThreadServiceEventSink,
    public IInitializeWithSettings
{
public:
    CListMembershipService()
    {
    }

    DECLARE_NO_REGISTRY()

    BEGIN_COM_MAP(CListMembershipService)
        COM_INTERFACE_ENTRY(IPluginSupportNotifications)
        COM_INTERFACE_ENTRY(IListMembershipService)
        COM_INTERFACE_ENTRY(IThreadServiceEventSink)
        COM_INTERFACE_ENTRY(IInitializeWithSettings)
    END_COM_MAP()

private:
    CComPtr<IServiceProvider> m_pServiceProvider;
    CComPtr<IThreadService> m_pThreadService;
    CComPtr<ITimerService> m_pTimerService;
    CComPtr<ISettings> m_pSettings;
    map<CComBSTR, CComPtr<IObjArray>> m_listMembers;
    boost::mutex m_mutex;
    DWORD m_dwAdviceThreadService = 0;

public:
    STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
    STDMETHOD(OnShutdown)();

    STDMETHOD(OnStart)(IVariantObject* pObject);
    STDMETHOD(OnRun)(IVariantObject* pObject);
    STDMETHOD(OnFinish)(IVariantObject* pObject);

    STDMETHOD(GetListMemebers)(BSTR bstrListId, IObjArray** ppArrayMembers);
    STDMETHOD(RefreshListMemebers)(BSTR bstrListId);

    STDMETHOD(Load)(ISettings *pSettings);
};

OBJECT_ENTRY_AUTO(__uuidof(ListMembershipService), CListMembershipService)
