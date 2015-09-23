#include "stdafx.h"
#include "GetUserService.h"

#include "stdafx.h"
#include "FollowService.h"

STDMETHODIMP CGetUserService::OnInitialized(IServiceProvider* pServiceProvider)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	m_pServiceProvider = pServiceProvider;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_GETUSER_THREAD, &m_pThreadService));

	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));

	return S_OK;
}

STDMETHODIMP CGetUserService::OnShutdown()
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

STDMETHODIMP CGetUserService::OnRun(IVariantObject *pResult)
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

	CComPtr<ITwitterConnection> pConnection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TwitterConnection, &pConnection));
	RETURN_IF_FAILED(pConnection->OpenConnectionWithAppAuth());

	CComVar vUserName;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Name, &vUserName));
	ATLASSERT(vUserName.vt == VT_BSTR);

	CComPtr<IVariantObject> pUserObject;
	RETURN_IF_FAILED(pConnection->GetUser(vUserName.bstrVal, &pUserObject));

	RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Connection::Metadata::TweetObject::UserObject, &CComVar(pUserObject)));

	return S_OK;
}

STDMETHODIMP CGetUserService::SetVariantObject(IVariantObject *pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	boost::lock_guard<boost::mutex> lock(m_mutex);
	m_pVariantObject = pVariantObject;
	return S_OK;
}

STDMETHODIMP CGetUserService::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	boost::lock_guard<boost::mutex> lock(m_mutex);
	m_pSettings = pSettings;
	return S_OK;
}
