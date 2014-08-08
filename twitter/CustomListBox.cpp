#include "stdafx.h"
#include "CustomListBox.h"

LRESULT CCustomListBox::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT CCustomListBox::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (!m_hWnd)
		return 0;

	auto itemCount = GetCount();
	for (size_t i = 0; i < itemCount; i++)
	{
		MEASUREITEMSTRUCT s = { 0 };
		s.itemID = i;
		MeasureItem(&s);
		SetItemHeight(i, s.itemHeight);
	}
	return 0;
}

void CCustomListBox::DrawItem(LPDRAWITEMSTRUCT lpdi)
{
	if (lpdi->itemID == -1)
		return;

	auto selectedItemId = GetCurSel();

	Gdiplus::Graphics gfx(lpdi->hDC);

	auto x = lpdi->rcItem.left;
	auto y = lpdi->rcItem.top;
	auto width = (lpdi->rcItem.right - lpdi->rcItem.left);
	auto height = (lpdi->rcItem.bottom - lpdi->rcItem.top);

	Gdiplus::FontFamily fontFamily(L"Microsoft Sans Serif");
	Gdiplus::Font nfont(&fontFamily, 10, Gdiplus::FontStyle::FontStyleBold);
	Gdiplus::Font tfont(&fontFamily, 10, Gdiplus::FontStyle::FontStyleRegular);

	auto strItemId = boost::lexical_cast<std::wstring>(lpdi->itemID);

	ItemData& itemData = m_items[lpdi->itemID];

	Gdiplus::SolidBrush brushSteelBlue(Gdiplus::Color::SteelBlue);
	Gdiplus::SolidBrush brushBlack(Gdiplus::Color::Black);

	RECT clientRect = { 0 };
	GetClientRect(&clientRect);

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

	gfx.DrawString(itemData.strName, itemData.strName.GetLength(), &nfont, Gdiplus::PointF(x + 8 + rectNumber.Width + 8, y + 10), &brushSteelBlue);
	Gdiplus::RectF rectName;
	status = gfx.MeasureString(itemData.strName, itemData.strName.GetLength(), &nfont, Gdiplus::PointF(0, 0), &rectName);

	Gdiplus::RectF rect(x + 8 + rectNumber.Width + 8, y + 10 + rectNumber.Height + 12, (clientRect.right - clientRect.left) - 28, (clientRect.bottom - clientRect.top));
	gfx.DrawString(itemData.strText, itemData.strText.GetLength(), &tfont, rect, &Gdiplus::StringFormat(Gdiplus::StringFormatFlags::StringFormatFlagsNoFitBlackBox), &brushBlack);
}

void CCustomListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	Gdiplus::FontFamily fontFamily(L"Microsoft Sans Serif");
	Gdiplus::Font nfont(&fontFamily, 10, Gdiplus::FontStyle::FontStyleBold);
	Gdiplus::Font tfont(&fontFamily, 10, Gdiplus::FontStyle::FontStyleRegular);

	ItemData& itemData = m_items[lpMeasureItemStruct->itemID];
	auto strItemId = boost::lexical_cast<std::wstring>(lpMeasureItemStruct->itemID);

	Gdiplus::Graphics gfx(m_hWnd);

	Gdiplus::RectF rectNumber;
	auto status = gfx.MeasureString(strItemId.c_str(), strItemId.length(), &nfont, Gdiplus::PointF(0, 0), &rectNumber);
	Gdiplus::RectF rectName;
	status = gfx.MeasureString(itemData.strName, itemData.strName.GetLength(), &nfont, Gdiplus::PointF(0, 0), &rectName);

	RECT clientRect = { 0 };
	GetClientRect(&clientRect);

	Gdiplus::SizeF sizeText;
	status = gfx.MeasureString(
		itemData.strText,
		itemData.strText.GetLength(),
		&tfont,
		Gdiplus::SizeF((clientRect.right - clientRect.left) - 8 - (int)rectNumber.Width - 28, 255),
		&Gdiplus::StringFormat(Gdiplus::StringFormatFlags::StringFormatFlagsNoFitBlackBox),
		&sizeText
		);

	lpMeasureItemStruct->itemHeight = min(255, 10 + rectNumber.Height + 12 + sizeText.Height + 8 + 8);
	lpMeasureItemStruct->itemWidth = sizeText.Width;
}

void CCustomListBox::AddItem(ItemData& itemData)
{
	m_items.push_back(itemData);
	auto itemId = SendMessage(LB_ADDSTRING, 0, (LPARAM)1);
}

