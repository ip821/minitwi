// SettingsControl.cpp : Implementation of CSettingsControl

#include "stdafx.h"
#include "SettingsControl.h"
#include "Plugins.h"
#include "..\model-libs\asyncsvc\Plugins.h"
#include "UpdateService.h"

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
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_FORM_MANAGER, &m_pFormManager));

	CComPtr<IControl> pControl;
	RETURN_IF_FAILED(m_pFormManager->FindForm(CLSID_TimelineControl, &pControl));
	m_pTimelineControl = pControl;

	CComPtr<ISettings> pSettingsTwitter;
	RETURN_IF_FAILED(m_pSettings->OpenSubSettings(SETTINGS_PATH, &pSettingsTwitter));
	CComVariant vKey;
	CComVariant vSecret;
	
	if (SUCCEEDED(pSettingsTwitter->GetVariantValue(KEY_TWITTERKEY, &vKey)) &&
		SUCCEEDED(pSettingsTwitter->GetVariantValue(KEY_TWITTERSECRET, &vSecret)) &&
		vKey.vt == VT_BSTR &&
		vSecret.vt == VT_BSTR)
	{
		SwitchToLogoutMode();
	}
	else
	{
		SwitchToLoginMode();
	}

	return S_OK;
}

STDMETHODIMP CSettingsControl::OnShutdown()
{
	RETURN_IF_FAILED(m_pThreadService->Join());
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	m_pThreadService.Release();
	m_pServiceProvider.Release();
	m_pTimelineControl.Release();
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
	m_labelLoggedUser = GetDlgItem(IDC_LABEL_LOGGED_USER);
	m_labelVersion = GetDlgItem(IDC_LABEL_VERSION);
	m_labelHomePage.SubclassWindow(GetDlgItem(IDC_LABEL_HOMEPAGE));

	m_labelVersion.SetWindowText(CUpdateService::GetInstalledVersion());

	return 0;
}

STDMETHODIMP CSettingsControl::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
	return E_NOTIMPL;
}

void CSettingsControl::SwitchToLoginMode()
{
	::ShowWindow(GetDlgItem(IDC_LABEL_LOGGEDIN), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_LABEL_LOGGED_USER), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_BUTTON_LOGOUT), SW_HIDE);

	::ShowWindow(GetDlgItem(IDC_BUTTON_LOGIN), SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_LABEL_LOGIN), SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_LABEL_PASS), SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_EDITUSER), SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_EDITPASSWORD), SW_SHOW);
}

void CSettingsControl::SwitchToLogoutMode()
{
	if (m_editUser.IsWindow() && m_editUser.IsWindow())
	{
		CString strText;
		m_editUser.GetWindowText(strText);
		m_labelLoggedUser.SetWindowText(strText);
	}

	::ShowWindow(GetDlgItem(IDC_LABEL_LOGGEDIN), SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_LABEL_LOGGED_USER), SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_BUTTON_LOGOUT), SW_SHOW);

	::ShowWindow(GetDlgItem(IDC_BUTTON_LOGIN), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_LABEL_LOGIN), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_LABEL_PASS), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_EDITUSER), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_EDITPASSWORD), SW_HIDE);
}

LRESULT CSettingsControl::OnClickedLogout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_pSettings->RemoveSubSettings(SETTINGS_PATH);
	m_pTimelineControl->Clear();
	SwitchToLoginMode();
	return 0;
}

LRESULT CSettingsControl::OnClickedLogin(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
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
		RETURN_IF_FAILED(pResult->GetVariantValue(KEY_HRESULT_DESCRIPTION, &vDesc));
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
	SwitchToLogoutMode();

	CComPtr<IViewControllerService> pTimelineService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ViewControllerService, &pTimelineService));
	RETURN_IF_FAILED(pTimelineService->StartTimers());
	RETURN_IF_FAILED(m_pFormManager->ActivateForm(CLSID_TimelineControl));
	return S_OK;
}

bool CSettingsControl::DlgResize_PositionControl(int in_nWidth, int in_nHeight, RECT& in_sGroupRect, _AtlDlgResizeData&  in_sData, bool in_bGroup, _AtlDlgResizeData* in_pDataPrev)
{
	bool l_bSuccess = true;

	if (!in_bGroup)
	{
		return __super::DlgResize_PositionControl(in_nWidth, in_nHeight, in_sGroupRect, in_sData, in_bGroup, in_pDataPrev);
	}

	// Call DlgResize, but without any CENTER stuff as if not a group
	{
		DWORD l_nOldFlags = in_sData.m_dwResizeFlags;

		in_sData.m_dwResizeFlags &= ~(DLSZ_CENTER_X | DLSZ_CENTER_Y);

		l_bSuccess = __super::DlgResize_PositionControl
			(
			in_nWidth,
			in_nHeight,
			in_sGroupRect,
			in_sData,
			false,      // Means group is ignored
			in_pDataPrev
			);

		in_sData.m_dwResizeFlags = l_nOldFlags;
	}

	// Now apply center to any group
	if (in_bGroup && (in_sData.m_dwResizeFlags & (DLSZ_CENTER_X | DLSZ_CENTER_Y)))
	{
		// Find start of group - so we know which item we are in the group
		int l_nGroupStart = -1;
		for (int l_nPos = m_arrData.Find(in_sData); l_nPos > 0; l_nPos--)
		{
			if (0 != (m_arrData[l_nPos].m_dwResizeFlags & _DLSZ_BEGIN_GROUP))
			{
				l_nGroupStart = l_nPos;
				break;
			}
		}

		ATLASSERT(-1 != l_nGroupStart);

		int l_nItem = m_arrData.Find(in_sData) - l_nGroupStart;
		int l_nItems = m_arrData[l_nGroupStart].GetGroupCount();

		// Get current details of control
		CRect   l_cArea;
		CWindow l_cCtrl;

		l_cCtrl.Attach(GetDlgItem(in_sData.m_nCtlID));

		l_cCtrl.GetWindowRect(l_cArea);
		ScreenToClient(l_cArea);

		int groupStart = l_nGroupStart;

		if (in_sData.m_dwResizeFlags & DLSZ_CENTER_X)
		{
			int offsetX = 0;
			for (int i = groupStart; i < groupStart + l_nItems; i++)
			{
				if (i == m_arrData.Find(in_sData))
					break;
				auto& gItem = m_arrData[i];
				offsetX += (gItem.m_rect.right - gItem.m_rect.left);
			}

			int l_nGroupWidth = in_sGroupRect.right - in_sGroupRect.left;
			int l_nGroupStart = (in_nWidth / 2) - (l_nGroupWidth / 2);

			int l_nSegment = l_nGroupWidth / l_nItems;
			l_cArea.MoveToX(l_nGroupStart + offsetX/*(l_nSegment * l_nItem)*/);
		}

		if (in_sData.m_dwResizeFlags & DLSZ_CENTER_Y)
		{
			int l_nGroupHeight = in_sGroupRect.bottom - in_sGroupRect.top;
			int l_nGroupStart = (in_nHeight / 2) - (l_nGroupHeight / 2);

			int l_nSegment = l_nGroupHeight / l_nItems;
			l_cArea.MoveToY(l_nGroupStart + (l_nSegment * l_nItem));
		}

		// Honour repaint
		if ((in_sData.m_dwResizeFlags & DLSZ_REPAINT) != 0)
			l_cCtrl.Invalidate();

		// Move control
		l_cCtrl.SetWindowPos(NULL, &l_cArea, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);

		l_cCtrl.Detach();
	}

	return l_bSuccess;
}