// TimelineControl.cpp : Implementation of CTimelineControl

#include "stdafx.h"
#include "TimelineControl.h"


// CTimelineControl

STDMETHODIMP CTimelineControl::GetHWND(HWND *hWnd)
{
	CHECK_E_POINTER(hWnd);
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CTimelineControl::CreateEx(HWND hWndParent, HWND *hWnd)
{
	CHECK_E_POINTER(hWnd);
	m_hWnd = __super::Create(hWndParent);
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CTimelineControl::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
	CHECK_E_POINTER(pMsg);
	CHECK_E_POINTER(pbResult);
	return S_OK;
}

STDMETHODIMP CTimelineControl::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plResult, BOOL* bResult)
{
	CHECK_E_POINTER(bResult);
	*bResult = ProcessWindowMessage(hWnd, uMsg, wParam, lParam, *plResult);
	return S_OK;
}

LRESULT CTimelineControl::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	DlgResize_Init(false);
	m_listBox.SubclassWindow(GetDlgItem(IDC_LIST1));
	m_listBox.AddString(L"123123123123123123123123123123123123123123123123123123123");
	bHandled = FALSE;
	return 0;
}

STDMETHODIMP CTimelineControl::UpdateControl(IControl *pControl)
{
	return S_OK;
}