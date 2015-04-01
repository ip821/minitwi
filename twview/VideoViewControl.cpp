#include "stdafx.h"
#include "VideoViewControl.h"
#include "..\twtheme\GdilPlusUtils.h"

#define TARGET_INTERVAL 15

CVideoViewControl::CVideoViewControl()
{

}

CVideoViewControl::~CVideoViewControl()
{
	if (IsWindow())
		DestroyWindow();
}

STDMETHODIMP CVideoViewControl::GetHWND(HWND* phWnd)
{
	CHECK_E_POINTER(phWnd);
	*phWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CVideoViewControl::CreateEx(HWND hWndParent, HWND *hWnd)
{
	m_hWnd = __super::Create(hWndParent, 0, 0, WS_VISIBLE | WS_CHILD);
	if (hWnd)
		*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CVideoViewControl::PreTranslateMessage(MSG *pMsg, BOOL *bResult)
{
	return S_OK;
}

LRESULT CVideoViewControl::OnForwardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return GetParent().PostMessage(uMsg, wParam, lParam);
}

STDMETHODIMP CVideoViewControl::SetVariantObject(IVariantObject *pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	m_pVariantObject = pVariantObject;

	CComVariant vUrl;
	RETURN_IF_FAILED(m_pVariantObject->GetVariantValue(Twitter::Metadata::Object::Url, &vUrl));
	ATLASSERT(vUrl.vt == VT_BSTR);
	m_bstrUrl = vUrl.bstrVal;

	return S_OK;
}

STDMETHODIMP CVideoViewControl::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	return S_OK;
}
STDMETHODIMP CVideoViewControl::OnShutdown()
{
	IInitializeWithControlImpl::OnShutdown();
	m_pVariantObject.Release();
	m_pServiceProvider.Release();
	m_pTheme.Release();
	return S_OK;
}

LRESULT CVideoViewControl::OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CVideoViewControl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps = { 0 };
	CDCHandle cdc(BeginPaint(&ps));

	CRect rect;
	GetClientRect(&rect);

	static DWORD dwBrushColor = 0;
	static DWORD dwTextColor = 0;
	if (!dwBrushColor)
	{
		CComPtr<IThemeColorMap> pThemeColorMap;
		ASSERT_IF_FAILED(m_pTheme->GetColorMap(&pThemeColorMap));
		ASSERT_IF_FAILED(pThemeColorMap->GetColor(Twitter::Metadata::Drawing::BrushBackground, &dwBrushColor));
		ASSERT_IF_FAILED(pThemeColorMap->GetColor(Twitter::Metadata::Drawing::PictureWindowText, &dwTextColor));
	}

	cdc.SetBkMode(TRANSPARENT);
	cdc.SetTextColor(dwTextColor);

	cdc.FillSolidRect(&rect, dwBrushColor);

	CComBSTR str = L"Launching video player...";
	if (!m_strLastErrorMsg.IsEmpty())
		str = m_strLastErrorMsg;

	CSize size;
	CRect rect1 = rect;
	cdc.DrawTextEx(str, str.Length(), &rect1, DT_WORDBREAK | DT_CENTER | DT_CALCRECT, NULL);
	size.cx = rect1.Width();
	size.cy = rect1.Height();

	auto x = (rect.right - rect.left) / 2 - (size.cx / 2);
	auto y = (rect.bottom - rect.top) / 2 - (size.cy / 2);

	CRect rectText(x, y, x + size.cx, y + size.cy);
	DrawRoundedRect(cdc, rectText, false);
	cdc.DrawTextEx(str, str.Length(), &rectText, DT_WORDBREAK | DT_CENTER, NULL);

	EndPaint(&ps);
	return 0;
}

STDMETHODIMP CVideoViewControl::SetTheme(ITheme *pTheme)
{
	CHECK_E_POINTER(pTheme);
	m_pTheme = pTheme;
	return S_OK;
}