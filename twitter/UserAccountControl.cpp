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

STDMETHODIMP CUserAccountControl::SetTheme(ITheme* pTheme)
{
	CHECK_E_POINTER(pTheme);
	m_pTheme = pTheme;
	RETURN_IF_FAILED(m_pTheme->GetColorMap(&m_pThemeColorMap));
	RETURN_IF_FAILED(m_pTheme->GetFontMap(&m_pThemeFontMap));
	return S_OK;
}

LRESULT CUserAccountControl::OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DrawBackground((HDC)wParam);
	return 0;
}

LRESULT CUserAccountControl::OnPrintClient(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CRect rect;
	GetClientRect(&rect);
	DrawContents((HDC)wParam, &rect);
	return 0;
}

LRESULT CUserAccountControl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps = { 0 };
	BeginPaint(&ps);

	CRect rect = ps.rcPaint;
	CDC cdc(ps.hdc);

	DrawContents(cdc, rect);

	EndPaint(&ps);
	return 0;
}

void CUserAccountControl::DrawBackground(HDC hdc)
{
	if (!m_pVariantObject)
		return;

	DWORD dwBackColor = 0;
	CBitmap bitmap;
	TBITMAP tBitmap = { 0 };

	CComVariant vBackColor;
	m_pVariantObject->GetVariantValue(VAR_TWITTER_USER_BACKCOLOR, &vBackColor);
	if (vBackColor.vt == VT_I4)
		dwBackColor = Color(vBackColor.intVal).ToCOLORREF();

	CComVariant vBitmapUrl;
	m_pVariantObject->GetVariantValue(VAR_TWITTER_USER_BANNER, &vBitmapUrl);
	if (vBitmapUrl.vt == VT_BSTR)
	{
		m_pImageManagerService->CreateImageBitmap(vBitmapUrl.bstrVal, &bitmap.m_hBitmap);
		m_pImageManagerService->GetImageInfo(vBitmapUrl.bstrVal, &tBitmap);
	}

	CRect rect;
	GetClientRect(&rect);

	CDCHandle cdc(hdc);

	if (bitmap.IsNull())
	{
		CRect rect;
		GetClientRect(&rect);
		CBrush brush;
		brush.CreateSolidBrush(dwBackColor);
		cdc.FillRect(&rect, brush);
	}
	else
	{
		CDC cdcBitmap;
		cdcBitmap.CreateCompatibleDC(cdc);
		cdcBitmap.SelectBitmap(bitmap);

		BLENDFUNCTION bf = { 0 };
		bf.BlendOp = AC_SRC_OVER;
		bf.SourceConstantAlpha = 255;
		cdc.AlphaBlend(0, 0, rect.Width(), rect.Height(), cdcBitmap, 0, 0, tBitmap.Width, tBitmap.Height, bf);
	}
}

void CUserAccountControl::DrawContents(HDC hdc, LPRECT lpRect)
{
	CRect rect = *lpRect;
	CDCHandle cdc(hdc);
	DWORD dwColor = 0;
	CComVariant vForeColor;
	m_pVariantObject->GetVariantValue(VAR_TWITTER_USER_FORECOLOR, &vForeColor);
	if (vForeColor.vt == VT_I4)
		dwColor = Color(vForeColor.intVal).ToCOLORREF();

	cdc.SetBkMode(TRANSPARENT);
	cdc.SetTextColor(dwColor);

	CComVariant vDisplayName;
	m_pVariantObject->GetVariantValue(VAR_TWITTER_USER_DISPLAY_NAME, &vDisplayName);
	if (vDisplayName.vt == VT_BSTR)
	{
		HFONT font = 0;
		ASSERT_IF_FAILED(m_pThemeFontMap->GetFont(VAR_TWITTER_USER_DISPLAY_NAME_USER_ACCOUNT, &font));
		cdc.SelectFont(font);
		CComBSTR bstr(vDisplayName.bstrVal);
		CSize sz;
		cdc.GetTextExtent(bstr, bstr.Length(), &sz);
		CRect rectText;
		rectText.left = rect.Width() / 2 - sz.cx / 2;
		rectText.top = 30;
		rectText.right = rectText.left + sz.cx;
		rectText.bottom = rectText.top + sz.cy;
		cdc.DrawText(bstr, bstr.Length(), &rectText, 0);
	}
}
