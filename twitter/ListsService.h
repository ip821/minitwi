#pragma once
#include "resource.h"       // main symbols
#include "twmdl_i.h"
#include "asyncmdl_contract_i.h"
#include "twconn_contract_i.h"

using namespace ATL;
using namespace std;
using namespace IP;

// ListsService

class ATL_NO_VTABLE CListsService :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CListsService, &CLSID_ListsService>,
    public IPluginSupportNotifications,
    public IListsService,
    public IThreadServiceEventSink
{
public:
    CListsService()
    {
    }

    DECLARE_NO_REGISTRY()

    BEGIN_COM_MAP(CListsService)
        COM_INTERFACE_ENTRY(IPluginSupportNotifications)
        COM_INTERFACE_ENTRY(IListsService)
        COM_INTERFACE_ENTRY(IThreadServiceEventSink)
    END_COM_MAP()

private:
    CComPtr<IServiceProvider> m_pServiceProvider;
    boost::mutex m_mutex;

public:
    STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
    STDMETHOD(OnShutdown)();

    STDMETHOD(OnStart)(IVariantObject* pObject);
    STDMETHOD(OnRun)(IVariantObject* pObject);
    STDMETHOD(OnFinish)(IVariantObject* pObject);

    STDMETHOD(GetListMemebers)(IObjArray** ppArrayMembers);
};

OBJECT_ENTRY_AUTO(__uuidof(ListsService), CListsService)
