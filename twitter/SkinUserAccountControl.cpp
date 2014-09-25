#include "stdafx.h"
#include "SkinUserAccountControl.h"

#define DISTANCE_DISPLAY_NAME 30 / 200
#define DISTANCE_DESCRIPTION_Y 20
#define DISTANCE_DESCRIPTION_X 20

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
	ASSERT_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_USER_FORECOLOR, &vForeColor));
	if (vForeColor.vt == VT_I4)
		dwColor = Color(vForeColor.intVal).ToCOLORREF();

	cdc.SetBkMode(TRANSPARENT);
	cdc.SetTextColor(dwColor);

	CComVariant vDisplayName;
	ASSERT_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_USER_DISPLAY_NAME, &vDisplayName));
	ATLASSERT(vDisplayName.vt == VT_BSTR);

	HFONT font = 0;
	ASSERT_IF_FAILED(m_pThemeFontMap->GetFont(VAR_TWITTER_USER_DISPLAY_NAME_USER_ACCOUNT, &font));
	cdc.SelectFont(font);

	CComBSTR bstrDisplayName(vDisplayName.bstrVal);
	CSize szDisplayName;
	cdc.GetTextExtent(bstrDisplayName, bstrDisplayName.Length(), &szDisplayName);
	CRect rectDisplayName;
	rectDisplayName.left = rect.Width() / 2 - szDisplayName.cx / 2;
	rectDisplayName.top = rect.Height() * DISTANCE_DISPLAY_NAME;
	rectDisplayName.right = rectDisplayName.left + szDisplayName.cx;
	rectDisplayName.bottom = rectDisplayName.top + szDisplayName.cy;
	cdc.DrawText(bstrDisplayName, bstrDisplayName.Length(), &rectDisplayName, 0);

	CComVariant vScreenName;
	ASSERT_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_USER_NAME, &vScreenName));
	ATLASSERT(vScreenName.vt == VT_BSTR);
	CComBSTR bstrScreenName(CString(L"@") + vScreenName.bstrVal);
	CSize szScreenName;
	cdc.GetTextExtent(bstrScreenName, bstrScreenName.Length(), &szScreenName);
	CRect rectScreenName;
	rectScreenName.left = rect.Width() / 2 - szScreenName.cx / 2;
	rectScreenName.top = rectDisplayName.bottom + 1;
	rectScreenName.right = rectScreenName.left + szScreenName.cx;
	rectScreenName.bottom = rectScreenName.top + szScreenName.cy;
	cdc.DrawText(bstrScreenName, bstrScreenName.Length(), &rectScreenName, 0);

	ASSERT_IF_FAILED(m_pThemeFontMap->GetFont(VAR_TWITTER_NORMALIZED_TEXT, &font));
	cdc.SelectFont(font);

	CComVariant vDescription;
	ASSERT_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_USER_DESCRIPTION, &vDescription));
	ATLASSERT(vDescription.vt == VT_BSTR);
	CComBSTR bstrDescription(vDescription.bstrVal);
	CSize szDescription;
	szDescription.cx = rect.Width() - DISTANCE_DESCRIPTION_X * 2;
	{
		CBitmap bitmapString;
		bitmapString.CreateCompatibleBitmap(cdc, szDescription.cx, rect.Height());
		CDC cdcString;
		cdcString.CreateCompatibleDC(cdc);
		cdcString.SelectBitmap(bitmapString);
		cdcString.SelectFont(font);
		CRect rect1;
		rect1.right = szDescription.cx;
		rect1.bottom = rect.Height();
		szDescription.cy = cdcString.DrawTextEx(bstrDescription, bstrDescription.Length(), &rect1, DT_WORDBREAK | DT_CENTER, NULL);
	}
	CRect rectDescription;
	rectDescription.left = rect.Width() / 2 - szDescription.cx / 2;
	rectDescription.top = rectScreenName.bottom + DISTANCE_DESCRIPTION_Y;
	rectDescription.right = rectDescription.left + szDescription.cx;
	rectDescription.bottom = rectDescription.top + szDescription.cy;
	cdc.DrawTextEx(bstrDescription, bstrDescription.Length(), &rectDescription, DT_WORDBREAK | DT_CENTER, NULL);

	auto bottom = rect.bottom - DISTANCE_DESCRIPTION_X;
	bottom = DrawCounter(cdc, DISTANCE_DESCRIPTION_X, bottom, rect.Width(), pVariantObject, VAR_TWITTER_USER_FOLLOWERS_COUNT, L"Followers: ");
	bottom = DrawCounter(cdc, DISTANCE_DESCRIPTION_X, bottom, rect.Width(), pVariantObject, VAR_TWITTER_USER_FRIENDS_COUNT, L"Following: ");
	bottom = DrawCounter(cdc, DISTANCE_DESCRIPTION_X, bottom, rect.Width(), pVariantObject, VAR_TWITTER_USER_TWEETS_COUNT, L"Tweets: ");

	return S_OK;
}

int CSkinUserAccountControl::DrawCounter(HDC hdc, int x, int y, int width, IVariantObject* pVariantObject, BSTR bstrName, BSTR bstrMessage)
{
	CComVariant v;
	ASSERT_IF_FAILED(pVariantObject->GetVariantValue(bstrName, &v));
	ATLASSERT(v.vt == VT_I4);

	CString strText = CString(bstrMessage) + boost::lexical_cast<wstring>(v.intVal).c_str();
	CDCHandle cdc(hdc);
	CSize sz;
	cdc.GetTextExtent(strText, strText.GetLength(), &sz);
	CRect rect;
	rect.left = x;
	rect.top = y - sz.cy;
	rect.right = rect.left + width;
	rect.bottom = y;
	cdc.DrawText(strText, strText.GetLength(), &rect, 0);
	return y - sz.cy - 1;
}