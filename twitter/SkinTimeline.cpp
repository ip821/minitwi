// SkinTimeline.cpp : Implementation of CSkinTimeline

#include "stdafx.h"
#include "SkinTimeline.h"

// CSkinTimeline

STDMETHODIMP CSkinTimeline::DrawItem(HWND hwndControl, IVariantObject* pItemObject, TDRAWITEMSTRUCT* lpdi, int iHoveredItem, int iHoveredColumn)
{
	CListBox wndListBox(hwndControl);
	auto selectedItemId = wndListBox.GetCurSel();

	Gdiplus::Font nfont(Gdiplus::FontFamily::GenericSansSerif(), 10, Gdiplus::FontStyle::FontStyleBold);
	Gdiplus::Font nfontu(Gdiplus::FontFamily::GenericSansSerif(), 10, Gdiplus::FontStyle::FontStyleBold | Gdiplus::FontStyle::FontStyleUnderline);
	Gdiplus::Font tfont(Gdiplus::FontFamily::GenericSansSerif(), 10, Gdiplus::FontStyle::FontStyleRegular);

	Gdiplus::Graphics gfx(lpdi->hDC);

	auto x = lpdi->rcItem.left;
	auto y = lpdi->rcItem.top;
	auto width = (lpdi->rcItem.right - lpdi->rcItem.left);
	auto height = (lpdi->rcItem.bottom - lpdi->rcItem.top);

	auto strItemId = boost::lexical_cast<std::wstring>(lpdi->itemID);

	CComVariant vName;
	pItemObject->GetVariantValue(VAR_NAME, &vName);
	CString strName;
	if (vName.vt == VT_BSTR)
		strName = vName.bstrVal;

	CComVariant vText;
	pItemObject->GetVariantValue(VAR_TEXT, &vText);
	CString strText;
	if (vText.vt == VT_BSTR)
		strText = vText.bstrVal;

	Gdiplus::SolidBrush brushSteelBlue(Gdiplus::Color::SteelBlue);
	Gdiplus::SolidBrush brushBlack(Gdiplus::Color::Black);

	RECT clientRect = { 0 };
	wndListBox.GetClientRect(&clientRect);

	if (lpdi->itemState & ODS_SELECTED)
	{
		Gdiplus::Pen pen(Gdiplus::Color::SteelBlue, 0.4f);

		gfx.DrawRectangle(&pen, x + 4, y + 4, width - 8, height - 8);

		Gdiplus::SolidBrush brushLightSteelBlue(Gdiplus::Color::LightSteelBlue);
		gfx.FillRectangle(&brushLightSteelBlue, x + 5, y + 5, width - 9, height - 9);
	}
	else
	{
		Gdiplus::SolidBrush brushWhite(Gdiplus::Color::White);
		gfx.FillRectangle(&brushWhite, x + 4, y + 4, width - 7, height - 7);
		gfx.FillRectangle(&brushSteelBlue, x + 4, y + height - 8, width - 8, 1);
	}
	gfx.DrawString(strItemId.c_str(), strItemId.length(), &nfont, Gdiplus::PointF(x + 8, y + 10), &brushBlack);
	Gdiplus::RectF rectNumber;
	auto status = gfx.MeasureString(strItemId.c_str(), strItemId.length(), &nfont, Gdiplus::PointF(0, 0), &rectNumber);

	Gdiplus::Font* pfont = &nfont;
	if (iHoveredItem == lpdi->itemID && iHoveredColumn == 0) //VAR_NAME
	{
		pfont = &nfontu;
	}

	gfx.DrawString(strName, strName.GetLength(), pfont, Gdiplus::PointF(x + 8 + rectNumber.Width + 8, y + 10), &brushSteelBlue);
	Gdiplus::RectF rectName;
	status = gfx.MeasureString(strName, strName.GetLength(), pfont, Gdiplus::PointF(0, 0), &rectName);

	Gdiplus::RectF rect(x + 8 + rectNumber.Width + 8, y + 10 + rectNumber.Height + 12, (clientRect.right - clientRect.left) - 28, (clientRect.bottom - clientRect.top));
	gfx.DrawString(strText, strText.GetLength(), &tfont, rect, &Gdiplus::StringFormat(Gdiplus::StringFormatFlags::StringFormatFlagsNoFitBlackBox), &brushBlack);
	return S_OK;
}

STDMETHODIMP CSkinTimeline::MeasureItem(HWND hwndControl, IVariantObject* pItemObject, TMEASUREITEMSTRUCT* lpMeasureItemStruct, IColumnRects* pColumnRects)
{
	pColumnRects->Clear();
	CListBox wndListBox(hwndControl);

	Gdiplus::Font nfont(Gdiplus::FontFamily::GenericSansSerif(), 10, Gdiplus::FontStyle::FontStyleBold);
	Gdiplus::Font tfont(Gdiplus::FontFamily::GenericSansSerif(), 10, Gdiplus::FontStyle::FontStyleRegular);

	CComVariant vName;
	pItemObject->GetVariantValue(VAR_NAME, &vName);
	CString strName;
	if (vName.vt == VT_BSTR)
		strName = vName.bstrVal;

	CComVariant vText;
	pItemObject->GetVariantValue(VAR_TEXT, &vText);
	CString strText;
	if (vText.vt == VT_BSTR)
		strText = vText.bstrVal;

	auto strItemId = boost::lexical_cast<std::wstring>(lpMeasureItemStruct->itemID);

	Gdiplus::Graphics gfx(hwndControl);

	Gdiplus::RectF rectNumber;
	auto status = gfx.MeasureString(strItemId.c_str(), strItemId.length(), &nfont, Gdiplus::PointF(0, 0), &rectNumber);
	Gdiplus::RectF rectName;
	status = gfx.MeasureString(strName, strName.GetLength(), &nfont, Gdiplus::PointF(0, 0), &rectName);

	pColumnRects->AddRect(CRect(rectName.X + 8 + rectNumber.Width + 8, rectName.Y + 10, rectName.X + 8 + rectNumber.Width + 8 + rectName.Width, rectName.Y + 10 + rectName.Height));

	RECT clientRect = { 0 };
	wndListBox.GetClientRect(&clientRect);

	Gdiplus::SizeF sizeText;
	status = gfx.MeasureString(
		strText,
		strText.GetLength(),
		&tfont,
		Gdiplus::SizeF((clientRect.right - clientRect.left) - 8 - (int)rectNumber.Width - 28, 255),
		&Gdiplus::StringFormat(Gdiplus::StringFormatFlags::StringFormatFlagsNoFitBlackBox),
		&sizeText
		);

	lpMeasureItemStruct->itemHeight = min(255, 10 + rectNumber.Height + 12 + sizeText.Height + 8 + 8);
	lpMeasureItemStruct->itemWidth = sizeText.Width;
	return S_OK;
}
