#include "stdafx.h"
#include "UserInfoControl.h"
#include "Plugins.h"

#define USER_CONTROL_HEIGHT 240

CUserInfoControl::CUserInfoControl()
{
}

STDMETHODIMP CUserInfoControl::OnInitialized(IServiceProvider* pServiceProvider)
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_PluginSupport, &m_pPluginSupport));
	RETURN_IF_FAILED(m_pPluginSupport->InitializePlugins(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE));
	RETURN_IF_FAILED(m_pPluginSupport->SetParentServiceProvider(pServiceProvider));
	m_pServiceProvider = m_pPluginSupport;
	ATLASSERT(m_pServiceProvider);

	RETURN_IF_FAILED(HrInitializeWithControl(m_pUserAccountControl, m_pControl));
	RETURN_IF_FAILED(HrInitializeWithControl(m_pTimelineControl, m_pControl));
	RETURN_IF_FAILED(HrInitializeWithControl(m_pPluginSupport, m_pControl));

	RETURN_IF_FAILED(m_pPluginSupport->OnInitialized());
	RETURN_IF_FAILED(HrNotifyOnInitialized(m_pUserAccountControl, m_pPluginSupport));
	RETURN_IF_FAILED(HrNotifyOnInitialized(m_pTimelineControl, m_pPluginSupport));

	return S_OK;
}

STDMETHODIMP CUserInfoControl::OnShutdown()
{
	return S_OK;
}

LRESULT CUserInfoControl::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_UserAccountControl, &m_pUserAccountControl));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TimelineControl, &m_pTimelineControl));
	RETURN_IF_FAILED(m_pUserAccountControl->CreateEx(m_hWnd, &m_hWndUserAccountControl));
	RETURN_IF_FAILED(m_pTimelineControl->CreateEx(m_hWnd, &m_hWndTimelineControl));

	AdjustSizes();
	return 0;
}

LRESULT CUserInfoControl::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	AdjustSizes();
	return 0;
}

void CUserInfoControl::AdjustSizes()
{
	CRect clientRect;
	GetClientRect(&clientRect);
	if (clientRect.Height() < USER_CONTROL_HEIGHT + 1)
		return;

	::SetWindowPos(m_hWndUserAccountControl, 0, 0, 0, clientRect.Width(), USER_CONTROL_HEIGHT, 0);
	::SetWindowPos(m_hWndTimelineControl, 0, 0, USER_CONTROL_HEIGHT, clientRect.Width(), clientRect.Height() - USER_CONTROL_HEIGHT, 0);
}

STDMETHODIMP CUserInfoControl::GetHWND(HWND *hWnd)
{
	CHECK_E_POINTER(hWnd);
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CUserInfoControl::CreateEx(HWND hWndParent, HWND *hWnd)
{
	CHECK_E_POINTER(hWnd);
	*hWnd = Create(hWndParent, 0, 0, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_COMPOSITED | WS_EX_CONTROLPARENT);
	return S_OK;
}

STDMETHODIMP CUserInfoControl::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
	return S_OK;
}

STDMETHODIMP CUserInfoControl::SetVariantObject(IVariantObject *pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	m_pVariantObject = pVariantObject;
	RETURN_IF_FAILED(HrInitializeWithVariantObject(m_pUserAccountControl, m_pVariantObject));
	RETURN_IF_FAILED(HrInitializeWithVariantObject(m_pTimelineControl, m_pVariantObject));
	return S_OK;
}

STDMETHODIMP CUserInfoControl::SetTheme(ITheme* pTheme)
{
	CHECK_E_POINTER(pTheme);
	m_pTheme = pTheme;

	CComPtr<ISkinUserAccountControl> pSkinUserAccountControl;
	RETURN_IF_FAILED(pTheme->GetSkinUserAccountControl(&pSkinUserAccountControl));
	RETURN_IF_FAILED(m_pUserAccountControl->SetSkinUserAccountControl(pSkinUserAccountControl));

	CComPtr<ISkinTimeline> pSkinTimeline;
	RETURN_IF_FAILED(m_pTheme->GetTimelineSkin(&pSkinTimeline));
	RETURN_IF_FAILED(m_pTimelineControl->SetSkinTimeline(pSkinTimeline));
	return S_OK;
}

STDMETHODIMP CUserInfoControl::OnActivate()
{
	RETURN_IF_FAILED(m_pTimelineControl->OnActivate());
	RETURN_IF_FAILED(m_pUserAccountControl->OnActivate());
	return S_OK;
}

STDMETHODIMP CUserInfoControl::OnDeactivate()
{
	RETURN_IF_FAILED(m_pTimelineControl->OnDeactivate());
	RETURN_IF_FAILED(m_pUserAccountControl->OnDeactivate());
	return S_OK;
}
