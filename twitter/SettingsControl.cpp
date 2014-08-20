// SettingsControl.cpp : Implementation of CSettingsControl

#include "stdafx.h"
#include "SettingsControl.h"
#include "Plugins.h"
#include "..\NotificationServices\Plugins.h"

// CSettingsControl

STDMETHODIMP CSettingsControl::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_AUTH_THREAD, &m_pThreadService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));

	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_InfoControlService, &m_pInfoControlService));

	return S_OK;
}

STDMETHODIMP CSettingsControl::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	m_pThreadService.Release();
	m_pServiceProvider.Release();
	return S_OK;
}

STDMETHODIMP CSettingsControl::GetText(BSTR* pbstr)
{
	*pbstr = CComBSTR(L"Settings").Detach();
	return S_OK;
}

STDMETHODIMP CSettingsControl::GetHWND(HWND *hWnd)
{
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CSettingsControl::CreateEx(HWND hWndParent, HWND *hWnd)
{
	*hWnd = Create(hWndParent);
	return S_OK;
}

STDMETHODIMP CSettingsControl::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult, BOOL *bResult)
{
	LRESULT lResult = 0;
	*bResult = ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult);
	*plResult = lResult;
	return S_OK;
}

LRESULT CSettingsControl::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CAxDialogImpl<CSettingsControl>::OnInitDialog(uMsg, wParam, lParam, bHandled);
	DlgResize_Init(false);
	m_editUser = GetDlgItem(IDC_EDITUSER);
	m_editPass = GetDlgItem(IDC_EDITPASSWORD);
	return 0;
}

STDMETHODIMP CSettingsControl::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
	return E_NOTIMPL;
}

LRESULT CSettingsControl::OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_pThreadService->Run();
	return 0;
}

STDMETHODIMP CSettingsControl::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	m_pSettings = pSettings;
	CComPtr<ISettings> pSettingsTwitter;
	RETURN_IF_FAILED(pSettings->OpenSubSettings(SETTINGS_PATH, &pSettingsTwitter));
	RETURN_IF_FAILED(LoadEditBoxText(IDC_EDITUSER, KEY_USER, pSettingsTwitter));
	return S_OK;
}

STDMETHODIMP CSettingsControl::Save(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);

	RETURN_IF_FAILED(SaveEditBoxText(IDC_EDITUSER, KEY_USER, pSettings));

	return S_OK;
}

STDMETHODIMP CSettingsControl::Reset(ISettings *pSettings)
{
	return E_NOTIMPL;
}

HRESULT CSettingsControl::SaveEditBoxText(int id, BSTR bstrKey, ISettings* pSettings)
{
	CEdit wndTextBox = GetDlgItem(id);
	CComBSTR bstr;
	wndTextBox.GetWindowText(&bstr);

	if (bstr == NULL)
		bstr = "";
	RETURN_IF_FAILED(pSettings->SetVariantValue(bstrKey, &CComVariant(bstr)));
	return S_OK;
}

HRESULT CSettingsControl::LoadEditBoxText(int id, BSTR bstrKey, ISettings* pSettings)
{
	CHECK_E_POINTER(pSettings);
	CComVariant vValue;
	CComBSTR bstr;
	if (SUCCEEDED(pSettings->GetVariantValue(bstrKey, &vValue) && vValue.vt == VT_BSTR))
		bstr = vValue.bstrVal;

	CEdit wndServerTextBox = GetDlgItem(id);
	wndServerTextBox.SetWindowText(bstr);
	return S_OK;
}

STDMETHODIMP CSettingsControl::OnStart(IVariantObject *pResult)
{
	RETURN_IF_FAILED(m_pInfoControlService->ShowControl(m_hWnd, L"Authenticating...", FALSE, FALSE));
	return S_OK;
}

STDMETHODIMP CSettingsControl::OnRun(IVariantObject *pResult)
{
	CoInitialize(NULL);

	CComPtr<IThreadService> pTimelineThreadService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &pTimelineThreadService));
	RETURN_IF_FAILED(pTimelineThreadService->Join());

	CComPtr<IViewControllerService> pTimelineService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ViewControllerService, &pTimelineService));
	RETURN_IF_FAILED(pTimelineService->StopTimers());

	CComBSTR bstrUser;
	m_editUser.GetWindowText(&bstrUser);

	CComBSTR bstrPass;
	m_editPass.GetWindowText(&bstrPass);

	CComPtr<ITwitterConnection> pConnection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TwitterConnection, &pConnection));

	CComBSTR bstrKey, bstrSecret;
	RETURN_IF_FAILED(pConnection->GetAuthKeys(bstrUser, bstrPass, &bstrKey, &bstrSecret));

	RETURN_IF_FAILED(pResult->SetVariantValue(KEY_TWITTERKEY, &CComVariant(bstrKey)));
	RETURN_IF_FAILED(pResult->SetVariantValue(KEY_TWITTERSECRET, &CComVariant(bstrSecret)));

	return S_OK;
}

STDMETHODIMP CSettingsControl::OnFinish(IVariantObject *pResult)
{
	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(KEY_HRESULT, &vHr));
	if (SUCCEEDED(vHr.intVal))
	{
		RETURN_IF_FAILED(m_pInfoControlService->HideControl(m_hWnd));
	}
	else
	{
		CComVariant vDesc;
		RETURN_IF_FAILED(pResult->GetVariantValue(VAR_HRESULT_DESCRIPTION, &vDesc));
		RETURN_IF_FAILED(m_pInfoControlService->ShowControl(m_hWnd, vDesc.bstrVal, TRUE, FALSE));
		return S_OK;
	}

	CComVariant vKey, vSecret;
	RETURN_IF_FAILED(pResult->GetVariantValue(KEY_TWITTERKEY, &vKey));
	RETURN_IF_FAILED(pResult->GetVariantValue(KEY_TWITTERSECRET, &vSecret));

	CComPtr<ISettings> pSettingsTwitter;
	RETURN_IF_FAILED(m_pSettings->OpenSubSettings(SETTINGS_PATH, &pSettingsTwitter));

	RETURN_IF_FAILED(pSettingsTwitter->SetVariantValue(KEY_TWITTERKEY, &vKey));
	RETURN_IF_FAILED(pSettingsTwitter->SetVariantValue(KEY_TWITTERSECRET, &vSecret));

	m_editPass.SetWindowText(L"");
	Save(pSettingsTwitter);

	CComPtr<IViewControllerService> pTimelineService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ViewControllerService, &pTimelineService));
	RETURN_IF_FAILED(pTimelineService->StartTimers());

	CComPtr<IFormManager> pFormManager;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_FORM_MANAGER, &pFormManager));
	RETURN_IF_FAILED(pFormManager->ActivateForm(CLSID_TimelineControl));
	return S_OK;
}
