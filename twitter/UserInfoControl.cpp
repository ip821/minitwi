#include "stdafx.h"
#include "UserInfoControl.h"

CUserInfoControl::CUserInfoControl()
{
}

STDMETHODIMP CUserInfoControl::OnInitialized(IServiceProvider *pServiceProvider)
{
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
	{
		CComQIPtr<IInitializeWithControl> pInit = m_pUserAccountControl;
		if (pInit)
		{
			RETURN_IF_FAILED(pInit->SetControl(m_pControl));
		}

		CComQIPtr<IInitializeWithVariantObject> pInit2 = m_pUserAccountControl;
		if (pInit2)
		{
			RETURN_IF_FAILED(pInit2->SetVariantObject(m_pVariantObject));
		}
	}
	{
		CComQIPtr<IInitializeWithControl> pInit = m_pTimelineControl;
		if (pInit)
		{
			RETURN_IF_FAILED(pInit->SetControl(m_pControl));
		}

		CComQIPtr<IInitializeWithVariantObject> pInit2 = m_pTimelineControl;
		if (pInit2)
		{
			RETURN_IF_FAILED(pInit2->SetVariantObject(m_pVariantObject));
		}
	}
	return S_OK;
}

STDMETHODIMP CUserInfoControl::SetTheme(ITheme* pTheme)
{
	CHECK_E_POINTER(pTheme);
	m_pTheme = pTheme;
	return S_OK;
}
