#include "stdafx.h"
#include "UserAccountControl.h"

CUserAccountControl::CUserAccountControl()
{
}

STDMETHODIMP CUserAccountControl::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ImageManagerService, &m_pImageManagerService));
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

STDMETHODIMP CUserAccountControl::SetSkinUserAccountControl(ISkinUserAccountControl* pSkinUserAccountControl)
{
	CHECK_E_POINTER(pSkinUserAccountControl);
	m_pSkinUserAccountControl = pSkinUserAccountControl;
	return S_OK;
}

LRESULT CUserAccountControl::OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CRect rect;
	GetClientRect(&rect);
	if (m_pVariantObject)
		m_pSkinUserAccountControl->EraseBackground((HDC)wParam, &rect, m_pVariantObject);
	
	return 0;
}

LRESULT CUserAccountControl::OnPrintClient(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CRect rect;
	GetClientRect(&rect);
	m_pSkinUserAccountControl->Draw((HDC)wParam, &rect, m_pVariantObject);
	return 0;
}

LRESULT CUserAccountControl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps = { 0 };
	BeginPaint(&ps);

	CRect rect = ps.rcPaint;
	CDC cdc(ps.hdc);

	m_pSkinUserAccountControl->Draw(cdc, &rect, m_pVariantObject);

	EndPaint(&ps);
	return 0;
}
