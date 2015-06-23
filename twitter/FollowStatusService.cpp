#include "stdafx.h"
#include "FollowStatusService.h"

STDMETHODIMP CFollowStatusService::OnInitialized(IServiceProvider* pServiceProvider)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	m_pServiceProvider = pServiceProvider;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_FOLLOW_STATUS_THREAD, &m_pThreadService));

	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));

	return S_OK;
}

STDMETHODIMP CFollowStatusService::OnShutdown()
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

STDMETHODIMP CFollowStatusService::OnRun(IVariantObject *pResult)
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

	CComVar vUserNameTarget;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Name, &vUserNameTarget));
	ATLASSERT(vUserNameTarget.vt == VT_BSTR);

	BOOL bFollowing = FALSE;
	RETURN_IF_FAILED(pConnection->GetFollowStatus(vUserNameTarget.bstrVal, &bFollowing));
	RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Item::VAR_IS_FOLLOWING, &CComVar(bFollowing != 0)));

	return S_OK;
}

STDMETHODIMP CFollowStatusService::SetVariantObject(IVariantObject *pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	boost::lock_guard<boost::mutex> lock(m_mutex);
	m_pVariantObject = pVariantObject;
	return S_OK;
}

STDMETHODIMP CFollowStatusService::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	boost::lock_guard<boost::mutex> lock(m_mutex);
	m_pSettings = pSettings;
	return S_OK;
}