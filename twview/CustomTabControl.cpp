#include "stdafx.h"
#include "CustomTabControl.h"
#include "Plugins.h"
#include "..\twtheme\GdilPlusUtils.h"

STDMETHODIMP CCustomTabControl::GetHWND(HWND *hWnd)
{
	CHECK_E_POINTER(hWnd);
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CCustomTabControl::CreateEx(HWND hWndParent, HWND *hWnd)
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &m_pControls));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ColumnsInfo, &m_pColumnsInfo));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ScrollControl, &m_pScrollControl));
	*hWnd = __super::Create(hWndParent, 0, L"", WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_COMPOSITED | WS_EX_CONTROLPARENT);
	RETURN_IF_FAILED(m_pScrollControl->CreateEx(m_hWnd, nullptr));
	RETURN_IF_FAILED(m_pScrollControl->SetTabControl(this));
	return S_OK;
}

STDMETHODIMP CCustomTabControl::StartAnimation()
{
	m_cAnimationRefs++;
	UINT uiMilliseconds = 0;
	RETURN_IF_FAILED(m_pSkinTabControl->AnimationGetParams(&uiMilliseconds));
	SetTimer(1, uiMilliseconds);
	return S_OK;
}

STDMETHODIMP CCustomTabControl::StopAnimation(UINT* puiRefs)
{
	CHECK_E_POINTER(puiRefs);
	if (!m_cAnimationRefs)
		return S_OK;

	--m_cAnimationRefs;
	*puiRefs = m_cAnimationRefs;

	if (m_cAnimationRefs > 0)
		return S_OK;

	KillTimer(1);
	CRect rectClient;
	GetClientRect(&rectClient);
	rectClient.bottom = m_rectChildControlArea.top - 1;
	InvalidateRect(rectClient);

	return S_OK;
}

LRESULT CCustomTabControl::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (m_pSkinTabControl)
		m_pSkinTabControl->AnimationNextFrame();
	CRect rectClient;
	GetClientRect(&rectClient);
	rectClient.bottom = m_rectChildControlArea.top - 1;
	InvalidateRect(rectClient);
	return 0;
}

STDMETHODIMP CCustomTabControl::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
	CComPtr<IControl> pControl;
	GetCurrentPage(&pControl);
	if (pControl)
	{
		RETURN_IF_FAILED(pControl->PreTranslateMessage(pMsg, pbResult));
	}
	return S_OK;
}

STDMETHODIMP CCustomTabControl::UpdateControl(IControl *pControl)
{
	return E_NOTIMPL;
}

STDMETHODIMP CCustomTabControl::EnableCommands(BOOL bEnabled)
{
	return S_OK;
}

STDMETHODIMP CCustomTabControl::AddPage(IControl *pControl)
{
	CHECK_E_POINTER(pControl);
	RETURN_IF_FAILED(m_pControls->AddObject(pControl));
	HWND hWnd = NULL;
	RETURN_IF_FAILED(pControl->CreateEx(m_hWnd, &hWnd));

	UINT uiCount = 0;
	RETURN_IF_FAILED(m_pControls->GetCount(&uiCount));

	if (uiCount > 1)
		::ShowWindow(hWnd, SW_HIDE);

	if (m_selectedPageIndex == INVALID_PAGE_INDEX)
		SelectPage(0);

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(IID_IUnknown, (LPVOID*)&pUnk));
	RETURN_IF_FAILED(HrInitializeWithControl(pControl, pUnk));
	RETURN_IF_FAILED(HrInitializeWithSettings(pControl, m_pSettings));
	RETURN_IF_FAILED(HrNotifyOnInitialized(pControl, m_pServiceProvider));

	AdjustSize();
	return S_OK;
}

void CCustomTabControl::SelectPage(DWORD dwIndex)
{
	if (m_selectedPageIndex == (int)dwIndex)
		return;

	CBitmap bitmap1;
	CDC cdcBitmap1;
	CDCSelectBitmapManualScope cdcSelectBitmapManualScope1;
	CBitmap bitmap2;
	CDC cdcBitmap2;
	CDCSelectBitmapManualScope cdcSelectBitmapManualScope2;

	UINT uiCount = 0;
	m_pControls->GetCount(&uiCount);
	ATLASSERT(dwIndex >= 0 && dwIndex < uiCount);
	
	if (m_selectedPageIndex != INVALID_PAGE_INDEX)
	{
		CComPtr<IControl> pControl;
		m_pControls->GetAt(m_selectedPageIndex, __uuidof(IControl), (LPVOID*)&pControl);

		HWND hWnd = 0;
		pControl->GetHWND(&hWnd);
		::ShowWindow(hWnd, SW_HIDE);

		if (m_selectedPageIndex != INVALID_PAGE_INDEX)
		{
			CClientDC cdc(hWnd);
			auto currentBitmap = cdc.GetCurrentBitmap();
			bitmap1.CreateCompatibleBitmap(cdc, m_rectChildControlArea.Width(), m_rectChildControlArea.Height());
			cdcBitmap1.CreateCompatibleDC(cdc);
			cdcSelectBitmapManualScope1.SelectBitmap(cdcBitmap1, bitmap1);
			::SendMessage(hWnd, WM_PRINT, (WPARAM)cdcBitmap1.m_hDC, PRF_CHILDREN | PRF_CLIENT | PRF_NONCLIENT | PRF_ERASEBKGND);
			cdc.SelectBitmap(currentBitmap);
		}
	}

	{
		CComPtr<IControl> pControl;
		m_pControls->GetAt(dwIndex, __uuidof(IControl), (LPVOID*)&pControl);
		HWND hWnd = 0;
		pControl->GetHWND(&hWnd);

		if (m_selectedPageIndex != INVALID_PAGE_INDEX)
		{
			CClientDC cdc(hWnd);
			auto currentBitmap = cdc.GetCurrentBitmap();
			bitmap2.CreateCompatibleBitmap(cdc, m_rectChildControlArea.Width(), m_rectChildControlArea.Height());
			cdcBitmap2.CreateCompatibleDC(cdc);
			cdcSelectBitmapManualScope2.SelectBitmap(cdcBitmap2, bitmap2);
			::SendMessage(hWnd, WM_PRINT, (WPARAM)cdcBitmap2.m_hDC, PRF_CHILDREN | PRF_CLIENT | PRF_NONCLIENT | PRF_ERASEBKGND);
			cdc.SelectBitmap(currentBitmap);
		}
	}

	if (m_selectedPageIndex != INVALID_PAGE_INDEX)
	{
		if (!m_scrollBitmap.IsNull())
			m_scrollBitmap.DeleteObject();

		CClientDC cdc(m_hWnd);
		auto currentBitmap = cdc.GetCurrentBitmap();
		m_scrollBitmap.CreateCompatibleBitmap(cdc, m_rectChildControlArea.Width() * 2, m_rectChildControlArea.Height());

		BOOL bRightToLeft = (int)dwIndex > m_selectedPageIndex;

		CDC cdcBitmap;
		cdcBitmap.CreateCompatibleDC(cdc);
		{
			CDCSelectBitmapScope cdcSelectBitmapScope(cdcBitmap, m_scrollBitmap);
			BitBlt(cdcBitmap, 0, 0, m_rectChildControlArea.Width(), m_rectChildControlArea.Height(), bRightToLeft ? cdcBitmap1 : cdcBitmap2, 0, 0, SRCCOPY);
			BitBlt(cdcBitmap, m_rectChildControlArea.Width(), 0, m_rectChildControlArea.Width(), m_rectChildControlArea.Height(), bRightToLeft ? cdcBitmap2 : cdcBitmap1, 0, 0, SRCCOPY);
		}
		
		cdc.SelectBitmap(currentBitmap);
		HWND hWnd = 0;
		m_pScrollControl->GetHWND(&hWnd);
		::SetWindowPos(hWnd, NULL, m_rectChildControlArea.left, m_rectChildControlArea.top, m_rectChildControlArea.Width(), m_rectChildControlArea.Height(), SWP_SHOWWINDOW);
		m_pScrollControl->SetBitmap(m_scrollBitmap.m_hBitmap);
		m_pScrollControl->Scroll(bRightToLeft);
	}

	m_nextSelectedPageIndex = dwIndex;

	if (m_selectedPageIndex == INVALID_PAGE_INDEX)
	{
		OnEndScroll();
	}
}

STDMETHODIMP CCustomTabControl::OnEndScroll()
{
	CComPtr<IControl> pNextControl;
	ASSERT_IF_FAILED(m_pControls->GetAt(m_nextSelectedPageIndex, __uuidof(IControl), (LPVOID*)&pNextControl));

	if (m_selectedPageIndex != INVALID_PAGE_INDEX)
	{
		CComPtr<IControl> pControl;
		ASSERT_IF_FAILED(GetPage(m_selectedPageIndex, &pControl));

		CComQIPtr<IControl2> pControl2 = pControl;
		if (pControl2)
		{
			ASSERT_IF_FAILED(pControl2->OnDeactivate());
		}

		ASSERT_IF_FAILED(Fire_OnDeactivate(pControl));
	}

	m_pScrollControl->ShowWindow(SW_HIDE);
	HWND hWnd = 0;
	ASSERT_IF_FAILED(pNextControl->GetHWND(&hWnd));
	::ShowWindow(hWnd, SW_SHOW);

	ASSERT_IF_FAILED(Fire_OnActivate(pNextControl));
	CComQIPtr<IControl2> pControl2 = pNextControl;
	if (pControl2)
	{
		ASSERT_IF_FAILED(pControl2->OnActivate());
	}

	if (!m_scrollBitmap.IsNull())
		m_scrollBitmap.DeleteObject();

	m_selectedPageIndex = m_nextSelectedPageIndex;
	m_nextSelectedPageIndex = INVALID_PAGE_INDEX;

	ASSERT_IF_FAILED(UpdateColumnInfo());
	Invalidate(TRUE);
	return S_OK;
}

STDMETHODIMP CCustomTabControl::GetCurrentPage(IControl **ppControl)
{
	CHECK_E_POINTER(ppControl);
	if (m_selectedPageIndex == INVALID_PAGE_INDEX)
		return E_PENDING;

#ifdef DEBUG
	UINT uiCount = 0;
	ASSERT_IF_FAILED(m_pControls->GetCount(&uiCount));
	ATLASSERT(m_selectedPageIndex <= (int)uiCount);
#endif

	CComPtr<IControl> pControl;
	RETURN_IF_FAILED(m_pControls->GetAt(m_selectedPageIndex, __uuidof(IControl), (LPVOID*)&pControl));

	RETURN_IF_FAILED(pControl->QueryInterface(ppControl));
	return S_OK;
}

STDMETHODIMP CCustomTabControl::RemovePage(IControl *pControl)
{
	CHECK_E_POINTER(pControl);

	UINT uiCount = 0;
	RETURN_IF_FAILED(m_pControls->GetCount(&uiCount));
	int index = INVALID_PAGE_INDEX;
	for (UINT i = 0; i < uiCount; i++)
	{
		CComPtr<IControl> pFoundControl;
		RETURN_IF_FAILED(m_pControls->GetAt(i, __uuidof(IControl), (LPVOID*)&pFoundControl));
		if (pControl == pFoundControl)
		{
			index = i;
			break;
		}
	}

	if (index == INVALID_PAGE_INDEX)
		return E_INVALIDARG;

	{
		CComQIPtr<IControl2> pControl2 = pControl;
		if (pControl2)
		{
			RETURN_IF_FAILED(pControl2->OnClose());
		}
	}

	RETURN_IF_FAILED(m_pControls->RemoveObjectAt(index));

	if (m_selectedPageIndex >= index)
		m_selectedPageIndex--;

	if (m_nextSelectedPageIndex >= index)
		m_nextSelectedPageIndex--;

	Fire_OnClose(pControl);

	return S_OK;
}

STDMETHODIMP CCustomTabControl::ActivatePage(IControl *pControl)
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(m_pControls->GetCount(&uiCount));
	int index = INVALID_PAGE_INDEX;
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IControl> pFoundControl;
		m_pControls->GetAt(i, __uuidof(IControl), (LPVOID*)&pFoundControl);
		if (pControl == pFoundControl)
		{
			index = i;
			break;
		}
	}

	if (index == INVALID_PAGE_INDEX)
		return E_INVALIDARG;

	SelectPage(index);
	return S_OK;
}

STDMETHODIMP CCustomTabControl::GetPageCount(DWORD *pdwCount)
{
	CHECK_E_POINTER(pdwCount);
	UINT uiCount = 0;
	RETURN_IF_FAILED(m_pControls->GetCount(&uiCount));
	*pdwCount = uiCount;
	return S_OK;
}

STDMETHODIMP CCustomTabControl::GetPage(DWORD dwIndex, IControl **ppControl)
{
	CHECK_E_POINTER(ppControl);
	UINT uiCount = 0;
	RETURN_IF_FAILED(m_pControls->GetCount(&uiCount));

	if (dwIndex > uiCount - 1 || dwIndex < 0)
		return E_INVALIDARG;

	CComPtr<IControl> pFoundControl;
	m_pControls->GetAt(dwIndex, __uuidof(IControl), (LPVOID*)&pFoundControl);

	RETURN_IF_FAILED(pFoundControl->QueryInterface(ppControl));
	return S_OK;
}

STDMETHODIMP CCustomTabControl::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	UINT uiCount = 0;
	RETURN_IF_FAILED(m_pControls->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IControl> pFoundControl;
		m_pControls->GetAt(i, __uuidof(IControl), (LPVOID*)&pFoundControl);

		CComQIPtr<IPluginSupportNotifications> pp = pFoundControl;
		if (pp)
		{
			RETURN_IF_FAILED(pp->OnInitialized(pServiceProvider));
		}
	}
	m_pServiceProvider = pServiceProvider;
	RETURN_IF_FAILED(HrNotifyOnInitialized(m_pScrollControl, m_pServiceProvider));
	return S_OK;
}

STDMETHODIMP CCustomTabControl::OnShutdown()
{
	HrNotifyOnShutdown(m_pScrollControl);
	m_pScrollControl.Release();
	UINT uiCount = 0;
	RETURN_IF_FAILED(m_pControls->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IControl> pFoundControl;
		m_pControls->GetAt(i, __uuidof(IControl), (LPVOID*)&pFoundControl);
		CComQIPtr<IPluginSupportNotifications> pp = pFoundControl;
		if (pp)
		{
			RETURN_IF_FAILED(pp->OnShutdown());
		}
	}
	m_pServiceProvider.Release();
	m_pSettings.Release();
	m_pSkinTabControl.Release();
	m_pColumnsInfo.Release();
	m_pControls.Release();
	return S_OK;
}

STDMETHODIMP CCustomTabControl::Load(ISettings* pSettings)
{
	CHECK_E_POINTER(pSettings);
	UINT uiCount = 0;
	RETURN_IF_FAILED(m_pControls->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IControl> pFoundControl;
		m_pControls->GetAt(i, __uuidof(IControl), (LPVOID*)&pFoundControl);
		CComQIPtr<IInitializeWithSettings> pp = pFoundControl;
		if (pp)
		{
			RETURN_IF_FAILED(pp->Load(pSettings));
		}
	}
	m_pSettings = pSettings;
	return S_OK;
}

STDMETHODIMP CCustomTabControl::Save(ISettings* pSettings)
{
	CHECK_E_POINTER(pSettings);
	UINT uiCount = 0;
	RETURN_IF_FAILED(m_pControls->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IControl> pFoundControl;
		m_pControls->GetAt(i, __uuidof(IControl), (LPVOID*)&pFoundControl);
		CComQIPtr<IPersistSettings> pp = pFoundControl;
		if (pp)
		{
			RETURN_IF_FAILED(pp->Save(pSettings));
		}
	}
	return S_OK;
}

HRESULT CCustomTabControl::UpdateColumnInfo()
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(m_pColumnsInfo->GetCount(&uiCount));

	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		ASSERT_IF_FAILED(m_pColumnsInfo->GetItem(i, &pColumnsInfoItem));
		ASSERT_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(Twitter::Metadata::Tabs::HeaderSelected, m_selectedPageIndex == static_cast<int>(i)));
	}
	return S_OK;
}

void CCustomTabControl::UpdateChildControlAreaRect()
{
	m_rectChildControlArea.SetRectEmpty();
	m_pColumnsInfo->Clear();
	CComQIPtr<IObjArray> pObjArray = m_pControls;
	UINT uiHeight = 0;
	CRect clientRect;
	GetClientRect(&clientRect);
	if (m_pSkinTabControl)
	{
		m_pSkinTabControl->MeasureHeader(m_hWnd, pObjArray, m_pColumnsInfo, &clientRect, &uiHeight);
		m_pSkinTabControl->GetInfoRect(&m_rectInfoImage);

		ASSERT_IF_FAILED(UpdateColumnInfo());
	}

	GetClientRect(&m_rectChildControlArea);
	m_rectChildControlArea.top += uiHeight;
}

STDMETHODIMP CCustomTabControl::SetSkinTabControl(ISkinTabControl* pSkinTabControl)
{
	CHECK_E_POINTER(pSkinTabControl);
	m_pSkinTabControl = pSkinTabControl;
	AdjustSize();
	return S_OK;
}

void CCustomTabControl::UpdateSizes()
{
	CRect clientRect = m_rectChildControlArea;

	if (clientRect.IsRectEmpty())
		return;

	UINT uiCount = 0;
	m_pControls->GetCount(&uiCount);
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IControl> pFoundControl;
		m_pControls->GetAt((UINT)i, __uuidof(IControl), (LPVOID*)&pFoundControl);
		HWND hWnd = 0;
		pFoundControl->GetHWND(&hWnd);
		UINT uiFlags = SWP_NOACTIVATE;
		uiFlags |= (UINT)m_selectedPageIndex == i ? SWP_SHOWWINDOW : SWP_HIDEWINDOW;
		::SetWindowPos(hWnd, NULL, clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), uiFlags);
	}
}

void CCustomTabControl::AdjustSize()
{
	UpdateChildControlAreaRect();
	UpdateSizes();
}

LRESULT CCustomTabControl::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	AdjustSize();
	return 0;
}

LRESULT CCustomTabControl::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (!m_pSkinTabControl)
		return 0;

	CRect rect;
	GetClientRect(&rect);

	PAINTSTRUCT ps = { 0 };
	BeginPaint(&ps);
	m_pSkinTabControl->DrawHeader(m_pColumnsInfo, ps.hdc, rect);

	if (m_cAnimationRefs > 0)
		m_pSkinTabControl->DrawAnimation(ps.hdc);
	else if (m_bShowInfoImage)
		m_pSkinTabControl->DrawInfoImage(ps.hdc, m_bInfoImageIsError, m_bstrInfoMessage);

	EndPaint(&ps);

	return 0;
}

LRESULT CCustomTabControl::OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_pSkinTabControl->EraseBackground((HDC)wParam);
	return 0;
}

LRESULT CCustomTabControl::OnLButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	auto x = GET_X_LPARAM(lParam);
	auto y = GET_Y_LPARAM(lParam);

	if (m_nextSelectedPageIndex != INVALID_PAGE_INDEX)
		return 0;

	if (m_bShowInfoImage && m_bInfoImageEnableClick && m_rectInfoImage.PtInRect(CPoint(x, y)))
	{
		Fire_OnLinkClick();
	}
	else
	{
		UINT uiCount = 0;
		m_pColumnsInfo->GetCount(&uiCount);
		for (size_t i = 0; i < uiCount; i++)
		{
			CComPtr<IColumnsInfoItem> pColumnsInfoItem;
			ASSERT_IF_FAILED(m_pColumnsInfo->GetItem(i, &pColumnsInfoItem));
			CRect rect;
			ASSERT_IF_FAILED(pColumnsInfoItem->GetRect(&rect));
			if (rect.PtInRect(CPoint(x, y)))
			{
				CComPtr<IControl> pControl;
				m_pControls->GetAt(i, __uuidof(IControl), (LPVOID*)&pControl);
				Fire_OnTabHeaderClick(pControl);
				SelectPage(i);
				break;
			}
		}
	}
	return 0;
}

LRESULT CCustomTabControl::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CComPtr<IControl> pControl;
	GetCurrentPage(&pControl);
	if (pControl)
	{
		HWND hWnd = 0;
		pControl->GetHWND(&hWnd);
		::SetFocus(hWnd);
	}
	return 0;
}

STDMETHODIMP CCustomTabControl::ShowInfo(BOOL bError, BOOL bInfoImageEnableClick, BSTR bstrMessage)
{
	m_bShowInfoImage = TRUE;
	m_bInfoImageIsError = bError;
	m_bstrInfoMessage = bstrMessage;
	m_bInfoImageEnableClick = bInfoImageEnableClick;
	m_pSkinTabControl->StartInfoImage();
	InvalidateRect(m_rectInfoImage);
	return S_OK;
}

STDMETHODIMP CCustomTabControl::HideInfo()
{
	m_bShowInfoImage = FALSE;
	m_bInfoImageIsError = FALSE;
	m_bstrInfoMessage.Empty();
	m_pSkinTabControl->StopInfoImage();
	return S_OK;
}

LRESULT CCustomTabControl::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	SetCursor(m_arrowCursor);

	auto x = GET_X_LPARAM(lParam);
	auto y = GET_Y_LPARAM(lParam);

	if (m_bShowInfoImage && m_bInfoImageEnableClick && m_rectInfoImage.PtInRect(CPoint(x, y)))
	{
		SetCursor(m_handCursor);
	}
	return 0;
}

HRESULT CCustomTabControl::Fire_OnLinkClick()
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	HRESULT hr = S_OK;
	CCustomTabControl* pThis = static_cast<CCustomTabControl*>(this);
	int cConnections = IConnectionPointImpl_IInfoControlEventSink::m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = IConnectionPointImpl_IInfoControlEventSink::m_vec.GetAt(iConnection);
		pThis->Unlock();

		IInfoControlEventSink* pConnection = static_cast<IInfoControlEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnLinkClick(NULL);
		}
	}
	return hr;
}

HRESULT CCustomTabControl::Fire_OnActivate(IControl* pControl)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	HRESULT hr = S_OK;
	CCustomTabControl* pThis = static_cast<CCustomTabControl*>(this);
	int cConnections = IConnectionPointImpl_ITabbedControlEventSink::m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = IConnectionPointImpl_ITabbedControlEventSink::m_vec.GetAt(iConnection);
		pThis->Unlock();

		ITabbedControlEventSink* pConnection = static_cast<ITabbedControlEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnActivate(pControl);
		}
	}
	return hr;
}

HRESULT CCustomTabControl::Fire_OnDeactivate(IControl* pControl)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	HRESULT hr = S_OK;
	CCustomTabControl* pThis = static_cast<CCustomTabControl*>(this);
	int cConnections = IConnectionPointImpl_ITabbedControlEventSink::m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = IConnectionPointImpl_ITabbedControlEventSink::m_vec.GetAt(iConnection);
		pThis->Unlock();

		ITabbedControlEventSink* pConnection = static_cast<ITabbedControlEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnDeactivate(pControl);
		}
	}
	return hr;
}

HRESULT CCustomTabControl::Fire_OnTabHeaderClick(IControl* pControl)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	HRESULT hr = S_OK;
	CCustomTabControl* pThis = static_cast<CCustomTabControl*>(this);
	int cConnections = IConnectionPointImpl_ITabbedControlEventSink::m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = IConnectionPointImpl_ITabbedControlEventSink::m_vec.GetAt(iConnection);
		pThis->Unlock();

		ITabbedControlEventSink* pConnection = static_cast<ITabbedControlEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnTabHeaderClick(pControl);
		}
	}
	return hr;
}

HRESULT CCustomTabControl::Fire_OnClose(IControl* pControl)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	HRESULT hr = S_OK;
	CCustomTabControl* pThis = static_cast<CCustomTabControl*>(this);
	int cConnections = IConnectionPointImpl_ITabbedControlEventSink::m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = IConnectionPointImpl_ITabbedControlEventSink::m_vec.GetAt(iConnection);
		pThis->Unlock();

		ITabbedControlEventSink* pConnection = static_cast<ITabbedControlEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnClose(pControl);
		}
	}
	return hr;
}
