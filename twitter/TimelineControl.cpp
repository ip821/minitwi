// TimelineControl.cpp : Implementation of CTimelineControl

#include "stdafx.h"
#include "TimelineControl.h"


// CTimelineControl

STDMETHODIMP CTimelineControl::GetText(BSTR* pbstr)
{
	*pbstr = CComBSTR(L"Home").Detach();
	return S_OK;
}

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
	ItemData data1 = { 0 };
	data1.strName = L"tweet1";
	data1.strText =
		CString(L"A communication channel") +
		L"that uses information routed through a packet-switching" +
		L"network. This information includes separate packets of " +
		L"information and the delivery information associated with " +
		L"those packets, such as the destination address. In a " +
		L"packet-switching network, data packets are routed independently " +
		L"of one another and may follow different routes. They may also arrive" +
		L"in a different order from the one in which they were sent.";
	m_listBox.AddItem(data1);

	data1.strName = L"tweet2";
	data1.strText =
		CString(L"A communication channel") +
		L"that uses information routed through a packet-switching" +
		L"network. This information includes separate packets of " +
		L"information and the delivery information associated with " +
		L"those packets, such as the destination address. In a " +
		L"packet-switching network, data packets are routed independently " +
		L"of one another and may follow different routes. They may also arrive" +
		L"in a different order from the one in which they were sent.";
	m_listBox.AddItem(data1);

	data1.strName = L"tweet3";
	data1.strText =
		CString(L"A communication channel") +
		L"that uses information routed through a packet-switching" +
		L"network. This information includes separate packets of " +
		L"information and the delivery information associated with " +
		L"those packets, such as the destination address. In a " +
		L"packet-switching network, data packets are routed independently " +
		L"of one another and may follow different routes. They may also arrive" +
		L"in a different order from the one in which they were sent.";
	m_listBox.AddItem(data1);

	bHandled = FALSE;
	return 0;
}

STDMETHODIMP CTimelineControl::UpdateControl(IControl *pControl)
{
	return S_OK;
}