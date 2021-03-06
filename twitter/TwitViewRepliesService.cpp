#include "stdafx.h"
#include "TwitViewRepliesService.h"
#include "Plugins.h"
#include "twconn_contract_i.h"
#include "HomeTimelineService.h"
#include "UpdateScope.h"

STDMETHODIMP CTwitViewRepliesService::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;

	CComQIPtr<ITimelineControlSupport> pTimelineControlSupport = m_pControl;
	ATLASSERT(pTimelineControlSupport);
	RETURN_IF_FAILED(pTimelineControlSupport->GetTimelineControl(&m_pTimelineControl));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &m_pThreadServiceQueueService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_QUEUE, &m_pTimelineQueueService));

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_UPDATE_THREAD, &m_pThreadService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));

	return S_OK;
}

STDMETHODIMP CTwitViewRepliesService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_pSettings.Release();
	}

	m_pThreadServiceQueueService.Release();
	m_pTimelineQueueService.Release();
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

STDMETHODIMP CTwitViewRepliesService::Load(ISettings* pSettings)
{
	CHECK_E_POINTER(pSettings);
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
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
		boost::lock_guard<boost::mutex> lock(m_mutex);
		pSettings = m_pSettings;
	}

	CComPtr<ISettings> pSettingsTwitter;
	RETURN_IF_FAILED(pSettings->OpenSubSettings(Twitter::Metadata::Settings::PathRoot, &pSettingsTwitter));

	CComBSTR bstrKey;
	RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, Twitter::Metadata::Settings::Twitter::TwitterKey, &bstrKey));

	CComBSTR bstrSecret;
	RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, Twitter::Metadata::Settings::Twitter::TwitterSecret, &bstrSecret));

	CComPtr<ITwitterConnection> pConnection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TwitterConnection, &pConnection));
	RETURN_IF_FAILED(pConnection->OpenConnectionWithAppAuth());

	CComVar vParentTwitId;
	RETURN_IF_FAILED(pVariantObjectMember->GetVariantValue(Twitter::Connection::Metadata::TweetObject::InReplyToStatusId, &vParentTwitId));

	CComVar vId;
	RETURN_IF_FAILED(pVariantObjectMember->GetVariantValue(ObjectModel::Metadata::Object::Id, &vId));
	ATLASSERT(vId.vt == VT_BSTR);

	CComVar vOriginalId;
	RETURN_IF_FAILED(pVariantObjectMember->GetVariantValue(Twitter::Connection::Metadata::TweetObject::OriginalId, &vOriginalId));

	if (!m_bParentRetrieved)
	{
		if (vOriginalId.vt == VT_BSTR)
		{
			CComPtr<IVariantObject> pOriginalItem;
			RETURN_IF_FAILED(pConnection->GetTwit(vOriginalId.bstrVal, &pOriginalItem));
			CComVar vOriginalParentTwitId;
			RETURN_IF_FAILED(pOriginalItem->GetVariantValue(Twitter::Connection::Metadata::TweetObject::InReplyToStatusId, &vOriginalParentTwitId));
			if (vOriginalParentTwitId.vt == VT_BSTR)
			{
				CComPtr<IVariantObject> pParentItem;
				RETURN_IF_FAILED(pConnection->GetTwit(vOriginalParentTwitId.bstrVal, &pParentItem));
				RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Object::ParentResult, &CComVar(pParentItem)));
				m_bParentRetrieved = true;
			}
		}

		if (!m_bParentRetrieved && vParentTwitId.vt == VT_BSTR)
		{
			CComPtr<IVariantObject> pParentItem;
			RETURN_IF_FAILED(pConnection->GetTwit(vParentTwitId.bstrVal, &pParentItem));
			RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Object::ParentResult, &CComVar(pParentItem)));
			m_bParentRetrieved = true;
		}
	}

	CComVar vUserName;
	RETURN_IF_FAILED(pVariantObjectMember->GetVariantValue(Twitter::Connection::Metadata::UserObject::Name, &vUserName));
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

		CComVar vInReplyToStatusId;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::TweetObject::InReplyToStatusId, &vInReplyToStatusId));

		if (vInReplyToStatusId.vt != VT_BSTR)
			continue;

		if (CComBSTR(vInReplyToStatusId.bstrVal) != CComBSTR(vId.bstrVal) &&
			(vOriginalId.vt != VT_BSTR || CComBSTR(vInReplyToStatusId.bstrVal) != CComBSTR(vOriginalId.bstrVal)))
			continue;

		RETURN_IF_FAILED(pObjectCollectionResult->AddObject(pVariantObject));
	}

	RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Object::Result, &CComVar(pObjectCollectionResult)));

	return S_OK;
}

STDMETHODIMP CTwitViewRepliesService::OnFinish(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);

	CComVar vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));
	if (FAILED(vHr.intVal))
	{
		return S_OK;
	}

	CUpdateScope scope(m_pTimelineControl);

	auto insertIndex = -1;

	CComVar vParentTwit;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::ParentResult, &vParentTwit));

	if (vParentTwit.vt == VT_UNKNOWN)
	{
		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObject));
		CComPtr<IObjCollection> pObjCollection;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pObjCollection));

		CComQIPtr<IVariantObject> pParentItem = vParentTwit.punkVal;
		RETURN_IF_FAILED(pObjCollection->AddObject(pParentItem));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Metadata::Object::Result, &CComVar(pObjCollection)));
		RETURN_IF_FAILED(m_pTimelineQueueService->AddToQueue(pVariantObject));
	}

	RETURN_IF_FAILED(pResult->SetVariantValue(ObjectModel::Metadata::Object::Index, &CComVar(insertIndex)));
	RETURN_IF_FAILED(m_pTimelineQueueService->AddToQueue(pResult));
	RETURN_IF_FAILED(m_pThreadServiceQueueService->Run());
	return S_OK;
}
