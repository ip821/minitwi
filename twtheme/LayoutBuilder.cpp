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

STDMETHODIMP CLayoutBuilder::BuildLayout(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo)
{
	RETURN_IF_FAILED(BuildHorizontalContainer(hdc, pSourceRect, pDestRect, pLayoutObject, pValueObject, pImageManagerService, pColumnInfo));
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::GetElements(IVariantObject* pVariantObject, IObjArray** ppObjArray)
{
	CHECK_E_POINTER(pVariantObject);
	CHECK_E_POINTER(ppObjArray);
	CComVariant vElements;
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
	CComVariant vLayoutType;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Themes::Metadata::Element::Type, &vLayoutType));
	ATLASSERT(vLayoutType.vt == VT_BSTR);
	RETURN_IF_FAILED(MapType(vLayoutType.bstrVal, pElementType));
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::ApplyRightAlign(IVariantObject* pElement, CRect& rectParent, CRect& rect)
{
	CComVariant vAlignRight;
	pElement->GetVariantValue(Twitter::Themes::Metadata::Element::AlignRight, &vAlignRight);

	if (vAlignRight.vt == VT_BSTR)
	{
		rect.left = rectParent.right - rect.Width();
		rect.right = rectParent.right;
	}
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::FitToParent(IVariantObject* pElement, CRect& rectParent, CRect& rect)
{
	CComVariant vFitHorizantal;
	CComVariant vFitVertical;
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
	CComVariant vMarginRight;
	CComVariant vMarginBottom;
	pElement->GetVariantValue(Twitter::Themes::Metadata::Element::MarginRight, &vMarginRight);
	pElement->GetVariantValue(Twitter::Themes::Metadata::Element::MarginBottom, &vMarginBottom);

	if (vMarginRight.vt == VT_BSTR)
	{
		auto val = _wtoi(vMarginRight.bstrVal);
		rect.right += val;
	}

	if (vMarginBottom.vt == VT_BSTR)
	{
		auto val = _wtoi(vMarginBottom.bstrVal);
		rect.bottom += val;
	}
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::ApplyStartMargins(IVariantObject* pElement, CRect& rect)
{
	CHECK_E_POINTER(pElement);

	CComVariant vMarginLeft;
	CComVariant vMarginTop;

	pElement->GetVariantValue(Twitter::Themes::Metadata::Element::MarginLeft, &vMarginLeft);
	pElement->GetVariantValue(Twitter::Themes::Metadata::Element::MarginTop, &vMarginTop);

	if (vMarginLeft.vt == VT_BSTR)
	{
		auto val = _wtoi(vMarginLeft.bstrVal);
		auto width = rect.Width();
		rect.left += val;
		rect.right = rect.left + width;
	}

	if (vMarginTop.vt == VT_BSTR)
	{
		auto val = _wtoi(vMarginTop.bstrVal);
		auto height = rect.Height();
		rect.top += val;
		rect.bottom= rect.top + height;
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
		CComVariant vVisible;
		RETURN_IF_FAILED(pElement->GetVariantValue(Twitter::Themes::Metadata::Element::Visible, &vVisible));
		ATLASSERT(vVisible.vt == VT_BOOL);
		if (!vVisible.boolVal)
			continue;

		CRect elementRect;
		ElementType elementType = ElementType::UnknownValue;
		RETURN_IF_FAILED(GetElementType(pElement, &elementType));

		CRect localSourceRect = sourceRect;
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
				RETURN_IF_FAILED(ApplyRightAlign(pElement, localSourceRect, elementRect));
				break;
			case ElementType::ImageColumn:
				RETURN_IF_FAILED(ApplyStartMargins(pElement, localSourceRect));
				RETURN_IF_FAILED(BuildImageColumn(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pImageManagerService, pChildItems));
				RETURN_IF_FAILED(ApplyEndMargins(pElement, elementRect));
				RETURN_IF_FAILED(FitToParent(pElement, localSourceRect, elementRect));
				RETURN_IF_FAILED(ApplyRightAlign(pElement, localSourceRect, elementRect));
				break;
			case ElementType::MarqueeProgressColumn:
				RETURN_IF_FAILED(ApplyStartMargins(pElement, localSourceRect));
				RETURN_IF_FAILED(BuildMarqueeProgressColumn(hdc, &localSourceRect, &elementRect, pElement, pValueObject, pChildItems));
				RETURN_IF_FAILED(ApplyEndMargins(pElement, elementRect));
				RETURN_IF_FAILED(FitToParent(pElement, localSourceRect, elementRect));
				RETURN_IF_FAILED(ApplyRightAlign(pElement, localSourceRect, elementRect));
				break;
		}

		sourceRect.left = elementRect.right;
		containerRect.right = elementRect.right;
		containerRect.bottom = max(elementRect.bottom, containerRect.bottom);
	}

	CRect parentRect = *pSourceRect;
	containerRect.OffsetRect(parentRect.left, parentRect.top);

	RETURN_IF_FAILED(ApplyStartMargins(pLayoutObject, containerRect));
	RETURN_IF_FAILED(ApplyEndMargins(pLayoutObject, containerRect));
	RETURN_IF_FAILED(FitToParent(pLayoutObject, parentRect, containerRect));
	RETURN_IF_FAILED(ApplyRightAlign(pLayoutObject, parentRect, containerRect));

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
		CComVariant vValue;
		RETURN_IF_FAILED(pLayoutObject->GetVariantValue(bstrKey, &vValue));
		RETURN_IF_FAILED(pColumnsInfoItem->SetVariantValue(bstrKey, &vValue));
	}

	CComVariant vName;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::Element::Name, &vName));
	ATLASSERT(vName.vt == VT_BSTR);
	RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Column::Name, vName.bstrVal));
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::BuildTextColumn(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo)
{
	CComVariant vTextKey;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::TextColumn::Text, &vTextKey));
	ATLASSERT(vTextKey.vt == VT_BSTR);

	CComVariant vFont;
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

	CComVariant vColor;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::TextColumn::Color, &vColor));
	ATLASSERT(vColor.vt == VT_BSTR);
	CComVariant vColorSelected;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::TextColumn::ColorSelected, &vColorSelected));
	ATLASSERT(vColorSelected.vt == VT_BSTR);
	CComVariant vText;
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
	CComVariant vImageKey;
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
	CComVariant vItemSize;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::MarqueeProgressColumn::ItemSize, &vItemSize));
	CComVariant vItemDistance;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::MarqueeProgressColumn::ItemDistance, &vItemDistance));
	CComVariant vItemCount;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::MarqueeProgressColumn::ItemCount, &vItemCount));

	ATLASSERT(vItemSize.vt == VT_BSTR);
	ATLASSERT(vItemDistance.vt == VT_BSTR);
	ATLASSERT(vItemCount.vt == VT_BSTR);

	auto itemSize = _wtoi(vItemSize.bstrVal);
	auto itemDistance = _wtoi(vItemDistance.bstrVal);
	auto itemCount = _wtoi(vItemCount.bstrVal);

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
