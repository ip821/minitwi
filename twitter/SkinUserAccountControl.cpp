#include "stdafx.h"
#include "SkinUserAccountControl.h"


STDMETHODIMP CSkinUserAccountControl::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	CHECK_E_POINTER(pThemeColorMap);
	m_pThemeColorMap = pThemeColorMap;
	return S_OK;
}

STDMETHODIMP CSkinUserAccountControl::SetFontMap(IThemeFontMap* pThemeFontMap)
{
	CHECK_E_POINTER(pThemeFontMap);
	m_pThemeFontMap = pThemeFontMap;
	return S_OK;
}

STDMETHODIMP CSkinUserAccountControl::SetImageManagerService(IImageManagerService* pImageManagerService)
{
	CHECK_E_POINTER(pImageManagerService);
	m_pImageManagerService = pImageManagerService;
	return S_OK;
}

STDMETHODIMP CSkinUserAccountControl::EraseBackground(HDC hdc, LPRECT lpRect, IVariantObject* pVariantObject)
{
	CRect rect = *lpRect;
	DWORD dwBackColor = 0;
	CBitmap bitmap;
	TBITMAP tBitmap = { 0 };

	CComVariant vBackColor;
	pVariantObject->GetVariantValue(VAR_TWITTER_USER_BACKCOLOR, &vBackColor);
	if (vBackColor.vt == VT_I4)
		dwBackColor = Color(vBackColor.intVal).ToCOLORREF();

	CComVariant vBitmapUrl;
	pVariantObject->GetVariantValue(VAR_TWITTER_USER_BANNER, &vBitmapUrl);
	if (vBitmapUrl.vt == VT_BSTR)
	{
		m_pImageManagerService->CreateImageBitmap(vBitmapUrl.bstrVal, &bitmap.m_hBitmap);
		m_pImageManagerService->GetImageInfo(vBitmapUrl.bstrVal, &tBitmap);
	}

	CDCHandle cdc(hdc);

	if (bitmap.IsNull())
	{
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
	return S_OK;
}

STDMETHODIMP CSkinUserAccountControl::Draw(HDC hdc, LPRECT lpRect, IVariantObject* pVariantObject)
{
	CRect rect = *lpRect;
	CDCHandle cdc(hdc);
	DWORD dwColor = 0;
	CComVariant vForeColor;
	pVariantObject->GetVariantValue(VAR_TWITTER_USER_FORECOLOR, &vForeColor);
	if (vForeColor.vt == VT_I4)
		dwColor = Color(vForeColor.intVal).ToCOLORREF();

	cdc.SetBkMode(TRANSPARENT);
	cdc.SetTextColor(dwColor);

	CComVariant vDisplayName;
	pVariantObject->GetVariantValue(VAR_TWITTER_USER_DISPLAY_NAME, &vDisplayName);
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
	return S_OK;
}
