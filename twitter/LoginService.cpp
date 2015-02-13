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
	CComPtr<IHomeTimelineControllerService> pHomeTimelineControllerService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_HomeTimelineControllerService, &pHomeTimelineControllerService));
	RETURN_IF_FAILED(pHomeTimelineControllerService->StopConnection());
	RETURN_IF_FAILED(m_pViewControllerService->HideInfo());
	RETURN_IF_FAILED(m_pViewControllerService->StartAnimation());
	return S_OK;
}

STDMETHODIMP CLoginService::OnRun(IVariantObject *pResult)
{
	CComPtr<IFormManager> pFormManager;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_FORM_MANAGER, &pFormManager));
	CComPtr<IControl> pTimelineControl;
	RETURN_IF_FAILED(pFormManager->FindForm(CLSID_HomeTimeLineControl, &pTimelineControl));
	CComQIPtr<IServiceProviderSupport> pServiceProviderSupport = pTimelineControl;
	CComPtr<IServiceProvider> pServiceProvider;
	RETURN_IF_FAILED(pServiceProviderSupport->GetServiceProvider(&pServiceProvider));
	CComPtr<IThreadService> pTimelineThreadService;
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &pTimelineThreadService));
	RETURN_IF_FAILED(pTimelineThreadService->Join());

	CComVariant vUser;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Settings::Twitter::User, &vUser));
	ATLASSERT(vUser.vt == VT_BSTR);
	CComVariant vPass;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Settings::Twitter::Password, &vPass));
	ATLASSERT(vPass.vt == VT_BSTR);

	CComPtr<ITwitterConnection> pConnection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TwitterConnection, &pConnection));

	CComBSTR bstrKey, bstrSecret;
	RETURN_IF_FAILED(pConnection->GetAuthKeys(vUser.bstrVal, vPass.bstrVal, &bstrKey, &bstrSecret));

	RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterKey, &CComVariant(bstrKey)));
	RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterSecret, &CComVariant(bstrSecret)));

	return S_OK;
}

STDMETHODIMP CLoginService::OnFinish(IVariantObject *pResult)
{
	RETURN_IF_FAILED(m_pViewControllerService->StopAnimation());
	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));
	if (FAILED(vHr.intVal))
	{
		return S_OK;
	}

	CComVariant vKey, vSecret, vUser;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterKey, &vKey));
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterSecret, &vSecret));
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Settings::Twitter::User, &vUser));

	CComPtr<ISettings> pSettingsTwitter;
	RETURN_IF_FAILED(m_pSettings->OpenSubSettings(Twitter::Metadata::Settings::PathRoot, &pSettingsTwitter));

	RETURN_IF_FAILED(pSettingsTwitter->SetVariantValue(Twitter::Metadata::Settings::Twitter::User, &vUser));
	RETURN_IF_FAILED(pSettingsTwitter->SetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterKey, &vKey));
	RETURN_IF_FAILED(pSettingsTwitter->SetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterSecret, &vSecret));

	CComPtr<IHomeTimelineControllerService> pHomeTimelineControllerService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_HomeTimelineControllerService, &pHomeTimelineControllerService));
	RETURN_IF_FAILED(pHomeTimelineControllerService->StartConnection());

	RETURN_IF_FAILED(m_pFormManager->ActivateForm(CLSID_HomeTimeLineControl));
	return S_OK;
}