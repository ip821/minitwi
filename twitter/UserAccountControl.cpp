#include "stdafx.h"
#include "UserAccountControl.h"

CUserAccountControl::CUserAccountControl()
{
}

STDMETHODIMP CUserAccountControl::OnInitialized(IServiceProvider *pServiceProvider)
{
	return S_OK;
}

STDMETHODIMP CUserAccountControl::OnShutdown()
{
	return S_OK;
}

STDMETHODIMP CUserAccountControl::GetHWND(HWND *hWnd)
{
	CHECK_E_POINTER(hWnd);
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CUserAccountControl::CreateEx(HWND hWndParent, HWND *hWnd)
{
	CHECK_E_POINTER(hWnd);
	*hWnd = Create(hWndParent);
	return S_OK;
}

STDMETHODIMP CUserAccountControl::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
	return S_OK;
}

STDMETHODIMP CUserAccountControl::SetVariantObject(IVariantObject *pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	m_pVariantObject = pVariantObject;
	return S_OK;
}

STDMETHODIMP CUserAccountControl::SetTheme(ITheme* pTheme)
{
	CHECK_E_POINTER(pTheme);
	m_pTheme = pTheme;
	return S_OK;
}

LRESULT CUserAccountControl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps = { 0 };
	BeginPaint(&ps);
	
	CDC cdc(ps.hdc);

	EndPaint(&ps);
	return 0;
}

LRESULT CUserAccountControl::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CAxDialogImpl<CUserAccountControl>::OnInitDialog(uMsg, wParam, lParam, bHandled);
	return 0;
}