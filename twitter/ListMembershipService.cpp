#include "stdafx.h"
#include "ListMembershipService.h"

STDMETHODIMP CListMembershipService::OnInitialized(IServiceProvider *pServiceProvider)
{
    m_pServiceProvider = pServiceProvider;

    CComPtr<IUnknown> pUnk;
    RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
    RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_LISTS_THREAD, &m_pThreadService));
    RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadService));
    RETURN_IF_FAILED(m_pThreadService->SetTimerService(SERVICE_LISTS_TIMER));

    RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_LISTS_TIMER, &m_pTimerService));
    RETURN_IF_FAILED(m_pTimerService->StartTimer(10 * 60 * 1000)); //10 min

    return S_OK;
}

STDMETHODIMP CListMembershipService::OnShutdown() 
{
    RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadService));
    RETURN_IF_FAILED(m_pTimerService->StopTimer());
    RETURN_IF_FAILED(m_pThreadService->Join());

    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        m_pServiceProvider.Release();
        m_pThreadService.Release();
        m_pTimerService.Release();
        m_pSettings.Release();
    }

    return S_OK;
}

STDMETHODIMP CListMembershipService::RefreshListMemebers(BSTR bstrListId)
{
    CComPtr<IVariantObject> pResult;
    RETURN_IF_FAILED(HrCreateVariantObject(&pResult));
    RETURN_IF_FAILED(pResult->SetVariantValue(ObjectModel::Metadata::Object::Id, &CComVar(bstrListId)));
    RETURN_IF_FAILED(m_pThreadService->SetThreadContext(pResult));
    RETURN_IF_FAILED(m_pThreadService->Run());
    return S_OK;
}

STDMETHODIMP CListMembershipService::GetListMemebers(BSTR bstrListId, IObjArray** ppArrayMembers)
{
    CHECK_E_POINTER(bstrListId);
    CHECK_E_POINTER(ppArrayMembers);

    boost::lock_guard<boost::mutex> lock(m_mutex);
    auto it = m_listMembers.find(CComBSTR(bstrListId));
    if (it == m_listMembers.cend())
    {
        RETURN_IF_FAILED(RefreshListMemebers(bstrListId));
    }
    else
    {
        RETURN_IF_FAILED(it->second.QueryInterface(ppArrayMembers));
    }
    return S_OK;
}

STDMETHODIMP CListMembershipService::Load(ISettings *pSettings)
{
    boost::lock_guard<boost::mutex> lock(m_mutex);
    m_pSettings = pSettings;
    return S_OK;
}

STDMETHODIMP CListMembershipService::OnStart(IVariantObject* pResult)
{
    return S_OK;
}

STDMETHODIMP CListMembershipService::OnRun(IVariantObject* pResult)
{
    CComPtr<CListMembershipService> guard = this;
    CHECK_E_POINTER(pResult);
    CComPtr<ISettings> pSettings;
    CComPtr<IVariantObject> pListVariantObject;
    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        pSettings = m_pSettings;
    }

    CComPtr<ITwitterConnection> pConnection;
    RETURN_IF_FAILED(HrOpenConnection(pSettings, &pConnection));

    CComVar vListId;
    RETURN_IF_FAILED(pListVariantObject->GetVariantValue(ObjectModel::Metadata::Object::Id, &vListId));
    ATLASSERT(vListId.vt == VT_BSTR);

    CComPtr<IObjArray> pObjArray;
    RETURN_IF_FAILED(pConnection->GetListMembers(vListId.bstrVal, 0, &pObjArray));

    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        m_listMembers[vListId.bstrVal] = pObjArray;
    }

    return S_OK;
}

STDMETHODIMP CListMembershipService::OnFinish(IVariantObject* pResult)
{
    return S_OK;
}
