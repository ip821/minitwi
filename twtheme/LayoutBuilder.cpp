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
	CComPtr<IObjArray> pElements;
	RETURN_IF_FAILED(GetElements(pLayoutObject, &pElements));
	UINT uiCount = 0;
	RETURN_IF_FAILED(pElements->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pElement;
		RETURN_IF_FAILED(pElements->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pElement));
		ElementType elementType = ElementType::UnknownValue;
		RETURN_IF_FAILED(GetElementType(pElement, &elementType));

		switch (elementType)
		{
			case ElementType::HorizontalContainer:
				RETURN_IF_FAILED(BuildHorizontalContainer(hdc, pSourceRect, pDestRect, pElement, pValueObject, pImageManagerService, pColumnInfo));
				break;
			case ElementType::TextColumn:
				RETURN_IF_FAILED(BuildTextColumn(hdc, pSourceRect, pDestRect, pElement, pValueObject, pColumnInfo));
				break;
		}
	}
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

STDMETHODIMP CLayoutBuilder::GetElementType(IVariantObject* pVariantObject, ElementType* pElementType)
{
	CHECK_E_POINTER(pElementType);
	CComVariant vLayoutType;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Themes::Metadata::Element::Type, &vLayoutType));
	ATLASSERT(vLayoutType.vt == VT_BSTR);
	if (CComBSTR(vLayoutType.bstrVal) == CComBSTR(Twitter::Themes::Metadata::LayoutTypes::HorizontalContainer))
		*pElementType = ElementType::HorizontalContainer;
	else if (CComBSTR(vLayoutType.bstrVal) == CComBSTR(Twitter::Themes::Metadata::LayoutTypes::ImageColumn))
		*pElementType = ElementType::ImageColumn;
	else if (CComBSTR(vLayoutType.bstrVal) == CComBSTR(Twitter::Themes::Metadata::LayoutTypes::TextColumn))
		*pElementType = ElementType::TextColumn;
	else
		*pElementType = ElementType::UnknownValue;
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::BuildHorizontalContainer(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo)
{
	CRect sourceRect = *pSourceRect;
	CRect destRect;
	CComPtr<IObjArray> pElements;
	RETURN_IF_FAILED(GetElements(pLayoutObject, &pElements));
	UINT uiCount = 0;
	RETURN_IF_FAILED(pElements->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pElement;
		RETURN_IF_FAILED(pElements->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pElement));
		CRect elementRect;
		RETURN_IF_FAILED(BuildLayout(hdc, &sourceRect, &elementRect, pLayoutObject, pValueObject, pImageManagerService, pColumnInfo));
		sourceRect.left += elementRect.Width();

		destRect.right += elementRect.Width();
		destRect.bottom = max(elementRect.bottom, destRect.bottom);
	}
	*pDestRect = destRect;
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::BuildTextColumn(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo)
{
	HFONT font = 0;
	RETURN_IF_FAILED(m_pThemeFontMap->GetFont(Twitter::Metadata::Tabs::Header, &font));
	CDCSelectFontScope cdcSelectFontScope(hdc, font);



	return S_OK;
}