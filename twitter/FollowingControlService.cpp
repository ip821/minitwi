#include "stdafx.h"
#include "FollowingControlService.h"
#include "Plugins.h"
#include "twconn_contract_i.h"
#include "HomeTimelineService.h"
#include "UpdateScope.h"

STDMETHODIMP CFollowingControlService::OnInitialized(IServiceProvider* pServiceProvider)
{
    CHECK_E_POINTER(pServiceProvider);
    m_pServiceProvider = pServiceProvider;

    CComPtr<IUnknown> pUnk;
    RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
    RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_UPDATE_THREAD, &m_pThreadUpdateService));
    RETURN_IF_FAILED(AtlAdvise(m_pThreadUpdateService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceUpdate));
    RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_SHOWMORE_THREAD, &m_pThreadShowMoreService));
    RETURN_IF_FAILED(AtlAdvise(m_pThreadShowMoreService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceShowMore));

    CComQIPtr<ITimelineControlSupport> pTimelineControlSupport = m_pControl;
    ATLASSERT(pTimelineControlSupport);
    RETURN_IF_FAILED(pTimelineControlSupport->GetTimelineControl(&m_pTimelineControl));

    RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &m_pThreadServiceQueueService));
    RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_QUEUE, &m_pTimelineQueueService));

    CComPtr<ITimelineLoadingService> pLoadingService;
    RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_TimelineLoadingService, &pLoadingService));
    RETURN_IF_FAILED(pLoadingService->SetText(L"Loading following..."));

    return S_OK;
}

STDMETHODIMP CFollowingControlService::OnShutdown()
{
    RETURN_IF_FAILED(AtlUnadvise(m_pThreadUpdateService, __uuidof(IThreadServiceEventSink), m_dwAdviceUpdate));
    RETURN_IF_FAILED(AtlUnadvise(m_pThreadShowMoreService, __uuidof(IThreadServiceEventSink), m_dwAdviceShowMore));
    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        m_pSettings.Release();
    }

    m_pThreadServiceQueueService.Release();
    m_pTimelineQueueService.Release();
    m_pTimelineControl.Release();
    m_pThreadUpdateService.Release();
    m_pThreadShowMoreService.Release();
    m_pServiceProvider.Release();
    IInitializeWithControlImpl::OnShutdown();

    return S_OK;
}

STDMETHODIMP CFollowingControlService::Load(ISettings* pSettings)
{
    CHECK_E_POINTER(pSettings);
    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        m_pSettings = pSettings;
    }
    return S_OK;
}

STDMETHODIMP CFollowingControlService::OnStart(IVariantObject *pResult)
{
    CHECK_E_POINTER(pResult);
    return S_OK;
}

STDMETHODIMP CFollowingControlService::OnRun(IVariantObject *pResult)
{
    CComPtr<CFollowingControlService> guard = this;
    CHECK_E_POINTER(pResult);
    CComPtr<ISettings> pSettings;
    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        pSettings = m_pSettings;
    }

    CComPtr<ISettings> pSettingsTwitter;
    RETURN_IF_FAILED(pSettings->OpenSubSettings(Twitter::Metadata::Settings::PathRoot, &pSettingsTwitter));

    CComBSTR bstrKey;
    RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, Twitter::Metadata::Settings::Twitter::TwitterKey, &bstrKey));

    CComBSTR bstrSecret;
    RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, Twitter::Metadata::Settings::Twitter::TwitterSecret, &bstrSecret));

    CComBSTR bstrUser;
    RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, Twitter::Metadata::Settings::Twitter::User, &bstrUser));

    CComPtr<ITwitterConnection> pConnection;
    RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TwitterConnection, &pConnection));
    RETURN_IF_FAILED(pConnection->OpenConnection(bstrKey, bstrSecret));

    CComBSTR bstrNextCursor(L"-1");

    CComVar vId;
    RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::MaxId, &vId));

    if (vId.vt == VT_BSTR)
    {
        bstrNextCursor = vId.bstrVal;
    }

    if (bstrNextCursor == L"0")
        return S_OK;

    CComPtr<IVariantObject> pVariantObject;
    RETURN_IF_FAILED(pConnection->GetFollowingUsers(bstrUser, bstrNextCursor, &pVariantObject));

    CComVar vNextCursor;
    RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::CursorObject::NextCursor, &vNextCursor));

    CComVar vItems;
    RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::CursorObject::Items, &vItems));
    ATLASSERT(vItems.vt == VT_UNKNOWN);

    CComQIPtr<IObjArray> pObjectArray = vItems.punkVal;

    {
        UINT uiCount = 0;
        RETURN_IF_FAILED(pObjectArray->GetCount(&uiCount));
        for (UINT i = 0; i < uiCount; i++)
        {
            CComPtr<IVariantObject> pItem;
            RETURN_IF_FAILED(pObjectArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pItem));
            RETURN_IF_FAILED(pItem->SetVariantValue(ObjectModel::Metadata::Object::Id, &vNextCursor));
        }
    }

    RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Object::Result, &CComVar(pObjectArray)));

    return S_OK;
}

STDMETHODIMP CFollowingControlService::OnFinish(IVariantObject *pResult)
{
    CHECK_E_POINTER(pResult);

    CUpdateScope scope(m_pTimelineControl);

    CComVar vHr;
    RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));
    if (FAILED(vHr.intVal))
    {
        return S_OK;
    }

    CComVar vResult;
    RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::Result, &vResult));

    if (vResult.vt == VT_EMPTY)
        return S_OK;

    CComQIPtr<IObjArray> pObjectArray = vResult.punkVal;

    UINT uiCount = 0;
    RETURN_IF_FAILED(pObjectArray->GetCount(&uiCount));
    if (uiCount)
    {
        RETURN_IF_FAILED(m_pTimelineQueueService->AddToQueue(pResult));
        RETURN_IF_FAILED(m_pThreadServiceQueueService->Run());
    }
    else
    {
        CComPtr<IVariantObject> pNoListsFoundObject;
        RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pNoListsFoundObject));
        RETURN_IF_FAILED(pNoListsFoundObject->SetVariantValue(ObjectModel::Metadata::Object::Type, &CComVar(Twitter::Metadata::Types::CustomTimelineObject)));
        RETURN_IF_FAILED(pNoListsFoundObject->SetVariantValue(Twitter::Metadata::Object::Text, &CComVar(L"No users found")));
        RETURN_IF_FAILED(pNoListsFoundObject->SetVariantValue(Twitter::Metadata::Item::VAR_ITEM_DISABLED_TEXT, &CComVar(L"No users found")));
        RETURN_IF_FAILED(pNoListsFoundObject->SetVariantValue(Twitter::Metadata::Item::VAR_ITEM_DISABLED, &CComVar(true)));
        RETURN_IF_FAILED(m_pTimelineControl->InsertItem(pNoListsFoundObject, 0));
    }

    return S_OK;
}
