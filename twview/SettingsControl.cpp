// SettingsControl.cpp : Implementation of CSettingsControl

#include "stdafx.h"
#include "SettingsControl.h"
#include "Plugins.h"
#include "..\model-libs\asyncsvc\Plugins.h"
#include "..\model-libs\objmdl\stringutils.h"
#include "..\model-libs\objmdl\stringutils.h"
#include "..\twiconn\Plugins.h"
#include "..\twitter\Plugins.h"

// CSettingsControl

STDMETHODIMP CSettingsControl::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_AUTH_THREAD, &m_pThreadService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));

	CComPtr<IFormManager> pFormManager;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_FORM_MANAGER, &pFormManager));

	m_pCustomTabControl = m_pControl;

	CComPtr<IControl> pControl;
	RETURN_IF_FAILED(pFormManager->FindForm(CLSID_HomeTimeLineControl, &pControl));
	CComQIPtr<ITimelineControlSupport> pTimelineControlSupport = pControl;
	ATLASSERT(pTimelineControlSupport);
	RETURN_IF_FAILED(pTimelineControlSupport->GetTimelineControl(&m_pTimelineControl));

	CComPtr<ISettings> pSettingsTwitter;
	RETURN_IF_FAILED(m_pSettings->OpenSubSettings(Twitter::Metadata::Settings::PathRoot, &pSettingsTwitter));
	CComVariant vKey;
	CComVariant vSecret;

	if (SUCCEEDED(pSettingsTwitter->GetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterKey, &vKey)) &&
		SUCCEEDED(pSettingsTwitter->GetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterSecret, &vSecret)) &&
		vKey.vt == VT_BSTR &&
		vSecret.vt == VT_BSTR)
	{
		SwitchToLogoutMode();
	}
	else
	{
		SwitchToLoginMode();
	}

	CComPtr<IUpdateService> pUpdateService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_UpdateService, &pUpdateService));
	CComBSTR bstrVersion;
	RETURN_IF_FAILED(pUpdateService->GetInstalledVersion(&bstrVersion));
	m_labelVersion.SetWindowText(bstrVersion);

	return S_OK;
}

STDMETHODIMP CSettingsControl::OnShutdown()
{
	RETURN_IF_FAILED(m_pSkinCommonControl->UnregisterButtonControl(m_buttonLogin.m_hWnd));
	RETURN_IF_FAILED(m_pSkinCommonControl->UnregisterButtonControl(m_buttonLogout.m_hWnd));
	RETURN_IF_FAILED(m_pSkinCommonControl->UnregisterStaticControl(m_hWnd));
	RETURN_IF_FAILED(m_pThreadService->Join());
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));

	m_pSkinCommonControl.Release();
	m_pThreadService.Release();
	m_pServiceProvider.Release();
	m_pTimelineControl.Release();
	m_pTheme.Release();
	m_pSettings.Release();
	m_pCustomTabControl.Release();
	DestroyWindow();
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
	m_editPin = GetDlgItem(IDC_EDIT_PIN);
	m_labelLoggedUser = GetDlgItem(IDC_LABEL_LOGGED_USER);
	m_labelVersion = GetDlgItem(IDC_LABEL_VERSION);
	m_buttonLogin = GetDlgItem(IDC_BUTTON_LOGIN);
	m_buttonLogout = GetDlgItem(IDC_BUTTON_LOGOUT);
	m_labelHomePage.SubclassWindow(GetDlgItem(IDC_LABEL_HOMEPAGE));

	return 0;
}

STDMETHODIMP CSettingsControl::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
	return S_OK;
}

void CSettingsControl::SwitchToLoginMode()
{
	CComPtr<IFormManager> pFormManager;
	ASSERT_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_FORM_MANAGER, &pFormManager));
	CComPtr<IControl> pControl;
	ASSERT_IF_FAILED(pFormManager->FindForm(CLSID_HomeTimeLineControl, &pControl));
	CComQIPtr<IServiceProviderSupport> pServiceProviderSupport = pControl;
	CComPtr<IServiceProvider> pServiceProvider;
	ASSERT_IF_FAILED(pServiceProviderSupport->GetServiceProvider(&pServiceProvider));
	CComPtr<IHomeTimelineControllerService> pHomeTimelineControllerService;
	ASSERT_IF_FAILED(pServiceProvider->QueryService(CLSID_HomeTimelineControllerService, &pHomeTimelineControllerService));
	ASSERT_IF_FAILED(pHomeTimelineControllerService->StopConnection());

	::ShowWindow(GetDlgItem(IDC_LABEL_LOGGED_USER), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_BUTTON_LOGOUT), SW_HIDE);
	
	::ShowWindow(GetDlgItem(IDC_LABEL_PIN), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_EDIT_PIN), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_BUTTON_ENTER_PIN), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_BUTTON_CANCEL_PIN), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_LABEL_PIN_INFO), SW_HIDE);

	::ShowWindow(GetDlgItem(IDC_BUTTON_LOGIN), SW_SHOW);
}

void CSettingsControl::SwitchToPinMode()
{
	m_editPin.SetWindowText(L"");
	::ShowWindow(GetDlgItem(IDC_LABEL_LOGGED_USER), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_BUTTON_LOGOUT), SW_HIDE);

	::ShowWindow(GetDlgItem(IDC_LABEL_PIN), SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_EDIT_PIN), SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_BUTTON_ENTER_PIN), SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_BUTTON_CANCEL_PIN), SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_LABEL_PIN_INFO), SW_SHOW);

	::ShowWindow(GetDlgItem(IDC_BUTTON_LOGIN), SW_HIDE);
}

void CSettingsControl::SwitchToLogoutMode()
{
	::ShowWindow(GetDlgItem(IDC_LABEL_LOGGED_USER), SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_BUTTON_LOGOUT), SW_SHOW);

	::ShowWindow(GetDlgItem(IDC_LABEL_PIN), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_EDIT_PIN), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_BUTTON_ENTER_PIN), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_BUTTON_CANCEL_PIN), SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_LABEL_PIN_INFO), SW_HIDE);

	::ShowWindow(GetDlgItem(IDC_BUTTON_LOGIN), SW_HIDE);

	CComVariant vUser;
	ASSERT_IF_FAILED(m_pSettings->GetVariantValue(Twitter::Metadata::Settings::Twitter::User, &vUser));
	ATLASSERT(vUser.vt == VT_BSTR);
	m_labelLoggedUser.SetWindowText(vUser.bstrVal);
}

LRESULT CSettingsControl::OnClickedEnterPin(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_pThreadService->Run();
	return 0;
}

LRESULT CSettingsControl::OnClickedCancelPin(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_accessUrl.Empty();
	m_authKey.Empty();
	m_authSecret.Empty();
	SwitchToLoginMode();
	return 0;
}

LRESULT CSettingsControl::OnClickedLogout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_pSettings->RemoveSubSettings(Twitter::Metadata::Settings::PathRoot);
	TCHAR lpszFileName[MAX_PATH] = { 0 };
	GetModuleFileName(nullptr, &lpszFileName[0], MAX_PATH);
	ShellExecute(NULL, L"open", lpszFileName, NULL, NULL, SW_SHOW);
	exit(0);
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
	RETURN_IF_FAILED(pSettings->OpenSubSettings(Twitter::Metadata::Settings::PathRoot, &pSettingsTwitter));
	return S_OK;
}

STDMETHODIMP CSettingsControl::Save(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	return S_OK;
}

STDMETHODIMP CSettingsControl::Reset(ISettings *pSettings)
{
	return E_NOTIMPL;
}

STDMETHODIMP CSettingsControl::OnStart(IVariantObject *pResult)
{
	EnableLoginControls(FALSE);

	CComBSTR bstrPin;
	m_editPin.GetWindowText(&bstrPin);

	if (!m_accessUrl.IsEmpty())
	{
		RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterAccessUrl, &CComVariant(m_accessUrl)));
		RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterAuthKey, &CComVariant(m_authKey)));
		RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterAuthSecret, &CComVariant(m_authSecret)));
		RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterAccessPin, &CComVariant(bstrPin)));
	}

	return S_OK;
}

STDMETHODIMP CSettingsControl::OnFinish(IVariantObject *pResult)
{
	EnableLoginControls(TRUE);

	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));
	if (FAILED(vHr.intVal))
	{
		CComVariant vDesc;
		RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResultDescription, &vDesc));
		RETURN_IF_FAILED(m_pCustomTabControl->ShowInfo(TRUE, FALSE, vDesc.bstrVal));
		MessageBox(vDesc.bstrVal, L"Error", MB_OK | MB_ICONERROR);
		return S_OK;
	}

	CComVariant vKey;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterKey, &vKey));
	if (vKey.vt == VT_EMPTY)
	{
		CComVariant vAccessUrl;
		RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterAccessUrl, &vAccessUrl));
		m_accessUrl = vAccessUrl.bstrVal;

		CComVariant vAuthKey;
		RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterAuthKey, &vAuthKey));
		m_authKey = vAuthKey.bstrVal;

		CComVariant vAuthSecret;
		RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Settings::Twitter::TwitterAuthSecret, &vAuthSecret));
		m_authSecret = vAuthSecret.bstrVal;

		int res = (int)ShellExecute(NULL, L"open", vAccessUrl.bstrVal, NULL, NULL, SW_SHOWNORMAL);
		StrCopyToClipboard(vAccessUrl.bstrVal);
		if (res <= 32)
		{
			auto errCode = res;
			auto msg = _com_error(HRESULT_FROM_WIN32(errCode)).ErrorMessage();
			MessageBox(msg, L"Error");
		}
		else
		{
			SwitchToPinMode();
		}
	}
	else
	{
		SwitchToLogoutMode();
	}

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
			int nGroupStart = (in_nWidth / 2) - (l_nGroupWidth / 2);

			l_cArea.MoveToX(nGroupStart + offsetX/*(l_nSegment * l_nItem)*/);
		}

		if (in_sData.m_dwResizeFlags & DLSZ_CENTER_Y)
		{
			int l_nGroupHeight = in_sGroupRect.bottom - in_sGroupRect.top;
			int nGroupStart = (in_nHeight / 2) - (l_nGroupHeight / 2);

			int l_nSegment = l_nGroupHeight / l_nItems;
			l_cArea.MoveToY(nGroupStart + (l_nSegment * l_nItem));
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

STDMETHODIMP CSettingsControl::SetTheme(ITheme* pTheme)
{
	CHECK_E_POINTER(pTheme);
	m_pTheme = pTheme;
	RETURN_IF_FAILED(m_pTheme->GetCommonControlSkin(&m_pSkinCommonControl));
	RETURN_IF_FAILED(m_pSkinCommonControl->RegisterStaticControl(m_hWnd));
	RETURN_IF_FAILED(m_pSkinCommonControl->RegisterButtonControl(m_buttonLogin.m_hWnd));
	RETURN_IF_FAILED(m_pSkinCommonControl->RegisterButtonControl(m_buttonLogout.m_hWnd));
	return S_OK;
}

void CSettingsControl::EnableLoginControls(BOOL bEnale)
{
	::EnableWindow(GetDlgItem(IDC_BUTTON_LOGIN), bEnale);
	::EnableWindow(GetDlgItem(IDC_EDIT_PIN), bEnale);
	::EnableWindow(GetDlgItem(IDC_BUTTON_ENTER_PIN), bEnale);
	::EnableWindow(GetDlgItem(IDC_BUTTON_CANCEL_PIN), bEnale);
}