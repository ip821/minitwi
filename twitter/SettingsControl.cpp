// SettingsControl.cpp : Implementation of CSettingsControl

#include "stdafx.h"
#include "SettingsControl.h"
#include "Plugins.h"

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
	RETURN_IF_FAILED(LoadEditBoxText(IDC_EDITUSER, KEY_USER, pSettings));
	RETURN_IF_FAILED(LoadEditBoxText(IDC_EDITPASSWORD, KEY_PASSWORD, pSettings));
	return S_OK;
}

STDMETHODIMP CSettingsControl::Save(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	RETURN_IF_FAILED(SaveEditBoxText(IDC_EDITUSER, KEY_USER, pSettings));
	RETURN_IF_FAILED(SaveEditBoxText(IDC_EDITPASSWORD, KEY_PASSWORD, pSettings));
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
