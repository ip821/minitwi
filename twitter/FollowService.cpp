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

    CComPtr<ITwitterConnection> pConnection;
    RETURN_IF_FAILED(HrOpenConnection(pSettings, &pConnection));

	CComVar vUserName;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Name, &vUserName));
	ATLASSERT(vUserName.vt == VT_BSTR);

	CComVar vFollowing;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Item::VAR_IS_FOLLOWING, &vFollowing));

	bool bFollowing = vFollowing.vt == VT_BOOL && vFollowing.boolVal;

	if (bFollowing)
	{
		RETURN_IF_FAILED(pConnection->UnfollowUser(vUserName.bstrVal));
	}
	else
	{
		RETURN_IF_FAILED(pConnection->FollowUser(vUserName.bstrVal));
	}

	RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Item::VAR_IS_FOLLOWING, &CComVar(!bFollowing)));

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