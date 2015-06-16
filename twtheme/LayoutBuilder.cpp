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

STDMETHODIMP CLayoutBuilder::BuildHorizontalContainer(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo)
{
	CRect sourceRect = *pSourceRect;
	CRect destRect;
	destRect.left = sourceRect.left;
	destRect.top = sourceRect.top;

	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	RETURN_IF_FAILED(pColumnInfo->AddItem(&pColumnsInfoItem));

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

		switch (elementType)
		{
			case ElementType::HorizontalContainer:
				RETURN_IF_FAILED(BuildHorizontalContainer(hdc, pSourceRect, &elementRect, pElement, pValueObject, pImageManagerService, pColumnInfo));
				break;
			case ElementType::TextColumn:
				RETURN_IF_FAILED(BuildTextColumn(hdc, pSourceRect, &elementRect, pElement, pValueObject, pColumnInfo));
				break;
			case ElementType::ImageColumn:
				RETURN_IF_FAILED(BuildImageColumn(hdc, pSourceRect, &elementRect, pElement, pValueObject, pImageManagerService, pColumnInfo));
				break;
		}

		sourceRect.left += elementRect.Width();

		destRect.right += elementRect.Width();
		destRect.bottom = max(elementRect.Height(), destRect.bottom);
	}

	RETURN_IF_FAILED(SetColumnProps(pLayoutObject, pColumnsInfoItem));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRect(destRect));
	*pDestRect = destRect;
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::SetColumnProps(IVariantObject* pLayoutObject, IColumnsInfoItem* pColumnsInfoItem)
{
	ElementType elementType = ElementType::UnknownValue;
	RETURN_IF_FAILED(GetElementType(pLayoutObject, &elementType));
	CComVariant vName;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::Element::Name, &vName));
	ATLASSERT(vName.vt == VT_BSTR);
	RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Themes::Metadata::Element::Name, vName.bstrVal));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Column::Name, vName.bstrVal));
	CComVariant vLayoutType;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Twitter::Themes::Metadata::Element::Type, &vLayoutType));
	ATLASSERT(vLayoutType.vt == VT_BSTR);
	RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Themes::Metadata::Element::Type, vLayoutType.bstrVal));
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

	CRect textRect;
	textRect.left = pSourceRect->left;
	textRect.top = pSourceRect->top;
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
	CRect imageRect;
	imageRect.left = pSourceRect->left;
	imageRect.top = pSourceRect->top;
	imageRect.right = imageRect.left + bitmapInfo.Width;
	imageRect.bottom = imageRect.top + bitmapInfo.Height;

	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	RETURN_IF_FAILED(pColumnInfo->AddItem(&pColumnsInfoItem));
	RETURN_IF_FAILED(SetColumnProps(pLayoutObject, pColumnsInfoItem));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRect(imageRect));

	*pDestRect = imageRect;
	return S_OK;
}
