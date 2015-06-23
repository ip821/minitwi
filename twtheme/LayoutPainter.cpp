#include "stdafx.h"
#include "LayoutPainter.h"
#include "LayoutBuilder.h"

#include "GdilPlusUtils.h"

STDMETHODIMP CLayoutPainter::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	CHECK_E_POINTER(pThemeColorMap);
	m_pThemeColorMap = pThemeColorMap;
	return S_OK;
}

STDMETHODIMP CLayoutPainter::SetFontMap(IThemeFontMap* pThemeFontMap)
{
	CHECK_E_POINTER(pThemeFontMap);
	m_pThemeFontMap = pThemeFontMap;
	return S_OK;
}

STDMETHODIMP CLayoutPainter::EraseBackground(HDC hdc, IColumnsInfo* pColumnInfo)
{
	CHECK_E_POINTER(pColumnInfo);
	UINT uiCount = 0;
	RETURN_IF_FAILED(pColumnInfo->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IColumnsInfoItem> pColumnInfoItem;
		RETURN_IF_FAILED(pColumnInfo->GetItem(i, &pColumnInfoItem));
		CComBSTR bstrType;
		RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Twitter::Themes::Metadata::Element::Type, &bstrType));
		ElementType elementType = ElementType::UnknownValue;
		RETURN_IF_FAILED(CLayoutBuilder::MapType(bstrType, &elementType));
		if (elementType == ElementType::HorizontalContainer)
		{
			CRect rect;
			RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));
			CDCHandle cdc(hdc);
			DWORD dwColor = 0;
			RETURN_IF_FAILED(m_pThemeColorMap->GetColor(Twitter::Metadata::Drawing::BrushBackground, &dwColor));
			CBrush brush;
			brush.CreateSolidBrush(dwColor);
			cdc.FillRect(rect, brush);
			break;
		}
	}
	return S_OK;
}

STDMETHODIMP CLayoutPainter::PaintLayout(HDC hdc, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, BSTR bstrItemName)
{
	CPoint ptOrigin;
	RETURN_IF_FAILED(PaintLayoutInternal(hdc, pImageManagerService, pColumnInfo, bstrItemName));
	return S_OK;
}

STDMETHODIMP CLayoutPainter::PaintLayoutInternal(HDC hdc, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, BSTR bstrItemName)
{
	CHECK_E_POINTER(pColumnInfo);
	UINT uiCount = 0;
	RETURN_IF_FAILED(pColumnInfo->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IColumnsInfoItem> pColumnInfoItem;
		RETURN_IF_FAILED(pColumnInfo->GetItem(i, &pColumnInfoItem));
		BOOL bVisible = FALSE;
		RETURN_IF_FAILED(pColumnInfoItem->GetRectBoolProp(Twitter::Themes::Metadata::Element::Visible, &bVisible));
		if (!bVisible)
			continue;
		CComBSTR bstrType;
		RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Twitter::Themes::Metadata::Element::Type, &bstrType));
		ElementType elementType = ElementType::UnknownValue;
		RETURN_IF_FAILED(CLayoutBuilder::MapType(bstrType, &elementType));
		switch (elementType)
		{
			case ElementType::HorizontalContainer:
			{
				RETURN_IF_FAILED(PaintContainer(hdc, pColumnInfoItem));
				CComPtr<IColumnsInfo> pChildItems;
				RETURN_IF_FAILED(pColumnInfoItem->GetChildItems(&pChildItems));

				CRect rect;
				RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));
				RETURN_IF_FAILED(PaintLayoutInternal(hdc, pImageManagerService, pChildItems, bstrItemName));
				break;
			}
			case ElementType::TextColumn:
				RETURN_IF_FAILED(PaintTextColumn(hdc, pColumnInfoItem));
				break;
			case ElementType::ImageColumn:
				RETURN_IF_FAILED(PaintImageColumn(hdc, pImageManagerService, pColumnInfoItem));
				break;
			case ElementType::MarqueeProgressColumn:
				RETURN_IF_FAILED(PaintMarqueeProgressColumn(hdc, pColumnInfoItem));
				break;
		}
	}
	return S_OK;
}

STDMETHODIMP CLayoutPainter::PaintContainer(HDC hdc, IColumnsInfoItem* pColumnInfoItem)
{
	BOOL bBorderBottom = FALSE;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectBoolProp(Twitter::Themes::Metadata::Element::BorderBottom, &bBorderBottom));
	if (bBorderBottom)
	{
		CComBSTR vBorderBottomColor;
		RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Twitter::Themes::Metadata::Element::BorderBottomColor, &vBorderBottomColor));
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(vBorderBottomColor, &dwColor));
		CBrush brush;
		brush.CreateSolidBrush(dwColor);
		CRect rect;
		RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));
		CComVariant vBorderBottomWidth;
		RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(Twitter::Themes::Metadata::Element::BorderBottomWidth, &vBorderBottomWidth));
		ATLASSERT(vBorderBottomWidth.vt == VT_I4);
		rect.top = rect.bottom - vBorderBottomWidth.intVal;
		FillRect(hdc, &rect, brush);
	}
	return S_OK;
}

STDMETHODIMP CLayoutPainter::PaintTextColumn(HDC hdc, IColumnsInfoItem* pColumnInfoItem)
{
	CDCHandle cdc(hdc);
	CComBSTR bstr;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Twitter::Themes::Metadata::TextColumn::Text, &bstr));

	CComBSTR bstrName;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Twitter::Themes::Metadata::Element::Name, &bstrName));

	CComBSTR bstrFont;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Twitter::Themes::Metadata::TextColumn::Font, &bstrFont));

	HFONT font = 0;
	RETURN_IF_FAILED(m_pThemeFontMap->GetFont(bstrFont, &font));
	CDCSelectFontScope cdcSelectFontScope(cdc, font);

	BOOL bSelected = FALSE;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectBoolProp(Twitter::Themes::Metadata::Element::Selected, &bSelected));

	CComBSTR bstrColor;
	DWORD dwColor = 0;
	if (bSelected)
	{
		RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Twitter::Themes::Metadata::TextColumn::ColorSelected, &bstrColor));
	}
	else
	{
		RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Twitter::Themes::Metadata::TextColumn::Color, &bstrColor));
	}
	RETURN_IF_FAILED(m_pThemeColorMap->GetColor(bstrColor, &dwColor));
	cdc.SetTextColor(dwColor);

	CRect rect;
	RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));
	DrawText(cdc, bstr, bstr.Length(), &rect, 0);

	return S_OK;
}

STDMETHODIMP CLayoutPainter::PaintImageColumn(HDC hdc, IImageManagerService* pImageManagerService, IColumnsInfoItem* pColumnInfoItem)
{
	CComBSTR bstrImageKey;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Twitter::Themes::Metadata::ImageColumn::ImageKey, &bstrImageKey));
	ATLASSERT(bstrImageKey != L"");
	CBitmap bitmap;
	RETURN_IF_FAILED(pImageManagerService->CreateImageBitmap(bstrImageKey, &bitmap.m_hBitmap));

	TBITMAP bitmapInfo = { 0 };
	RETURN_IF_FAILED(pImageManagerService->GetImageInfo(bstrImageKey, &bitmapInfo));
	CDC cdcBitmap;
	cdcBitmap.CreateCompatibleDC(hdc);
	CDCSelectBitmapScope cdcSelectBitmapScope(cdcBitmap, bitmap);

	CRect rect;
	RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));
	static Gdiplus::Color color(Gdiplus::Color::Transparent);
	auto res = TransparentBlt(hdc, rect.left, rect.top, bitmapInfo.Width, bitmapInfo.Height, cdcBitmap, 0, 0, bitmapInfo.Width, bitmapInfo.Height, color.ToCOLORREF());
	if (!res)
		return HRESULT_FROM_WIN32(GetLastError());
	return S_OK;
}

STDMETHODIMP CLayoutPainter::PaintMarqueeProgressColumn(HDC hdc, IColumnsInfoItem* pColumnInfoItem)
{
	CComVariant vValue;
	RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(Twitter::Themes::Metadata::MarqueeProgressColumn::Value, &vValue));
	CComVariant vItemSize;
	RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(Twitter::Themes::Metadata::MarqueeProgressColumn::ItemSize, &vItemSize));
	CComVariant vItemDistance;
	RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(Twitter::Themes::Metadata::MarqueeProgressColumn::ItemDistance, &vItemDistance));
	CComVariant vItemCount;
	RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(Twitter::Themes::Metadata::MarqueeProgressColumn::ItemCount, &vItemCount));

	ATLASSERT(vValue.vt == VT_I4);
	ATLASSERT(vItemSize.vt == VT_I4);
	ATLASSERT(vItemDistance.vt == VT_I4);
	ATLASSERT(vItemCount.vt == VT_I4);

	auto value = vValue.intVal;
	auto itemSize = vItemSize.intVal;
	auto itemDistance = vItemDistance.intVal;
	auto itemCount = vItemCount.intVal;

	CComBSTR bstrActiveColor;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Twitter::Themes::Metadata::MarqueeProgressColumn::ColorActive, &bstrActiveColor));
	CComBSTR bstrInactiveColor;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Twitter::Themes::Metadata::MarqueeProgressColumn::ColorInactive, &bstrInactiveColor));

	DWORD dwActiveColor = 0;
	RETURN_IF_FAILED(m_pThemeColorMap->GetColor(bstrActiveColor, &dwActiveColor));
	DWORD dwInactiveColor = 0;
	RETURN_IF_FAILED(m_pThemeColorMap->GetColor(bstrInactiveColor, &dwInactiveColor));

	CBrush brushActive;
	brushActive.CreateSolidBrush(dwActiveColor);
	CBrush brushInactive;
	brushInactive.CreateSolidBrush(dwInactiveColor);

	CRect rect;
	RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));
	for (int i = 0; i < itemCount; i++)
	{
		auto x = rect.left + itemSize * i + itemDistance * (max(0, i));
		auto y = rect.top;
		CRect rectItem = { (int)x, y, (int)x + itemSize, y + itemSize };
		FillRect(hdc, rectItem, i == value ? brushActive : brushInactive);
	}
	return S_OK;
}