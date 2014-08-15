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

CSkinTimeline::CSkinTimeline() :
	m_FontNormal(Gdiplus::FontFamily::GenericSansSerif(), 10, Gdiplus::FontStyle::FontStyleRegular),
	m_FontNormalUnderlined(Gdiplus::FontFamily::GenericSansSerif(), 10, Gdiplus::FontStyle::FontStyleUnderline),
	m_FontBold(Gdiplus::FontFamily::GenericSansSerif(), 10, Gdiplus::FontStyle::FontStyleBold),
	m_FontBoldUnderlined(Gdiplus::FontFamily::GenericSansSerif(), 10, Gdiplus::FontStyle::FontStyleBold | Gdiplus::FontStyle::FontStyleUnderline)
{
	m_pFonts[VAR_TWITTER_USER_DISPLAY_NAME] = &m_FontBold;
	m_pFonts[VAR_TWITTER_USER_NAME] = &m_FontNormal;
	m_pFonts[VAR_TWITTER_USER_NAME + CString(VAR_SELECTED_POSTFIX)] = &m_FontNormalUnderlined;
	m_pFonts[CString(VAR_TWITTER_USER_DISPLAY_NAME) + CString(VAR_SELECTED_POSTFIX)] = &m_FontBoldUnderlined;
	m_pFonts[VAR_TWITTER_TEXT] = &m_FontNormal;
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

	Gdiplus::Graphics gfx(lpdi->hDC);

	auto x = lpdi->rcItem.left;
	auto y = lpdi->rcItem.top;
	auto width = (lpdi->rcItem.right - lpdi->rcItem.left);
	auto height = (lpdi->rcItem.bottom - lpdi->rcItem.top);

	RECT clientRect = { 0 };
	wndListBox.GetClientRect(&clientRect);

	if (lpdi->itemState & ODS_SELECTED)
	{
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(VAR_BRUSH_SELECTED, &dwColor));
		Gdiplus::SolidBrush brush(dwColor);
		gfx.FillRectangle(&brush, x, y, width, height);
	}
	else
	{
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(VAR_BRUSH_BACKGROUND, &dwColor));
		Gdiplus::SolidBrush brush(dwColor);
		gfx.FillRectangle(&brush, x, y, width, height - COLUMN_Y_SPACING);
	}

	{
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(VAR_TWITTER_DELIMITER, &dwColor));
		Gdiplus::SolidBrush brush(dwColor);
		gfx.FillRectangle(&brush, x, y, width, ITEM_DELIMITER_HEIGHT);
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

		Gdiplus::Font* pfont = m_pFonts[CString(bstrColumnName)];
		if (iHoveredItem == lpdi->itemID && iHoveredColumn == i && bstrIsUrl == L"1")
		{
			pfont = m_pFonts[CString(bstrColumnName) + VAR_SELECTED_POSTFIX];
		}

		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(bstrColumnName, &dwColor));
		Gdiplus::SolidBrush brush(dwColor);

		CString str(bstrText);

		Gdiplus::RectF rectF(x + rect.left, y + rect.top, rect.right - rect.left, rect.bottom - rect.top);
		gfx.DrawString(str, str.GetLength(), pfont, rectF, &Gdiplus::StringFormat(Gdiplus::StringFormatFlags::StringFormatFlagsNoFitBlackBox), &brush);
	}

	return S_OK;
}

Gdiplus::SizeF CSkinTimeline::AddColumn(
	Gdiplus::Graphics& gfx,
	IColumnRects* pColumnRects,
	CString& strColumnName,
	CString& strDisplayText,
	CString& strValue,
	int x,
	int y,
	Gdiplus::SizeF& size,
	BOOL bIsUrl = TRUE,
	int flags = Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap | Gdiplus::StringFormatFlags::StringFormatFlagsMeasureTrailingSpaces
	)
{
	Gdiplus::SizeF sizeName;
	gfx.MeasureString(
		strDisplayText,
		strDisplayText.GetLength(),
		m_pFonts[strColumnName],
		size,
		&Gdiplus::StringFormat(flags),
		&sizeName
		);

	UINT uiIndex = 0;
	pColumnRects->AddRect(CRect(x, y, x + sizeName.Width, y + sizeName.Height), &uiIndex);
	pColumnRects->SetRectProp(uiIndex, VAR_COLUMN_NAME, CComBSTR(strColumnName));
	pColumnRects->SetRectProp(uiIndex, VAR_TEXT, CComBSTR(strDisplayText));
	pColumnRects->SetRectProp(uiIndex, VAR_VALUE, CComBSTR(strValue));
	if (bIsUrl)
		pColumnRects->SetRectProp(uiIndex, VAR_IS_URL, CComBSTR(L"1"));
	return sizeName;
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

	CString strDisplayName;
	GetValue(pItemObject, CComBSTR(VAR_TWITTER_USER_DISPLAY_NAME), strDisplayName);

	CString strName;
	GetValue(pItemObject, CComBSTR(VAR_TWITTER_USER_NAME), strName);

	CString strText;
	GetValue(pItemObject, CComBSTR(VAR_TWITTER_TEXT), strText);

	Gdiplus::Graphics gfx(hwndControl);

	RECT clientRect = { 0 };
	wndListBox.GetClientRect(&clientRect);

	Gdiplus::SizeF sizeDislpayName;
	UINT uiIndex = 0;
	{
		auto x = COL_NAME_LEFT;
		auto y = COLUMN_Y_SPACING;

		sizeDislpayName = AddColumn(
			gfx,
			pColumnRects,
			CString(VAR_TWITTER_USER_DISPLAY_NAME),
			strDisplayName,
			strDisplayName,
			x,
			y,
			Gdiplus::SizeF((clientRect.right - clientRect.left) - COL_NAME_LEFT, 255)
			);
	}

	Gdiplus::SizeF sizeName;
	{
		auto x = COL_NAME_LEFT + sizeDislpayName.Width + COLUMN_X_SPACING;
		auto y = COLUMN_Y_SPACING;

		sizeName = AddColumn(
			gfx,
			pColumnRects,
			CString(VAR_TWITTER_USER_NAME),
			L"@" + strName,
			strName,
			x,
			y,
			Gdiplus::SizeF((clientRect.right - clientRect.left) - COL_NAME_LEFT, 255)
			);
	}

	Gdiplus::SizeF sizeText;
	{
		auto x = COL_NAME_LEFT;
		auto y = sizeDislpayName.Height + COLUMN_Y_SPACING + COLUMN_Y_SPACING;

		sizeText = AddColumn(
			gfx,
			pColumnRects,
			CString(VAR_TWITTER_TEXT),
			strText,
			strText,
			x,
			y,
			Gdiplus::SizeF((clientRect.right - clientRect.left) - COL_NAME_LEFT, 255),
			FALSE,
			Gdiplus::StringFormatFlags::StringFormatFlagsNoFitBlackBox
			);
	}

	lpMeasureItemStruct->itemHeight = min(255, sizeDislpayName.Height + sizeText.Height + ITEM_SPACING);
	lpMeasureItemStruct->itemWidth = sizeText.Width;
	return S_OK;
}
