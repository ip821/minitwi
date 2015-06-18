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

STDMETHODIMP CLayoutPainter::PaintLayout(HDC hdc, POINT* ptOrigin, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo)
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
		switch (elementType)
		{
			case ElementType::HorizontalContainer:
			{
				RETURN_IF_FAILED(PaintContainer(hdc, ptOrigin, pColumnInfoItem));
				CComPtr<IColumnsInfo> pChildItems;
				RETURN_IF_FAILED(pColumnInfoItem->GetChildItems(&pChildItems));

				CRect rect;
				RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));
				CPoint pt(ptOrigin->x + rect.left, ptOrigin->y + rect.top);

				RETURN_IF_FAILED(PaintLayout(hdc, &pt, pImageManagerService, pChildItems));
				break;
			}
			case ElementType::TextColumn:
				RETURN_IF_FAILED(PaintTextColumn(hdc, ptOrigin, pColumnInfoItem));
				break;
			case ElementType::ImageColumn:
				RETURN_IF_FAILED(PaintImageColumn(hdc, ptOrigin, pImageManagerService, pColumnInfoItem));
				break;
		}
	}
	return S_OK;
}

STDMETHODIMP CLayoutPainter::PaintContainer(HDC hdc, POINT* ptOrigin, IColumnsInfoItem* pColumnInfoItem)
{
	CComBSTR vBorderBottom;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Twitter::Themes::Metadata::Element::BorderBottom, &vBorderBottom));
	if (vBorderBottom != L"")
	{
		CComBSTR vBorderBottomColor;
		RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Twitter::Themes::Metadata::Element::BorderBottomColor, &vBorderBottomColor));
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(vBorderBottomColor, &dwColor));
		CBrush brush;
		brush.CreateSolidBrush(dwColor);
		CRect rect;
		RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));
		rect.OffsetRect(*ptOrigin);
		CComBSTR vBorderBottomWidth;
		RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Twitter::Themes::Metadata::Element::BorderBottomWidth, &vBorderBottomWidth));
		rect.top = rect.bottom - _wtoi(vBorderBottomWidth);
		FillRect(hdc, &rect, brush);
	}
	return S_OK;
}

STDMETHODIMP CLayoutPainter::PaintTextColumn(HDC hdc, POINT* ptOrigin, IColumnsInfoItem* pColumnInfoItem)
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
	rect.OffsetRect(*ptOrigin);
	DrawText(cdc, bstr, bstr.Length(), &rect, 0);

	return S_OK;
}

STDMETHODIMP CLayoutPainter::PaintImageColumn(HDC hdc, POINT* ptOrigin, IImageManagerService* pImageManagerService, IColumnsInfoItem* pColumnInfoItem)
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
	rect.OffsetRect(*ptOrigin);
	static Gdiplus::Color color(Gdiplus::Color::Transparent);
	auto res = TransparentBlt(hdc, rect.left, rect.top, bitmapInfo.Width, bitmapInfo.Height, cdcBitmap, 0, 0, bitmapInfo.Width, bitmapInfo.Height, color.ToCOLORREF());
	if (!res)
		return HRESULT_FROM_WIN32(GetLastError());
	return S_OK;
}
