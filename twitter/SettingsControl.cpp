// SettingsControl.cpp : Implementation of CSettingsControl

#include "stdafx.h"
#include "SettingsControl.h"
#include "Plugins.h"
#include "twitconn_contract_i.h"

// CSettingsControl

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
	return 0;
}

STDMETHODIMP CSettingsControl::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
	return E_NOTIMPL;
}

LRESULT CSettingsControl::OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	Save(m_pSettings);
	return 0;
}

STDMETHODIMP CSettingsControl::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	m_pSettings = pSettings;
	CComPtr<ISettings> pSettingsTwitter;
	RETURN_IF_FAILED(pSettings->OpenSubSettings(SETTINGS_PATH, &pSettingsTwitter));
	RETURN_IF_FAILED(LoadEditBoxText(IDC_EDITUSER, KEY_USER, pSettingsTwitter));
	RETURN_IF_FAILED(LoadEditBoxText(IDC_EDITPASSWORD, KEY_PASSWORD, pSettingsTwitter));
	return S_OK;
}

STDMETHODIMP CSettingsControl::Save(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);

	CComPtr<ISettings> pSettingsTwitter;
	RETURN_IF_FAILED(pSettings->OpenSubSettings(SETTINGS_PATH, &pSettingsTwitter));

	RETURN_IF_FAILED(SaveEditBoxText(IDC_EDITUSER, KEY_USER, pSettingsTwitter));
	RETURN_IF_FAILED(SaveEditBoxText(IDC_EDITPASSWORD, KEY_PASSWORD, pSettingsTwitter));

	CEdit wnd1 = GetDlgItem(IDC_EDITUSER);
	CComBSTR bstrUser;
	wnd1.GetWindowText(&bstrUser);

	CEdit wnd2 = GetDlgItem(IDC_EDITPASSWORD);
	CComBSTR bstrPass;
	wnd2.GetWindowText(&bstrPass);

	CComPtr<ITwitterConnection> pTwitterConnection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TwitterConnection, &pTwitterConnection));
	CComBSTR bstrKey, bstrSecret;
	RETURN_IF_FAILED(pTwitterConnection->GetAuthKeys(bstrUser, bstrPass, &bstrKey, &bstrSecret));

	RETURN_IF_FAILED(pSettingsTwitter->SetVariantValue(KEY_TWITTERKEY, &CComVariant(bstrKey)));
	RETURN_IF_FAILED(pSettingsTwitter->SetVariantValue(KEY_TWITTERSECRET, &CComVariant(bstrSecret)));

	pTwitterConnection->OpenConnection(bstrKey, bstrSecret);

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
