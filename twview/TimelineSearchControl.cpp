#include "stdafx.h"
#include "TimelineSearchControl.h"
#include "..\twiconn\Plugins.h"
#include "Plugins.h"

#define OFFSET_Y 30

STDMETHODIMP CTimelineSearchControl::OnInitialized(IServiceProvider* pServiceProvider)
{
    CHECK_E_POINTER(pServiceProvider);
    return S_OK;
}

STDMETHODIMP CTimelineSearchControl::OnShutdown()
{
    RETURN_IF_FAILED(m_pSkinCommonControl->UnregisterStaticControl(m_hWnd));
    RETURN_IF_FAILED(m_pSkinCommonControl->UnregisterButtonControl(m_buttonGo));

    m_editText.Clear();

    m_pSkinCommonControl.Release();
    m_pTheme.Release();
    DestroyWindow();
    return S_OK;
}

STDMETHODIMP CTimelineSearchControl::GetHWND(HWND *hWnd)
{
    *hWnd = m_hWnd;
    return S_OK;
}

STDMETHODIMP CTimelineSearchControl::CreateEx(HWND hWndParent, HWND *hWnd)
{
    *hWnd = Create(hWndParent);
    return S_OK;
}

STDMETHODIMP CTimelineSearchControl::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
    if ((pMsg->hwnd == m_hWnd || IsChild(pMsg->hwnd)) && IsDialogMessage(pMsg))
    {
        *pbResult = TRUE;
        return S_OK;
    }

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

    return S_OK;
}

STDMETHODIMP CTimelineSearchControl::SetSkinCommonControl(ISkinCommonControl* pSkinCommonControl)
{
    CHECK_E_POINTER(pSkinCommonControl);
    m_pSkinCommonControl = pSkinCommonControl;

    RETURN_IF_FAILED(m_pSkinCommonControl->RegisterStaticControl(m_hWnd));
    RETURN_IF_FAILED(m_pSkinCommonControl->RegisterButtonControl(m_buttonGo));

    return S_OK;
}

STDMETHODIMP CTimelineSearchControl::DoSearch()
{
    CComBSTR bstrText;
    m_editText.GetWindowText(&bstrText);
    Fire_OnSearch(bstrText);
    return S_OK;
}

LRESULT CTimelineSearchControl::OnTextChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    CString strText;
    m_editText.GetWindowText(strText);
    ASSERT_IF_FAILED(Fire_OnSearch(CComBSTR(strText)));
    return 0;
}

LRESULT CTimelineSearchControl::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CAxDialogImpl<CTimelineSearchControl>::OnInitDialog(uMsg, wParam, lParam, bHandled);
    DlgResize_Init(false);
    m_editText = GetDlgItem(IDC_EDITTEXT);
    m_buttonGo = GetDlgItem(IDC_BUTTONGO);
    return 0;
}

LRESULT CTimelineSearchControl::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    return CDialogResize<CTimelineSearchControl>::OnSize(uMsg, wParam, lParam, bHandled);
}

void CTimelineSearchControl::EnableControls(BOOL bEnable)
{
    m_editText.EnableWindow(bEnable);
    m_buttonGo.EnableWindow(bEnable);
}

HRESULT CTimelineSearchControl::Fire_OnSearch(BSTR bstrSearchPattern)
{
    HRESULT hr = S_OK;
    CTimelineSearchControl* pThis = static_cast<CTimelineSearchControl*>(this);
    int cConnections = m_vec.GetSize();

    for (int iConnection = 0; iConnection < cConnections; iConnection++)
    {
        pThis->Lock();
        CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
        pThis->Unlock();

        ITimelineSearchControlEventSink* pConnection = static_cast<ITimelineSearchControlEventSink*>(punkConnection.p);

        if (pConnection)
        {
            hr = pConnection->OnSearch(bstrSearchPattern);
        }
    }
    return hr;
}
