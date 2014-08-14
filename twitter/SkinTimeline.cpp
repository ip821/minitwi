// SkinTimeline.cpp : Implementation of CSkinTimeline

#include "stdafx.h"
#include "SkinTimeline.h"
#include "Plugins.h"

// CSkinTimeline

#define COL_NAME_LEFT 16
#define COLUMN_Y_SPACING 5
#define ITEM_SPACING 30

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
		gfx.FillRectangle(&brush, x, y, width, height - ITEM_SPACING + COLUMN_Y_SPACING);
	}
	else
	{
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(VAR_BRUSH_BACKGROUND, &dwColor));
		Gdiplus::SolidBrush brush(dwColor);
		gfx.FillRectangle(&brush, x, y, width, height - ITEM_SPACING + COLUMN_Y_SPACING);
	}

	UINT uiCount = 0;
	pColumnRects->GetCount(&uiCount);
	for (size_t i = 0; i < uiCount; i++)
	{
		RECT rect = { 0 };
		pColumnRects->GetRect(i ,&rect);
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

STDMETHODIMP CSkinTimeline::MeasureItem(HWND hwndControl, IVariantObject* pItemObject, TMEASUREITEMSTRUCT* lpMeasureItemStruct, IColumnRects* pColumnRects)
{
	pColumnRects->Clear();
	CListBox wndListBox(hwndControl);

	CComVariant vName;
	pItemObject->GetVariantValue(VAR_NAME, &vName);
	CString strName;
	if (vName.vt == VT_BSTR)
		strName = vName.bstrVal + CString(L" "); //measure item bug

	CComVariant vText;
	pItemObject->GetVariantValue(VAR_TEXT, &vText);
	CString strText;
	if (vText.vt == VT_BSTR)
		strText = vText.bstrVal;

	Gdiplus::Graphics gfx(hwndControl);

	RECT clientRect = { 0 };
	wndListBox.GetClientRect(&clientRect);

	Gdiplus::SizeF sizeName;
	gfx.MeasureString(
		strName, 
		strName.GetLength(), 
		m_pFonts[VAR_NAME], 
		Gdiplus::SizeF((clientRect.right - clientRect.left) - COL_NAME_LEFT, 255),
		&Gdiplus::StringFormat(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap | Gdiplus::StringFormatFlags::StringFormatFlagsMeasureTrailingSpaces),
		&sizeName
		);

	UINT uiIndex = 0;
	{
		auto x = COL_NAME_LEFT;
		auto y = 0;
		pColumnRects->AddRect(CRect(x, y, x + sizeName.Width, y + sizeName.Height), &uiIndex);
		pColumnRects->SetRectProp(uiIndex, VAR_COLUMN_NAME, CComBSTR(VAR_NAME));
		pColumnRects->SetRectProp(uiIndex, VAR_TEXT, CComBSTR(strName));
		pColumnRects->SetRectProp(uiIndex, VAR_VALUE, CComBSTR(strName));
		pColumnRects->SetRectProp(uiIndex, VAR_IS_URL, CComBSTR(L"1"));
	}

	Gdiplus::SizeF sizeText;
	gfx.MeasureString(
		strText,
		strText.GetLength(),
		m_pFonts[VAR_TEXT],
		Gdiplus::SizeF((clientRect.right - clientRect.left) - COL_NAME_LEFT, 255),
		&Gdiplus::StringFormat(Gdiplus::StringFormatFlags::StringFormatFlagsNoFitBlackBox),
		&sizeText
		);

	{
		auto x = COL_NAME_LEFT;
		auto y = sizeName.Height + COLUMN_Y_SPACING;

		pColumnRects->AddRect(CRect(x, y, x + sizeText.Width, y + sizeText.Height), &uiIndex);
		pColumnRects->SetRectProp(uiIndex, VAR_COLUMN_NAME, CComBSTR(VAR_TEXT));
		pColumnRects->SetRectProp(uiIndex, VAR_TEXT, CComBSTR(strText));
		pColumnRects->SetRectProp(uiIndex, VAR_VALUE, CComBSTR(strText));
	}

	lpMeasureItemStruct->itemHeight = min(255, sizeName.Height + sizeText.Height + ITEM_SPACING);
	lpMeasureItemStruct->itemWidth = sizeText.Width;
	return S_OK;
}
