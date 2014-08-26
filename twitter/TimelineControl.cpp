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
	auto dwExStyle = GetWindowLong(GWL_EXSTYLE);
	dwExStyle |= WS_EX_COMPOSITED;
	SetWindowLong(GWL_EXSTYLE, dwExStyle);
	m_listBox.SubclassWindow(GetDlgItem(IDC_LIST1));
	HrCoCreateInstance(CLSID_PluginSupport, &m_pPluginSupport);
	m_pPluginSupport->InitializePlugins(PNAMESP_TIMELINE_CONTROL, PVIEWTYPE_COMMAND);
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

STDMETHODIMP CTimelineControl::BeginUpdate()
{
	m_listBox.SetRedraw(FALSE);
	return S_OK;
}

STDMETHODIMP CTimelineControl::EndUpdate()
{
	m_listBox.SetRedraw();
	RETURN_IF_FAILED(Invalidate());
	return S_OK;
}

STDMETHODIMP CTimelineControl::OnItemsUpdated()
{
	m_listBox.OnItemsUpdated();
	return S_OK;
}

STDMETHODIMP CTimelineControl::Clear()
{
	m_listBox.Clear();
	return S_OK;
}

STDMETHODIMP CTimelineControl::GetItems(IObjArray** ppObjectArray)
{
	RETURN_IF_FAILED(m_listBox.GetItems(ppObjectArray));
	return S_OK;
}

STDMETHODIMP CTimelineControl::SetItems(IObjArray* pObjectArray)
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(pObjectArray->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(pObjectArray->GetAt(uiCount - i - 1, IID_IVariantObject, (LPVOID*)&pVariantObject));

		m_listBox.AddItem(pVariantObject);
	}
	return S_OK;
}

STDMETHODIMP CTimelineControl::Invalidate()
{
	m_listBox.Invalidate(TRUE);
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

	CComQIPtr<IInitializeWithVariantObject> pInitializeWithVariantObject = m_pCommandSupport;
	if (pInitializeWithVariantObject)
	{
		RETURN_IF_FAILED(pInitializeWithVariantObject->SetVariantObject(pNm->pVariantObject));
	}

	if (pNm->dwCurrentColumn != -1)
	{
		CComQIPtr<IInitializeWithColumnName> pInitializeWithColumnName = m_pCommandSupport;
		if (pInitializeWithColumnName)
		{
			CComBSTR bstrColumnName;
			pNm->pColumnRects->GetRectProp(pNm->dwCurrentColumn, VAR_COLUMN_NAME, &bstrColumnName);
			RETURN_IF_FAILED(pInitializeWithColumnName->SetColumnName(bstrColumnName));
		}
	}

	CComQIPtr<IIdleHandler> pIdleHandler = m_pCommandSupport;
	ATLASSERT(pIdleHandler);
	if (pIdleHandler)
	{
		BOOL bResult = FALSE;
		RETURN_IF_FAILED(pIdleHandler->OnIdle(&bResult));
	}

	m_popupMenu.TrackPopupMenu(0, pNm->x, pNm->y, m_hWnd);
	return 0;
}

LRESULT CTimelineControl::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BOOL bResult = FALSE;
	LRESULT lResult = 0;
	RETURN_IF_FAILED(m_pCommandSupport->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, &lResult, &bResult));
	bHandled = bResult;
	return lResult;
}
