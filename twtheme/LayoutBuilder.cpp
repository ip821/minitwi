#include "stdafx.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"

STDMETHODIMP CLayoutBuilder::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	CHECK_E_POINTER(pThemeColorMap);
	m_pThemeColorMap = pThemeColorMap;
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::SetFontMap(IThemeFontMap* pThemeFontMap)
{
	CHECK_E_POINTER(pThemeFontMap);
	m_pThemeFontMap = pThemeFontMap;
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::BuildLayout(HDC hdc, RECT* pSourceRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo)
{
	CRect rect;
	RETURN_IF_FAILED(BuildHorizontalContainer(hdc, pSourceRect, &rect, pLayoutObject, pValueObject, pImageManagerService, pColumnInfo));
	CPoint pt;
	RETURN_IF_FAILED(TranslateRects(&pt, pColumnInfo));
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::TranslateRects(POINT* ptOrigin, IColumnsInfo* pColumnsInfo)
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(pColumnsInfo->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(pColumnsInfo->GetItem(i, &pColumnsInfoItem));
		CRect rect;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRect(&rect));
		rect.OffsetRect(*ptOrigin);
		RETURN_IF_FAILED(pColumnsInfoItem->SetRect(rect));
		CComPtr<IColumnsInfo> pChildren;
		RETURN_IF_FAILED(pColumnsInfoItem->GetChildItems(&pChildren));
		CPoint pt(rect.left, rect.top);
		RETURN_IF_FAILED(TranslateRects(&pt, pChildren));
	}
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::GetElements(IVariantObject* pVariantObject, IObjArray** ppObjArray)
{
	CHECK_E_POINTER(pVariantObject);
	CHECK_E_POINTER(ppObjArray);
	CComVar vElements;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Themes::Metadata::Element::Elements, &vElements));
	ATLASSERT(vElements.vt == VT_UNKNOWN);
	RETURN_IF_FAILED(vElements.punkVal->QueryInterface(ppObjArray));
	return S_OK;
}

HRESULT CLayoutBuilder::MapType(BSTR bstrType, ElementType* pElementType)
{
	CComBSTR type(bstrType);
	if (type == CComBSTR(Twitter::Themes::Metadata::LayoutTypes::HorizontalContainer))
		*pElementType = ElementType::HorizontalContainer;
	else if (type == CComBSTR(Twitter::Themes::Metadata::LayoutTypes::ImageColumn))
		*pElementType = ElementType::ImageColumn;
	else if (type == CComBSTR(Twitter::Themes::Metadata::LayoutTypes::TextColumn))
		*pElementType = ElementType::TextColumn;
	else if (type == CComBSTR(Twitter::Themes::Metadata::LayoutTypes::MarqueeProgressColumn))
		*pElementType = ElementType::MarqueeProgressColumn;
	else
		*pElementType = ElementType::UnknownValue;
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::GetElementType(IVariantObject* pVariantObject, ElementType* pElementType)
{
	CHECK_E_POINTER(pElementType);
	CComVar vLayoutType;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Themes::Metadata::Element::Type, &vLayoutType));
	ATLASSERT(vLayoutType.vt == VT_BSTR);
	RETURN_IF_FAILED(MapType(vLayoutType.bstrVal, pElementType));
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::ApplyRightAlign(IColumnsInfo* pChildItems, CRect& rectParent, CRect& rect)
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(pChildItems->GetCount(&uiCount));
	int maxRight = rectParent.right;
	while (uiCount > 0)
	{
		uiCount--;
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(pChildItems->GetItem(uiCount, &pColumnsInfoItem));
		CRect rect;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRect(&rect));
		CComBSTR bstrAlign;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(Twitter::Themes::Metadata::Element::AlignHorizontal, &bstrAlign));
		if (bstrAlign == Twitter::Themes::Metadata::AlignTypes::Right)
		{
			rect.left = maxRight - rect.Width();
			rect.right = maxRight;
			maxRight -= rect.Width();
			RETURN_IF_FAILED(pColumnsInfoItem->SetRect(rect));
		}
	}
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::FitToParent(IVariantObject* pElement, CRect& rectParent, CRect& rect)
{
	CComVar vFitHorizantal;
	CComVar vFitVertical;
	pElement->GetVariantValue(Twitter::Themes::Metadata::Element::FitHorizontal, &vFitHorizantal);
	pElement->GetVariantValue(Twitter::Themes::Metadata::Element::FitVertical, &vFitVertical);

	if (vFitHorizantal.vt == VT_BSTR && CComBSTR(vFitHorizantal.bstrVal) == Twitter::Themes::Metadata::FitTypes::Parent)
	{
		rect.right = rectParent.right;
	}

	if (vFitVertical.vt == VT_BSTR && CComBSTR(vFitVertical.bstrVal) == Twitter::Themes::Metadata::FitTypes::Parent)
	{
		rect.bottom = rectParent.bottom;
	}
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::ApplyEndMargins(IVariantObject* pElement, CRect& rect)
{
	CComVar vMarginRight;
	CComVar vMarginBottom;
	pElement->GetVariantValue(Twitter::Themes::Metadata::Element::MarginRight, &vMarginRight);
	pElement->GetVariantValue(Twitter::Themes::Metadata::Element::MarginBottom, &vMarginBottom);

	if (vMarginRight.vt == VT_I4)
	{
		auto val = vMarginRight.intVal;
		rect.right += val;
	}

	if (vMarginBottom.vt == VT_I4)
	{
		auto val = vMarginBottom.intVal;
		rect.bottom += val;
	}
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::ApplyStartMargins(IVariantObject* pElement, CRect& rect)
{
	CHECK_E_POINTER(pElement);

	CComVar vMarginLeft;
	CComVar vMarginTop;

	pElement->GetVariantValue(Twitter::Themes::Metadata::Element::MarginLeft, &vMarginLeft);
	pElement->GetVariantValue(Twitter::Themes::Metadata::Element::MarginTop, &vMarginTop);

	if (vMarginLeft.vt == VT_I4)
	{
		auto val = vMarginLeft.intVal;
		auto width = rect.Width();
		rect.left += val;
		rect.right = rect.left + width;
	}

	if (vMarginTop.vt == VT_I4)
	{
		auto val = vMarginTop.intVal;
		auto height = rect.Height();
		rect.top += val;
		rect.bottom = rect.top + height;
	}

	return S_OK;
}

STDMETHODIMP CLayoutBuilder::BuildHorizontalContainer(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo)
{
	CRect sourceRect = *pSourceRect;
	sourceRect.MoveToX(0);
	sourceRect.MoveToY(0);

	CRect containerRect;
	containerRect.left = sourceRect.left;
	containerRect.right = sourceRect.left;
	containerRect.top = sourceRect.top;

	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	RETURN_IF_FAILED(pColumnInfo->AddItem(&pColumnsInfoItem));

	CComPtr<IColumnsInfo> pChildItems;
	RETURN_IF_FAILED(pColumnsInfoItem->GetChildItems(&pChildItems));

	CComPtr<IObjArray> pElements;
	RETURN_IF_FAILED(GetElements(pLayoutObject, &pElements));
	UINT uiCount = 0;
	RETURN_IF_FAILED(pElements->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pElement;
		RETURN_IF_FAILED(pElements->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pElement));

		CRect elementRect;
		ElementType elementType = ElementType::UnknownValue;
		RETURN_IF_FAILED(GetElementType(pElement, &elementType));

		CRect localSourceRect = sourceRect;

		CComVar vVisible;
		RETURN_IF_FAILED(pElement->GetVariantValue(Twitter::Themes::Metadata::Element::Visible, &vVisible));
		ATLASSERT(vVisible.vt == VT_BOOL);
		switch (elementType)
		{
			case ElementType::HorizontalContainer:
			{
				RETURN_IF_FAILED(BuildHorizontalContainer(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pImageManagerService, pChildItems));
				break;
			}
			case ElementType::TextColumn:
				RETURN_IF_FAILED(ApplyStartMargins(pElement, localSourceRect));
				RETURN_IF_FAILED(BuildTextColumn(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pChildItems));
				RETURN_IF_FAILED(ApplyEndMargins(pElement, elementRect));
				RETURN_IF_FAILED(FitToParent(pElement, localSourceRect, elementRect));
				break;
			case ElementType::ImageColumn:
				RETURN_IF_FAILED(ApplyStartMargins(pElement, localSourceRect));
				RETURN_IF_FAILED(BuildImageColumn(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pImageManagerService, pChildItems));
				RETURN_IF_FAILED(ApplyEndMargins(pElement, elementRect));
				RETURN_IF_FAILED(FitToParent(pElement, localSourceRect, elementRect));
				break;
			case ElementType::MarqueeProgressColumn:
				RETURN_IF_FAILED(ApplyStartMargins(pElement, localSourceRect));
				RETURN_IF_FAILED(BuildMarqueeProgressColumn(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pChildItems));
				RETURN_IF_FAILED(ApplyEndMargins(pElement, elementRect));
				RETURN_IF_FAILED(FitToParent(pElement, localSourceRect, elementRect));
				break;
		}

		if (vVisible.boolVal)
		{
			sourceRect.left = elementRect.right;
			containerRect.right = elementRect.right;
			containerRect.bottom = max(elementRect.bottom, containerRect.bottom);
		}
		else
		{
			elementRect = localSourceRect;
			elementRect.right = elementRect.left;
			elementRect.bottom = elementRect.top;
		}
	}

	CRect parentRect = *pSourceRect;
	containerRect.OffsetRect(parentRect.left, parentRect.top);

	RETURN_IF_FAILED(ApplyStartMargins(pLayoutObject, containerRect));
	RETURN_IF_FAILED(ApplyEndMargins(pLayoutObject, containerRect));
	RETURN_IF_FAILED(FitToParent(pLayoutObject, parentRect, containerRect));
	RETURN_IF_FAILED(ApplyRightAlign(pChildItems, parentRect, containerRect));

	RETURN_IF_FAILED(SetColumnProps(pLayoutObject, pColumnsInfoItem));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRect(containerRect));
	*pDestRect = containerRect;
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::SetColumnProps(IVariantObject* pLayoutObject, IColumnsInfoItem* pColumnsInfoItem)
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(pLayoutObject->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComBSTR bstrKey;
		RETURN_IF_FAILED(pLayoutObject->GetKeyByIndex(i, &bstrKey));
		CComVar vValue;
		RETURN_IF_FAILED(pLayoutObject->GetVariantValue(bstrKey, &vValue));
		RETURN_IF_FAILED(pColumnsInfoItem->SetVariantValue(bstrKey, &vValue));
	}

	CComVar vName;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::Element::Name, &vName));
	ATLASSERT(vName.vt == VT_BSTR);
	RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Column::Name, vName.bstrVal));
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::BuildTextColumn(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo)
{
	CComVar vTextKey;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::TextColumn::Text, &vTextKey));
	ATLASSERT(vTextKey.vt == VT_BSTR);

	CComVar vFont;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::TextColumn::Font, &vFont));
	ATLASSERT(vFont.vt == VT_BSTR);

	HFONT font = 0;
	RETURN_IF_FAILED(m_pThemeFontMap->GetFont(vFont.bstrVal, &font));
	CDCSelectFontScope cdcSelectFontScope(hdc, font);

	CComBSTR bstr = vTextKey.bstrVal;
	CSize sz;
	GetTextExtentPoint32(hdc, bstr, bstr.Length(), &sz);

	CRect sourceRect = *pSourceRect;
	CRect textRect;
	textRect.left = sourceRect.left;
	textRect.top = sourceRect.top;
	textRect.right = textRect.left + sz.cx;
	textRect.bottom = textRect.top + sz.cy;

	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	RETURN_IF_FAILED(pColumnInfo->AddItem(&pColumnsInfoItem));
	RETURN_IF_FAILED(SetColumnProps(pLayoutObject, pColumnsInfoItem));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRect(textRect));

	CComVar vColor;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::TextColumn::Color, &vColor));
	ATLASSERT(vColor.vt == VT_BSTR);
	CComVar vColorSelected;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::TextColumn::ColorSelected, &vColorSelected));
	ATLASSERT(vColorSelected.vt == VT_BSTR);
	CComVar vText;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::TextColumn::Text, &vText));
	ATLASSERT(vText.vt == VT_BSTR);

	RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Themes::Metadata::TextColumn::Color, vColor.bstrVal));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Themes::Metadata::TextColumn::ColorSelected, vColorSelected.bstrVal));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Themes::Metadata::TextColumn::Text, vText.bstrVal));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Themes::Metadata::TextColumn::Font, vFont.bstrVal));
	*pDestRect = textRect;
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::BuildImageColumn(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo)
{
	CComVar vImageKey;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::ImageColumn::ImageKey, &vImageKey));
	ATLASSERT(vImageKey.vt == VT_BSTR);
	TBITMAP bitmapInfo = { 0 };
	RETURN_IF_FAILED(pImageManagerService->GetImageInfo(vImageKey.bstrVal, &bitmapInfo));

	CRect sourceRect = *pSourceRect;
	CRect imageRect;
	imageRect.left = sourceRect.left;
	imageRect.top = sourceRect.top;
	imageRect.right = imageRect.left + bitmapInfo.Width;
	imageRect.bottom = imageRect.top + bitmapInfo.Height;

	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	RETURN_IF_FAILED(pColumnInfo->AddItem(&pColumnsInfoItem));
	RETURN_IF_FAILED(SetColumnProps(pLayoutObject, pColumnsInfoItem));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRect(imageRect));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Themes::Metadata::ImageColumn::ImageKey, vImageKey.bstrVal));
	*pDestRect = imageRect;
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::BuildMarqueeProgressColumn(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo)
{
	CComVar vItemSize;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::MarqueeProgressColumn::ItemSize, &vItemSize));
	CComVar vItemDistance;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::MarqueeProgressColumn::ItemDistance, &vItemDistance));
	CComVar vItemCount;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::MarqueeProgressColumn::ItemCount, &vItemCount));

	ATLASSERT(vItemSize.vt == VT_I4);
	ATLASSERT(vItemDistance.vt == VT_I4);
	ATLASSERT(vItemCount.vt == VT_I4);

	auto itemSize = vItemSize.intVal;
	auto itemDistance = vItemDistance.intVal;
	auto itemCount = vItemCount.intVal;

	CRect sourceRect = *pSourceRect;
	CRect columnRect;
	columnRect.left = sourceRect.left;
	columnRect.top = sourceRect.top;

	int width = itemSize * itemCount + itemDistance * (itemCount - 1);
	columnRect.right = columnRect.left + width;
	columnRect.bottom = columnRect.top + itemSize;

	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	RETURN_IF_FAILED(pColumnInfo->AddItem(&pColumnsInfoItem));
	RETURN_IF_FAILED(SetColumnProps(pLayoutObject, pColumnsInfoItem));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRect(columnRect));
	*pDestRect = columnRect;
	return S_OK;
}
