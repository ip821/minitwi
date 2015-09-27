#include "stdafx.h"
#include "SearchControl.h"
#include "..\twiconn\Plugins.h"
#include "Plugins.h"

#define OFFSET_Y 30

STDMETHODIMP CSearchControl::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(HrInitializeWithSettings(m_pTimelineControl, m_pSettings));
	RETURN_IF_FAILED(HrInitializeWithControl(m_pTimelineControl, pUnk));
	RETURN_IF_FAILED(HrNotifyOnInitialized(m_pTimelineControl, m_pServiceProvider));

	if (!m_pThreadService)
	{
		CComQIPtr<IServiceProviderSupport> pServiceProviderSupport = m_pTimelineControl;
		ATLASSERT(pServiceProviderSupport);
		CComPtr<IServiceProvider> pServiceProviderTimelineControl;
		RETURN_IF_FAILED(pServiceProviderSupport->GetServiceProvider(&pServiceProviderTimelineControl));
		RETURN_IF_FAILED(pServiceProviderTimelineControl->QueryService(SERVICE_TIMELINE_UPDATE_THREAD, &m_pThreadService));
	}

	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));

	return S_OK;
}

STDMETHODIMP CSearchControl::OnShutdown()
{
	RETURN_IF_FAILED(m_pSkinCommonControl->UnregisterStaticControl(m_hWnd));
	RETURN_IF_FAILED(m_pSkinCommonControl->UnregisterButtonControl(m_buttonGo));

	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	CComQIPtr<IPluginSupportNotifications> p = m_pTimelineControl;
	ATLASSERT(p);
	RETURN_IF_FAILED(p->OnShutdown());
	m_editText.Clear();

	m_pSkinCommonControl.Release();
	m_pSettings.Release();
	m_pThreadService.Release();
	m_pServiceProvider.Release();
	m_pTimelineControl.Release();
	m_pTheme.Release();
	m_pViewControllerService.Release();
	m_pVariantObject.Release();

	DestroyWindow();

	return S_OK;
}

STDMETHODIMP CSearchControl::OnActivate()
{
	m_bActive = TRUE;
	m_editText.SetFocus();
	return S_OK;
}

STDMETHODIMP CSearchControl::OnDeactivate()
{
	m_bActive = FALSE;
	return S_OK;
}

STDMETHODIMP CSearchControl::Clear()
{
	CComQIPtr<ITimelineControlSupport> pTimelineControlSupport = m_pTimelineControl;
	ATLASSERT(pTimelineControlSupport);
	CComPtr<ITimelineControl> pControl;
	RETURN_IF_FAILED(pTimelineControlSupport->GetTimelineControl(&pControl));
	RETURN_IF_FAILED(pControl->Clear());
	m_pVariantObject.Release();
	return S_OK;
}

STDMETHODIMP CSearchControl::GetHWND(HWND *hWnd)
{
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CSearchControl::CreateEx(HWND hWndParent, HWND *hWnd)
{
	*hWnd = Create(hWndParent);
	return S_OK;
}

STDMETHODIMP CSearchControl::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
	if (!m_pTimelineControl)
		return S_OK;

	if (
		(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		&&
		(pMsg->hwnd == m_editText || pMsg->hwnd == m_buttonGo)
		&&
		(::GetWindowLong(pMsg->hwnd, GWL_STYLE) & WS_DISABLED) == 0
		)
	{
		RETURN_IF_FAILED(DoSearch());
	}

	CComQIPtr<IControl> pControl = m_pTimelineControl;
	ATLASSERT(pControl);
	RETURN_IF_FAILED(pControl->PreTranslateMessage(pMsg, pbResult));
	return S_OK;
}

STDMETHODIMP CSearchControl::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	m_pSettings = pSettings;
	return S_OK;
}

STDMETHODIMP CSearchControl::GetText(BSTR* pbstr)
{
	*pbstr = CComBSTR(L"Search").Detach();
	return S_OK;
}

STDMETHODIMP CSearchControl::OnStart(IVariantObject *pResult)
{
	EnableControls(FALSE);
	return S_OK;
}

STDMETHODIMP CSearchControl::OnFinish(IVariantObject *pResult)
{
	EnableControls(TRUE);
	if (m_bActive)
		::SetFocus(m_hWndTimelineControl);
	return S_OK;
}

STDMETHODIMP CSearchControl::SetTheme(ITheme* pTheme)
{
	CHECK_E_POINTER(pTheme);
	m_pTheme = pTheme;

	RETURN_IF_FAILED(m_pTheme->GetCommonControlSkin(&m_pSkinCommonControl));
	RETURN_IF_FAILED(m_pSkinCommonControl->RegisterStaticControl(m_hWnd));
	RETURN_IF_FAILED(m_pSkinCommonControl->RegisterButtonControl(m_buttonGo));

	CComQIPtr<IThemeSupport> pThemeSupport = m_pTimelineControl;
	ATLASSERT(pThemeSupport);
	RETURN_IF_FAILED(pThemeSupport->SetTheme(m_pTheme));

	return S_OK;
}

STDMETHODIMP CSearchControl::DoSearch()
{
	CString strText;
	m_editText.GetWindowText(strText);
	if (!strText.IsEmpty())
	{
		CComQIPtr<IServiceProviderSupport> pServiceProviderSupport = m_pTimelineControl;
		ATLASSERT(pServiceProviderSupport);
		CComPtr<IServiceProvider> pServiceProviderTimelineControl;
		RETURN_IF_FAILED(pServiceProviderSupport->GetServiceProvider(&pServiceProviderTimelineControl));

		CComPtr<ITimelineService> pTimelineService;
		RETURN_IF_FAILED(pServiceProviderTimelineControl->QueryService(SERVICE_TIMELINE, &pTimelineService));

		CComQIPtr<IInitializeWithVariantObject> pInit = pTimelineService;
		ATLASSERT(pInit);

		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObject));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Metadata::Object::Text, &CComVar(strText)));
		RETURN_IF_FAILED(pInit->SetVariantObject(pVariantObject));

		CComPtr<IThreadService> pThreadService;
		RETURN_IF_FAILED(pServiceProviderTimelineControl->QueryService(SERVICE_TIMELINE_UPDATE_THREAD, &pThreadService));
		RETURN_IF_FAILED(pThreadService->Run());
	}
	return S_OK;
}

LRESULT CSearchControl::OnClickedGo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ASSERT_IF_FAILED(DoSearch());
	return 0;
}

LRESULT CSearchControl::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CAxDialogImpl<CSearchControl>::OnInitDialog(uMsg, wParam, lParam, bHandled);
	DlgResize_Init(false);
	m_editText = GetDlgItem(IDC_EDITTEXT);
	m_buttonGo = GetDlgItem(IDC_BUTTONGO);
	
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SearchTimelineControl, &m_pTimelineControl));
	CComQIPtr<IControl> pControl = m_pTimelineControl;
	ATLASSERT(pControl);
	RETURN_IF_FAILED(pControl->CreateEx(m_hWnd, &m_hWndTimelineControl));
	AdjustSizes();
	return 0;
}

LRESULT CSearchControl::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	AdjustSizes();
	return CDialogResize<CSearchControl>::OnSize(uMsg, wParam, lParam, bHandled);
}

void CSearchControl::AdjustSizes()
{
	CRect clientRect;
	GetClientRect(&clientRect);
	if (clientRect.Height() < OFFSET_Y + 1)
		return;

	::SetWindowPos(m_hWndTimelineControl, 0, 0, OFFSET_Y, clientRect.Width(), clientRect.Height() - OFFSET_Y, SWP_SHOWWINDOW);
}

void CSearchControl::EnableControls(BOOL bEnable)
{
	m_editText.EnableWindow(bEnable);
	m_buttonGo.EnableWindow(bEnable);
}

STDMETHODIMP CSearchControl::GetVariantObject(IVariantObject** ppVariantObject)
{
	CHECK_E_POINTER(ppVariantObject);
	if (m_pVariantObject)
	{
		RETURN_IF_FAILED(m_pVariantObject->QueryInterface(ppVariantObject));
	}
	return S_OK;
}

STDMETHODIMP CSearchControl::SetVariantObject(IVariantObject* pVariantObject)
{
	m_pVariantObject = pVariantObject;
	CComVar vText;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Metadata::Object::Text, &vText));
	ATLASSERT(vText.vt == VT_BSTR);
	m_editText.SetWindowText(vText.bstrVal);
	RETURN_IF_FAILED(DoSearch());
	return S_OK;
}
