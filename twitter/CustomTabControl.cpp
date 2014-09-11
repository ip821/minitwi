#include "stdafx.h"
#include "CustomTabControl.h"

STDMETHODIMP CCustomTabControl::GetHWND(HWND *hWnd)
{
	return S_OK;
}

STDMETHODIMP CCustomTabControl::CreateEx(HWND hWndParent, HWND *hWnd)
{
	*hWnd = __super::Create(hWndParent, CRect(), L"", WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_COMPOSITED);
	return S_OK;
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
	HWND hWnd = NULL;
	RETURN_IF_FAILED(pControl->CreateEx(m_hWnd, &hWnd));
	m_pControls.push_back(CAdapt<CComPtr<IControl>>(pControl));
	if (m_pControls.size() > 1)
		::ShowWindow(hWnd, SW_HIDE);
	if (m_selectedPageIndex == -1)
		SelectPage(0);
	if (m_pControls.size() > 1)
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

	return S_OK;
}

void CCustomTabControl::SelectPage(DWORD dwIndex)
{
	ATLASSERT(dwIndex >= 0 && dwIndex < m_pControls.size());
	if (m_selectedPageIndex != -1)
	{
		HWND hWnd = 0;
		m_pControls[m_selectedPageIndex]->GetHWND(&hWnd);
		::ShowWindow(hWnd, SW_HIDE);
		m_selectedPageIndex = -1;
	}

	HWND hWnd = 0;
	m_pControls[dwIndex]->GetHWND(&hWnd);
	::ShowWindow(hWnd, SW_SHOW);
	m_selectedPageIndex = dwIndex;
}

STDMETHODIMP CCustomTabControl::GetCurrentPage(IControl **ppControl)
{
	CHECK_E_POINTER(ppControl);
	if (m_selectedPageIndex == -1)
		return E_PENDING;
	RETURN_IF_FAILED(m_pControls[m_selectedPageIndex]->QueryInterface(ppControl));
	return S_OK;
}

STDMETHODIMP CCustomTabControl::RemovePage(IControl *pControl)
{
	return E_NOTIMPL;
}

STDMETHODIMP CCustomTabControl::ActivatePage(IControl *pControl)
{
	auto it = find_if(
		m_pControls.begin(),
		m_pControls.end(),
		[&](CAdapt<CComPtr<IControl>>& pc)
	{
		return pc.m_T == pControl;
	}
	);

	if (it == m_pControls.end())
		return E_INVALIDARG;
	SelectPage(distance(it, m_pControls.end()) - 1);
	return S_OK;
}

STDMETHODIMP CCustomTabControl::GetPageCount(DWORD *pdwCount)
{
	CHECK_E_POINTER(pdwCount);
	*pdwCount = m_pControls.size();
	return S_OK;
}

STDMETHODIMP CCustomTabControl::GetPage(DWORD dwIndex, IControl **ppControl)
{
	CHECK_E_POINTER(ppControl);
	if (dwIndex > m_pControls.size() - 1 || dwIndex < 0)
		return E_INVALIDARG;
	RETURN_IF_FAILED(m_pControls[dwIndex]->QueryInterface(ppControl));
	return S_OK;
}

STDMETHODIMP CCustomTabControl::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	for (auto& pControl : m_pControls)
	{
		CComQIPtr<IPluginSupportNotifications> pp = pControl.m_T;
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
	for (auto& pControl : m_pControls)
	{
		CComQIPtr<IPluginSupportNotifications> pp = pControl.m_T;
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
	for (auto& pControl : m_pControls)
	{
		CComQIPtr<IInitializeWithSettings> pp = pControl.m_T;
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
	for (auto& pControl : m_pControls)
	{
		CComQIPtr<IPersistSettings> pp = pControl.m_T;
		if (pp)
		{
			RETURN_IF_FAILED(pp->Save(pSettings));
		}
	}
	return S_OK;
}

CRect& CCustomTabControl::GetChildControlAreaRect()
{
	if (m_rectChildControlArea.IsRectEmpty())
	{

	}
	return m_rectChildControlArea;
}

STDMETHODIMP CCustomTabControl::SetSkinTabControl(ISkinTabControl* pSkinTabControl)
{
	CHECK_E_POINTER(pSkinTabControl);
	m_pSkinTabControl = pSkinTabControl;
	return S_OK;
}

LRESULT CCustomTabControl::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CRect clientRect;
	GetClientRect(&clientRect);

	for (size_t i = 0; i < m_pControls.size(); i++)
	{
		HWND hWnd = 0;
		m_pControls[i]->GetHWND(&hWnd);
		UINT uiFlags = SWP_NOACTIVATE | SWP_NOMOVE;
		uiFlags |= m_selectedPageIndex == i ? SWP_SHOWWINDOW : SWP_HIDEWINDOW;
		::SetWindowPos(hWnd, NULL, clientRect.left, clientRect.top, clientRect.right, clientRect.bottom, uiFlags);
	}
	return 0;
}
