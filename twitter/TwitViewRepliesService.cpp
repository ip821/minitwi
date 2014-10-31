#include "stdafx.h"
#include "TwitViewRepliesService.h"
#include "Plugins.h"
#include "twitconn_contract_i.h"
#include "TimelineService.h"
#include "UpdateScope.h"

STDMETHODIMP CTwitViewRepliesService::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;

	m_tz = { 0 };
	GetTimeZoneInformation(&m_tz);

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &m_pThreadService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));

	return S_OK;
}

STDMETHODIMP CTwitViewRepliesService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	{
		lock_guard<mutex> lock(m_mutex);
		m_pSettings.Release();
	}

	m_pTimelineControl.Release();
	m_pThreadService.Release();
	m_pServiceProvider.Release();
	m_pSettings.Release();
	m_pVariantObject.Release();
	IInitializeWithControlImpl::OnShutdown();

	return S_OK;
}

STDMETHODIMP CTwitViewRepliesService::SetVariantObject(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	m_pVariantObject = pVariantObject;
	return S_OK;
}

STDMETHODIMP CTwitViewRepliesService::SetTimelineControl(ITimelineControl *pTimelineControl)
{
	CHECK_E_POINTER(pTimelineControl);
	m_pTimelineControl = pTimelineControl;
	return S_OK;
}

STDMETHODIMP CTwitViewRepliesService::Load(ISettings* pSettings)
{
	CHECK_E_POINTER(pSettings);
	{
		lock_guard<mutex> lock(m_mutex);
		m_pSettings = pSettings;
	}
	return S_OK;
}

STDMETHODIMP CTwitViewRepliesService::OnStart(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);
	return S_OK;
}

STDMETHODIMP CTwitViewRepliesService::OnRun(IVariantObject *pResult)
{
	CComPtr<ITwitViewRepliesService> guard = this;
	CComPtr<IVariantObject> pVariantObjectMember = m_pVariantObject;
	CHECK_E_POINTER(pResult);
	CComPtr<ISettings> pSettings;
	{
		lock_guard<mutex> lock(m_mutex);
		pSettings = m_pSettings;
	}

	CComPtr<ISettings> pSettingsTwitter;
	RETURN_IF_FAILED(pSettings->OpenSubSettings(SETTINGS_PATH, &pSettingsTwitter));

	CComBSTR bstrKey;
	RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, KEY_TWITTERKEY, &bstrKey));

	CComBSTR bstrSecret;
	RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, KEY_TWITTERSECRET, &bstrSecret));

	CComPtr<ITwitterConnection> pConnection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TwitterConnection, &pConnection));
	RETURN_IF_FAILED(pConnection->OpenConnectionWithAppAuth());

	CComVariant vParentTwitId;
	RETURN_IF_FAILED(pVariantObjectMember->GetVariantValue(VAR_TWITTER_IN_REPLYTO_STATUS_ID, &vParentTwitId));

	CComVariant vId;
	RETURN_IF_FAILED(pVariantObjectMember->GetVariantValue(VAR_ID, &vId));
	ATLASSERT(vId.vt == VT_BSTR);

	CComVariant vOriginalId;
	RETURN_IF_FAILED(pVariantObjectMember->GetVariantValue(VAR_TWITTER_ORIGINAL_ID, &vOriginalId));

	if (!m_bParentRetrieved)
	{
		if (vOriginalId.vt == VT_BSTR)
		{
			CComPtr<IVariantObject> pOriginalItem;
			RETURN_IF_FAILED(pConnection->GetTwit(vOriginalId.bstrVal, &pOriginalItem));
			CComVariant vOriginalParentTwitId;
			RETURN_IF_FAILED(pOriginalItem->GetVariantValue(VAR_TWITTER_IN_REPLYTO_STATUS_ID, &vOriginalParentTwitId));
			if (vOriginalParentTwitId.vt == VT_BSTR)
			{
				CComPtr<IVariantObject> pParentItem;
				RETURN_IF_FAILED(pConnection->GetTwit(vOriginalParentTwitId.bstrVal, &pParentItem));
				RETURN_IF_FAILED(pResult->SetVariantValue(VAR_PARENT_RESULT, &CComVariant(pParentItem)));
				m_bParentRetrieved = true;
			}
		}

		if (!m_bParentRetrieved && vParentTwitId.vt == VT_BSTR)
		{
			CComPtr<IVariantObject> pParentItem;
			RETURN_IF_FAILED(pConnection->GetTwit(vParentTwitId.bstrVal, &pParentItem));
			RETURN_IF_FAILED(pResult->SetVariantValue(VAR_PARENT_RESULT, &CComVariant(pParentItem)));
			m_bParentRetrieved = true;
		}
	}

	CComVariant vUserName;
	RETURN_IF_FAILED(pVariantObjectMember->GetVariantValue(VAR_TWITTER_USER_NAME, &vUserName));
	ATLASSERT(vUserName.vt == VT_BSTR);

	CComPtr<IObjArray> pObjectArray;
	RETURN_IF_FAILED(pConnection->Search(CComBSTR(CString(L"@") + vUserName.bstrVal), vOriginalId.vt == VT_BSTR ? vOriginalId.bstrVal : vId.bstrVal, 100, &pObjectArray));

	CComPtr<IObjCollection> pObjectCollectionResult;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pObjectCollectionResult));

	UINT uiCollectionCount = 0;
	RETURN_IF_FAILED(pObjectArray->GetCount(&uiCollectionCount));
	for (size_t i = 0; i < uiCollectionCount; i++)
	{
		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(pObjectArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));

		CComVariant vInReplyToStatusId;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_IN_REPLYTO_STATUS_ID, &vInReplyToStatusId));

		if (vInReplyToStatusId.vt != VT_BSTR)
			continue;

		if (CComBSTR(vInReplyToStatusId.bstrVal) != CComBSTR(vId.bstrVal) &&
			(vOriginalId.vt != VT_BSTR || CComBSTR(vInReplyToStatusId.bstrVal) != CComBSTR(vOriginalId.bstrVal)))
			continue;

		RETURN_IF_FAILED(pObjectCollectionResult->AddObject(pVariantObject));
	}

	RETURN_IF_FAILED(pResult->SetVariantValue(VAR_RESULT, &CComVariant(pObjectCollectionResult)));

	return S_OK;
}

STDMETHODIMP CTwitViewRepliesService::OnFinish(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);

	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(KEY_HRESULT, &vHr));
	if (FAILED(vHr.intVal))
	{
		return S_OK;
	}

	CUpdateScope scope(m_pTimelineControl);

	auto insertIndex = 1;

	CComVariant vParentTwit;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_PARENT_RESULT, &vParentTwit));

	if (vParentTwit.vt == VT_UNKNOWN)
	{
		CComQIPtr<IVariantObject> pParentItem = vParentTwit.punkVal;
		RETURN_IF_FAILED(CTimelineService::UpdateRelativeTimeForTwit(pResult, m_tz));
		RETURN_IF_FAILED(m_pTimelineControl->InsertItem(pParentItem, 0));
		RETURN_IF_FAILED(m_pTimelineControl->RefreshItem(0));
		++insertIndex;
	}

	CComVariant vResult;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_RESULT, &vResult));
	CComQIPtr<IObjArray> pObjectArray = vResult.punkVal;

	RETURN_IF_FAILED(CTimelineService::UpdateRelativeTime(pObjectArray, m_tz));
	RETURN_IF_FAILED(m_pTimelineControl->InsertItems(pObjectArray, insertIndex));
	CComPtr<IObjArray> pAllItemsObjectArray;
	RETURN_IF_FAILED(m_pTimelineControl->GetItems(&pAllItemsObjectArray));
	RETURN_IF_FAILED(CTimelineService::UpdateRelativeTime(pAllItemsObjectArray, m_tz));

	return S_OK;
}
