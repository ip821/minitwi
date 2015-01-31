#include "stdafx.h"
#include "FollowService.h"

STDMETHODIMP CFollowService::OnInitialized(IServiceProvider* pServiceProvider)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	m_pServiceProvider = pServiceProvider;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_FOLLOW_THREAD, &m_pThreadService));

	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));

	return S_OK;
}

STDMETHODIMP CFollowService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	m_pThreadService.Release();
	m_pServiceProvider.Release();

	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_pVariantObject.Release();
		m_pSettings.Release();
	}

	return S_OK;
}

STDMETHODIMP CFollowService::OnRun(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);

	CComPtr<ISettings> pSettings;
	CComPtr<IVariantObject> pVariantObject;
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		pSettings = m_pSettings;
		pVariantObject = m_pVariantObject;
	}

	if (!pSettings)
		return S_OK;

	CComPtr<ISettings> pSettingsTwitter;
	RETURN_IF_FAILED(pSettings->OpenSubSettings(Twitter::Metadata::Settings::PathRoot, &pSettingsTwitter));

	CComBSTR bstrKey;
	RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, Twitter::Metadata::Settings::Twitter::TwitterKey, &bstrKey));

	CComBSTR bstrSecret;
	RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, Twitter::Metadata::Settings::Twitter::TwitterSecret, &bstrSecret));

	CComPtr<ITwitterConnection> pConnection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TwitterConnection, &pConnection));
	RETURN_IF_FAILED(pConnection->OpenConnection(bstrKey, bstrSecret));

	CComVariant vUserName;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Name, &vUserName));
	ATLASSERT(vUserName.vt == VT_BSTR);

	CComVariant vFollowing;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::IsFollowingFlag, &vFollowing));

	auto bFollowing = vFollowing.vt == VT_BOOL && vFollowing.boolVal;

	if (bFollowing)
	{
		RETURN_IF_FAILED(pConnection->UnfollowUser(vUserName.bstrVal));
	}
	else
	{
		RETURN_IF_FAILED(pConnection->FollowUser(vUserName.bstrVal));
	}

	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::UserObject::IsFollowingFlag, &CComVariant(!bFollowing)));

	return S_OK;
}

STDMETHODIMP CFollowService::SetVariantObject(IVariantObject *pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	boost::lock_guard<boost::mutex> lock(m_mutex);
	m_pVariantObject = pVariantObject;
	return S_OK;
}

STDMETHODIMP CFollowService::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	boost::lock_guard<boost::mutex> lock(m_mutex);
	m_pSettings = pSettings;
	return S_OK;
}