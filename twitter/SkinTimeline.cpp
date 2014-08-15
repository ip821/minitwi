// SkinTimeline.cpp : Implementation of CSkinTimeline

#include "stdafx.h"
#include "SkinTimeline.h"
#include "Plugins.h"

// CSkinTimeline

#define COL_NAME_LEFT 16
#define COLUMN_X_SPACING 5
#define COLUMN_Y_SPACING 5
#define ITEM_SPACING 30
#define ITEM_DELIMITER_HEIGHT 1

#define FONT_NAME L"Microsoft Sans Serif"
#define FONT_SIZE 100

CSkinTimeline::CSkinTimeline()
{
	m_FontNormal.CreatePointFont(FONT_SIZE, FONT_NAME);

	{
		LOGFONT logFontNormalUnderlined = { 0 };
		m_FontNormal.GetLogFont(logFontNormalUnderlined);
		logFontNormalUnderlined.lfHeight = FONT_SIZE;
		logFontNormalUnderlined.lfUnderline = TRUE;
		m_FontNormalUnderlined.CreatePointFontIndirect(&logFontNormalUnderlined);
	}

	m_FontBold.CreatePointFont(FONT_SIZE, FONT_NAME, 0, true);

	{
		LOGFONT logFontBoldUnderlined = { 0 };
		m_FontBold.GetLogFont(logFontBoldUnderlined);
		logFontBoldUnderlined.lfHeight = FONT_SIZE;
		logFontBoldUnderlined.lfUnderline = TRUE;
		m_FontBoldUnderlined.CreatePointFontIndirect(&logFontBoldUnderlined);
	}

	m_pFonts[VAR_TWITTER_USER_DISPLAY_NAME] = m_FontBold;
	m_pFonts[VAR_TWITTER_USER_NAME] = m_FontNormal;
	m_pFonts[VAR_TWITTER_USER_NAME + CString(VAR_SELECTED_POSTFIX)] = m_FontNormalUnderlined;
	m_pFonts[CString(VAR_TWITTER_USER_DISPLAY_NAME) + CString(VAR_SELECTED_POSTFIX)] = m_FontBoldUnderlined;
	m_pFonts[VAR_TWITTER_NORMALIZED_TEXT] = m_FontNormal;
	m_pFonts[VAR_TWITTER_URL] = m_FontNormal;
	m_pFonts[VAR_TWITTER_URL + CString(VAR_SELECTED_POSTFIX)] = m_FontNormalUnderlined;
}

STDMETHODIMP CSkinTimeline::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	m_pThemeColorMap = pThemeColorMap;
	return S_OK;
}

STDMETHODIMP CSkinTimeline::DrawItem(HWND hwndControl, IColumnRects* pColumnRects, TDRAWITEMSTRUCT* lpdi, int iHoveredItem, int iHoveredColumn)
{
	CListBox wndListBox(hwndControl);
	auto selectedItemId = wndListBox.GetCurSel();

	RECT clientRect = { 0 };
	wndListBox.GetClientRect(&clientRect);

	::SetBkMode(lpdi->hDC, TRANSPARENT);

	if (lpdi->itemState & ODS_SELECTED)
	{
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(VAR_BRUSH_SELECTED, &dwColor));
		CBrush brush;
		brush.CreateSolidBrush(dwColor);
		FillRect(lpdi->hDC, &(lpdi->rcItem), brush);
	}
	else
	{
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(VAR_BRUSH_BACKGROUND, &dwColor));
		CBrush brush;
		brush.CreateSolidBrush(dwColor);
		RECT rect = lpdi->rcItem;
		rect.bottom -= COLUMN_Y_SPACING;
		FillRect(lpdi->hDC, &rect, brush);
	}

	{
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(VAR_TWITTER_DELIMITER, &dwColor));
		CBrush brush;
		brush.CreateSolidBrush(dwColor);
		RECT rect = lpdi->rcItem;
		rect.bottom = rect.top + ITEM_DELIMITER_HEIGHT;
		FillRect(lpdi->hDC, &rect, brush);
	}

	UINT uiCount = 0;
	pColumnRects->GetCount(&uiCount);
	for (size_t i = 0; i < uiCount; i++)
	{
		RECT rect = { 0 };
		pColumnRects->GetRect(i, &rect);
		CComBSTR bstrColumnName;
		pColumnRects->GetRectProp(i, VAR_COLUMN_NAME, &bstrColumnName);
		CComBSTR bstrText;
		pColumnRects->GetRectProp(i, VAR_TEXT, &bstrText);
		CComBSTR bstrIsUrl;
		pColumnRects->GetRectProp(i, VAR_IS_URL, &bstrIsUrl);
		CComBSTR bstrIsWordWrap;
		pColumnRects->GetRectProp(i, VAR_IS_WORDWRAP, &bstrIsWordWrap);

		CFontHandle font = m_pFonts[CString(bstrColumnName)];
		if (iHoveredItem == lpdi->itemID && iHoveredColumn == i && bstrIsUrl == L"1")
		{
			font = m_pFonts[CString(bstrColumnName) + VAR_SELECTED_POSTFIX];
		}

		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(bstrColumnName, &dwColor));
		::SetTextColor(lpdi->hDC, dwColor);

		auto x = lpdi->rcItem.left;
		auto y = lpdi->rcItem.top;

		CString str(bstrText);
		::SelectObject(lpdi->hDC, font);
		RECT rectText = { x + rect.left, y + rect.top, x + rect.right, y + rect.bottom };
		DrawText(lpdi->hDC, str, str.GetLength(), &rectText, bstrIsWordWrap == L"1" ? DT_WORDBREAK : 0);
	}

	return S_OK;
}

SIZE CSkinTimeline::AddColumn(
	HDC hdc,
	IColumnRects* pColumnRects,
	CString& strColumnName,
	CString& strDisplayText,
	CString& strValue,
	int x,
	int y,
	SIZE size,
	BOOL bIsUrl = TRUE,
	BOOL bWordWrap = FALSE
	)
{
	CDC cdc;
	cdc.CreateCompatibleDC(hdc);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(cdc, size.cx, size.cy);
	cdc.SelectBitmap(bitmap);

	RECT rect = { 0 };
	rect.right = size.cx;
	rect.bottom = size.cy;
	
	CFontHandle font = m_pFonts[CString(strColumnName)];
	cdc.SelectFont(font);

	SIZE sz = { 0 };
	if (bWordWrap)
	{
		sz.cx = size.cx;
		sz.cy = DrawText(cdc, strDisplayText, strDisplayText.GetLength(), &rect, DT_WORDBREAK);
	}
	else
	{
		GetTextExtentPoint32(cdc, strDisplayText, strDisplayText.GetLength(), &sz);
	}

	UINT uiIndex = 0;
	pColumnRects->AddRect(CRect(x, y, x + sz.cx, y + sz.cy), &uiIndex);
	pColumnRects->SetRectProp(uiIndex, VAR_COLUMN_NAME, CComBSTR(strColumnName));
	pColumnRects->SetRectProp(uiIndex, VAR_TEXT, CComBSTR(strDisplayText));
	pColumnRects->SetRectProp(uiIndex, VAR_VALUE, CComBSTR(strValue));
	if (bWordWrap)
		pColumnRects->SetRectProp(uiIndex, VAR_IS_WORDWRAP, CComBSTR(L"1"));
	if (bIsUrl)
		pColumnRects->SetRectProp(uiIndex, VAR_IS_URL, CComBSTR(L"1"));
	return sz;
}

void CSkinTimeline::GetValue(IVariantObject* pItemObject, CComBSTR& bstrColumnName, CString& strValue)
{
	CComVariant v;
	pItemObject->GetVariantValue(bstrColumnName, &v);
	if (v.vt == VT_BSTR)
		strValue = v.bstrVal + CString(L" "); //measure item bug
}

STDMETHODIMP CSkinTimeline::MeasureItem(HWND hwndControl, IVariantObject* pItemObject, TMEASUREITEMSTRUCT* lpMeasureItemStruct, IColumnRects* pColumnRects)
{
	pColumnRects->Clear();
	CListBox wndListBox(hwndControl);

	HDC hdc = GetDC(hwndControl);

	CString strDisplayName;
	GetValue(pItemObject, CComBSTR(VAR_TWITTER_USER_DISPLAY_NAME), strDisplayName);

	CString strName;
	GetValue(pItemObject, CComBSTR(VAR_TWITTER_USER_NAME), strName);

	CString strText;
	GetValue(pItemObject, CComBSTR(VAR_TWITTER_NORMALIZED_TEXT), strText);

	RECT clientRect = { 0 };
	wndListBox.GetClientRect(&clientRect);

	CSize sizeDislpayName;
	UINT uiIndex = 0;
	{
		auto x = COL_NAME_LEFT;
		auto y = COLUMN_Y_SPACING;

		sizeDislpayName = AddColumn(
			hdc,
			pColumnRects,
			CString(VAR_TWITTER_USER_DISPLAY_NAME),
			strDisplayName,
			strDisplayName,
			x,
			y,
			CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, 255)
			);
	}

	CSize sizeName;
	{
		auto x = COL_NAME_LEFT + sizeDislpayName.cx + COLUMN_X_SPACING;
		auto y = COLUMN_Y_SPACING;

		sizeName = AddColumn(
			hdc,
			pColumnRects,
			CString(VAR_TWITTER_USER_NAME),
			L"@" + strName,
			strName,
			x,
			y,
			CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, 255)
			);
	}

	CSize sizeText;
	if (!strText.IsEmpty())
	{
		auto x = COL_NAME_LEFT;
		auto y = sizeDislpayName.cy + COLUMN_Y_SPACING + COLUMN_Y_SPACING;

		sizeText = AddColumn(
			hdc,
			pColumnRects,
			CString(VAR_TWITTER_NORMALIZED_TEXT),
			strText,
			strText,
			x,
			y,
			CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, 255),
			FALSE,
			TRUE
			);
	}

	CComVariant vUrls;
	pItemObject->GetVariantValue(VAR_TWITTER_URLS, &vUrls);
	if (vUrls.vt == VT_UNKNOWN)
	{
		auto x = COL_NAME_LEFT;
		auto y = sizeDislpayName.cy + COLUMN_Y_SPACING + sizeText.cy + COLUMN_Y_SPACING;

		CComQIPtr<IBstrCollection> pBstrCollection = vUrls.punkVal;
		UINT_PTR uiCount = 0;
		pBstrCollection->GetCount(&uiCount);
		for (size_t i = 0; i < uiCount; i++)
		{
			CComBSTR bstrUrl;
			pBstrCollection->GetItem(i, &bstrUrl);
			auto size = AddColumn(
				hdc,
				pColumnRects,
				CString(VAR_TWITTER_URL),
				CString(bstrUrl),
				CString(bstrUrl),
				x,
				y,
				CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, 255)
				);
			y += size.cy + COLUMN_Y_SPACING;
		}
	}

	lpMeasureItemStruct->itemHeight = min(255, sizeDislpayName.cy + sizeText.cy + ITEM_SPACING);
	lpMeasureItemStruct->itemWidth = sizeText.cx;
	return S_OK;
}
