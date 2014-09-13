#include "stdafx.h"
#include "CustomTabControl.h"
#include "Plugins.h"

STDMETHODIMP CCustomTabControl::GetHWND(HWND *hWnd)
{
	return S_OK;
}

STDMETHODIMP CCustomTabControl::CreateEx(HWND hWndParent, HWND *hWnd)
{
	HrCoCreateInstance(CLSID_ObjectCollection, &m_pControls);
	HrCoCreateInstance(CLSID_ColumnRects, &m_pColumnRects);
	*hWnd = __super::Create(hWndParent, CRect(), L"", WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_COMPOSITED | WS_EX_CONTROLPARENT);

	return S_OK;
}

STDMETHODIMP CCustomTabControl::StartAnimation()
{
	m_bDrawAnimation = TRUE;
	SetTimer(1, 200);
	return S_OK;
}

STDMETHODIMP CCustomTabControl::StopAnimation()
{
	KillTimer(1);
	m_bDrawAnimation = FALSE;
	Invalidate();
	return S_OK;
}

LRESULT CCustomTabControl::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	Invalidate();
	return 0;
}

STDMETHODIMP CCustomTabControl::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
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
	if (m_selectedPageIndex == -1)
		SelectPage(0);
	if (uiCount > 1)
		SelectPage(1);

	{
		CComQIPtr<IInitializeWithSettings> pp = pControl;
		if (pp)
		{
			RETURN_IF_FAILED(pp->Load(m_pSettings));
		}
	}

	{
		CComQIPtr<IPluginSupportNotifications> pp = pControl;
		if (pp)
		{
			RETURN_IF_FAILED(pp->OnInitialized(m_pServiceProvider));
		}
	}

	UpdateChildControlAreaRect();
	UpdateSizes();

	return S_OK;
}

void CCustomTabControl::SelectPage(DWORD dwIndex)
{
	if (m_selectedPageIndex == dwIndex)
		return;

	UINT uiCount = 0;
	m_pControls->GetCount(&uiCount);
	ATLASSERT(dwIndex >= 0 && dwIndex < uiCount);
	if (m_selectedPageIndex != -1)
	{
		CComPtr<IControl> pControl;
		m_pControls->GetAt(m_selectedPageIndex, __uuidof(IControl), (LPVOID*)&pControl);
		HWND hWnd = 0;
		pControl->GetHWND(&hWnd);
		::ShowWindow(hWnd, SW_HIDE);
		m_selectedPageIndex = -1;
	}

	{
		CComPtr<IControl> pControl;
		m_pControls->GetAt(dwIndex, __uuidof(IControl), (LPVOID*)&pControl);
		HWND hWnd = 0;
		pControl->GetHWND(&hWnd);
		::ShowWindow(hWnd, SW_SHOW);
	}
	m_selectedPageIndex = dwIndex;
	Invalidate(TRUE);
}

STDMETHODIMP CCustomTabControl::GetCurrentPage(IControl **ppControl)
{
	CHECK_E_POINTER(ppControl);
	if (m_selectedPageIndex == -1)
		return E_PENDING;

	CComPtr<IControl> pControl;
	RETURN_IF_FAILED(m_pControls->GetAt(m_selectedPageIndex, __uuidof(IControl), (LPVOID*)&pControl));

	RETURN_IF_FAILED(pControl->QueryInterface(ppControl));
	return S_OK;
}

STDMETHODIMP CCustomTabControl::RemovePage(IControl *pControl)
{
	return E_NOTIMPL;
}

STDMETHODIMP CCustomTabControl::ActivatePage(IControl *pControl)
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(m_pControls->GetCount(&uiCount));
	size_t index = -1;
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

	if (index == -1)
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
	return S_OK;
}

STDMETHODIMP CCustomTabControl::OnShutdown()
{
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

void CCustomTabControl::UpdateChildControlAreaRect()
{
	m_rectChildControlArea.SetRectEmpty();
	m_pColumnRects->Clear();
	CComQIPtr<IObjArray> pObjArray = m_pControls;
	UINT uiHeight = 0;
	CRect clientRect;
	GetClientRect(&clientRect);
	if (m_pSkinTabControl)
		m_pSkinTabControl->MeasureHeader(m_hWnd, pObjArray, m_pColumnRects, &clientRect, &uiHeight);

	GetClientRect(&m_rectChildControlArea);
	m_rectChildControlArea.top += uiHeight;
	m_rectChildControlArea.bottom -= uiHeight;
}

STDMETHODIMP CCustomTabControl::SetSkinTabControl(ISkinTabControl* pSkinTabControl)
{
	CHECK_E_POINTER(pSkinTabControl);
	m_pSkinTabControl = pSkinTabControl;
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
		m_pControls->GetAt(i, __uuidof(IControl), (LPVOID*)&pFoundControl);
		HWND hWnd = 0;
		pFoundControl->GetHWND(&hWnd);
		UINT uiFlags = SWP_NOACTIVATE;
		uiFlags |= m_selectedPageIndex == i ? SWP_SHOWWINDOW : SWP_HIDEWINDOW;
		::SetWindowPos(hWnd, NULL, clientRect.left, clientRect.top, clientRect.right, clientRect.bottom, uiFlags);
	}
}

LRESULT CCustomTabControl::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	UpdateChildControlAreaRect();
	UpdateSizes();
	return 0;
}

LRESULT CCustomTabControl::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	PAINTSTRUCT ps = { 0 };
	BeginPaint(&ps);
	m_pSkinTabControl->DrawHeader(m_pColumnRects, ps.hdc, ps.rcPaint, m_selectedPageIndex, m_bDrawAnimation);
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

	UINT uiCount = 0;
	m_pColumnRects->GetCount(&uiCount);
	for (size_t i = 0; i < uiCount; i++)
	{
		CRect rect;
		m_pColumnRects->GetRect(i, &rect);
		if (rect.PtInRect(CPoint(x, y)))
		{
			SelectPage(i);
			break;
		}
	}
	return 0;
}