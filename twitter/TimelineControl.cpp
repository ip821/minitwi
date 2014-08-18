// TimelineControl.cpp : Implementation of CTimelineControl

#include "stdafx.h"
#include "TimelineControl.h"
#include "Plugins.h"

// CTimelineControl

STDMETHODIMP CTimelineControl::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;
	return S_OK;
}

STDMETHODIMP CTimelineControl::OnShutdown()
{
	m_pServiceProvider.Release();
	return S_OK;
}

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
	HrCoCreateInstance(CLSID_PluginSupport, &m_pPluginSupport);
	m_pPluginSupport->InitializePlugins(PNAMESPACE_TIMELINE_CONTROL, PVIEWTYPE_COMMAND);
	CComQIPtr<IInitializeWithControl> pInit = m_pPluginSupport;
	if (pInit)
	{
		pInit->SetControl(m_pControl);
	}

	m_pPluginSupport->OnInitialized();

	m_popupMenu.CreatePopupMenu();
	HrCoCreateInstance(CLSID_CommandSupport, &m_pCommandSupport);
	m_pCommandSupport->SetMenu(m_popupMenu);
	m_pCommandSupport->InstallCommands(m_pPluginSupport);
	bHandled = FALSE;
	return 0;
}

STDMETHODIMP CTimelineControl::SetItems(IObjectArray* pObjectArray)
{
	m_listBox.SetRedraw(FALSE);
	m_listBox.Clear();
	UINT uiCount = 0;
	RETURN_IF_FAILED(pObjectArray->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(pObjectArray->GetAt(i, IID_IVariantObject, (LPVOID*)&pVariantObject));

		m_listBox.AddItem(pVariantObject);
	}
	m_listBox.SetRedraw();
	return S_OK;
}

STDMETHODIMP CTimelineControl::UpdateControl(IControl *pControl)
{
	return S_OK;
}

STDMETHODIMP CTimelineControl::SetSkinTimeline(ISkinTimeline* pSkinTimeline)
{
	m_listBox.SetSkinTimeline(pSkinTimeline);
	return S_OK;
}

LRESULT CTimelineControl::OnColumnClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	NMCOLUMNCLICK* pNm = (NMCOLUMNCLICK*)pnmh;
	if (pNm->dwCurrentColumn == -1)
		return 0;

	CComPtr<IOpenUrlService> pOpenUrlService;
	m_pServiceProvider->QueryService(SERVICE_OPEN_URLS, &pOpenUrlService);

	CComBSTR bstrIsUrl;
	pNm->pColumnRects->GetRectProp(pNm->dwCurrentColumn, VAR_IS_URL, &bstrIsUrl);

	if (bstrIsUrl == L"1")
	{
		CComBSTR bstrColumnName;
		pNm->pColumnRects->GetRectProp(pNm->dwCurrentColumn, VAR_COLUMN_NAME, &bstrColumnName);

		if (pOpenUrlService)
		{
			pOpenUrlService->OpenColumnAsUrl(bstrColumnName, pNm->dwCurrentColumn, pNm->pColumnRects, pNm->pVariantObject);
		}
	}

	return 0;
}

LRESULT CTimelineControl::OnColumnRClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	NMCOLUMNCLICK* pNm = (NMCOLUMNCLICK*)pnmh;
	m_popupMenu.TrackPopupMenu(0, pNm->x, pNm->y, m_hWnd);
	return 0;
}