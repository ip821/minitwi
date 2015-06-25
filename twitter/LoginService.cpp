#include "stdafx.h"
#include "LoginService.h"

STDMETHODIMP CLoginService::OnInitialized(IServiceProvider *pServiceProvider)
{
	m_pServiceProvider = pServiceProvider;

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_AUTH_THREAD, &m_pThreadService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_FORM_MANAGER, &m_pFormManager));
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ViewControllerService, &m_pViewControllerService));

	return S_OK;
}

STDMETHODIMP CLoginService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());
	m_pSettings.Release();
	m_pFormManager.Release();
	m_pViewControllerService.Release();
	m_pServiceProvider.Release();
	return S_OK;
}

STDMETHODIMP CLoginService::Load(ISettings* pSettings)
{
	m_pSettings = pSettings;
	return S_OK;
}

STDMETHODIMP CLoginService::OnStart(IVariantObject *pResult)
{
	CComPtr<IControl> pControl;
	RETURN_IF_FAILED(m_pFormManager->FindForm(CLSID_HomeTimeLineControl, &pControl));
	CComQIPtr<IServiceProviderSupport> pServiceProviderSupport = pControl;
	CComPtr<IServiceProvider> pServiceProvider;
	RETURN_IF_FAILED(pServiceProviderSupport->GetServiceProvider(&pServiceProvider));
	CComPtr<IHomeTimelineControllerService> pHomeTimelineControllerService;
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_HomeTimelineControllerService, &pHomeTimelineControllerService));
	RETURN_IF_FAILED(pHomeTimelineControllerService->StopConnection());

	RETURN_IF_FAILED(m_pViewControllerService->HideInfo());
	RETURN_IF_FAILED(m_pViewControllerService->StartAnimation());
	return S_OK;
}

STDMETHODIMP CLoginService::OnRun(IVariantObject *pResult)
{
	CComPtr<IControl> pTimelineControl;
	RETURN_IF_FAILED(m_pFormManager->FindForm(CLSID_HomeTimeLineControl, &pTimelineControl));
	CComQIPtr<IServiceProviderSupport> pServiceProviderSupport = pTimelineControl;
	CComPtr<IServiceProvider> pServiceProvider;
	RETURN_IF_FAILED(pServiceProviderSupport->GetServiceProvider(&pServiceProvider));
	CComPtr<IThreadService> pTimelineThreadService;
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &pTimelineThreadService));
	RETURN_IF_FAILED(pTimelineThreadService->Join());

	CComVariant vAccessPin;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterAccessPin, &vAccessPin));

	CComPtr<ITwitterConnection> pConnection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TwitterConnection, &pConnection));

	CComVariant vAccessUrl;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterAccessUrl, &vAccessUrl));

	if (vAccessUrl.vt == VT_EMPTY)
	{
		CComBSTR bstrAccessUrl;
		CComBSTR bstrAuthKey;
		CComBSTR bstrAuthSecret;
		RETURN_IF_FAILED(pConnection->GetAccessUrl(&bstrAuthKey, &bstrAuthSecret, &bstrAccessUrl));
		vAccessUrl = bstrAccessUrl;
		RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterAccessUrl, &vAccessUrl));
		RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterAuthKey, &CComVariant(bstrAuthKey)));
		RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterAuthSecret, &CComVariant(bstrAuthSecret)));
	}
	else
	{
		CComVariant vAuthKey;
		RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterAuthKey, &vAuthKey));
		CComVariant vAuthSecret;
		RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterAuthSecret, &vAuthSecret));

		ATLASSERT(vAccessPin.vt == VT_BSTR && vAuthKey.vt == VT_BSTR && vAuthSecret.vt == VT_BSTR);
		CComBSTR bstrUser, bstrKey, bstrSecret;
		RETURN_IF_FAILED(pConnection->GetAccessTokens(vAuthKey.bstrVal, vAuthSecret.bstrVal, vAccessPin.bstrVal, &bstrUser, &bstrKey, &bstrSecret));
		RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Settings::Twitter::User, &CComVariant(bstrUser)));
		RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterKey, &CComVariant(bstrKey)));
		RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterSecret, &CComVariant(bstrSecret)));
	}

	return S_OK;
}

STDMETHODIMP CLoginService::OnFinish(IVariantObject *pResult)
{
	RETURN_IF_FAILED(m_pViewControllerService->StopAnimation());
	CComVar vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));
	if (FAILED(vHr.intVal))
	{
		return S_OK;
	}

	CComVar vKey, vSecret, vUser;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterKey, &vKey));
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterSecret, &vSecret));
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Settings::Twitter::User, &vUser));

	CComPtr<ISettings> pSettingsTwitter;
	RETURN_IF_FAILED(m_pSettings->OpenSubSettings(Twitter::Metadata::Settings::PathRoot, &pSettingsTwitter));

	if (vKey.vt != VT_EMPTY)
	{
		RETURN_IF_FAILED(pSettingsTwitter->SetVariantValue(Twitter::Metadata::Settings::Twitter::User, &vUser));
		RETURN_IF_FAILED(pSettingsTwitter->SetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterKey, &vKey));
		RETURN_IF_FAILED(pSettingsTwitter->SetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterSecret, &vSecret));

		CComPtr<IControl> pControl;
		RETURN_IF_FAILED(m_pFormManager->FindForm(CLSID_HomeTimeLineControl, &pControl));
		CComQIPtr<IServiceProviderSupport> pServiceProviderSupport = pControl;
		CComPtr<IServiceProvider> pServiceProvider;
		RETURN_IF_FAILED(pServiceProviderSupport->GetServiceProvider(&pServiceProvider));
		CComPtr<IHomeTimelineControllerService> pHomeTimelineControllerService;
		RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_HomeTimelineControllerService, &pHomeTimelineControllerService));
		RETURN_IF_FAILED(pHomeTimelineControllerService->StartConnection());
		RETURN_IF_FAILED(m_pFormManager->ActivateForm(CLSID_HomeTimeLineControl));
	}

	return S_OK;
}