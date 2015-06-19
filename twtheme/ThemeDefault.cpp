// SkinDefault.cpp : Implementation of CSkinDefault

#include "stdafx.h"
#include "ThemeDefault.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"
#include "..\model-libs\objmdl\textfile.h"
#include "..\model-libs\objmdl\JSONConverter.h"

// CSkinDefault

#define FONT_NAME L"Tahoma"
#define FONT_SIZE 10

using namespace Gdiplus;
using namespace IP;
using namespace std;

hash_set<wstring> CThemeDefault::m_inheritedProps =
{
};

HRESULT CThemeDefault::FinalConstruct()
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinTabControl, &m_pSkinTabControl));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinCommonControl, &m_pSkinCommonControl));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ThemeColorMap, &m_pThemeColorMap));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ThemeFontMap, &m_pThemeFontMap));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ImageManagerService, &m_pImageManagerService));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_LayoutManager, &m_pLayoutManager));

	CComQIPtr<ILayoutManagerInternal> pLayoutManagerInternal = m_pLayoutManager;
	RETURN_IF_FAILED(pLayoutManagerInternal->SetFontMap(m_pThemeFontMap));
	RETURN_IF_FAILED(pLayoutManagerInternal->SetColorMap(m_pThemeColorMap));

	CComPtr<IStream> pStream;
	RETURN_IF_FAILED(HrGetResourceStream(_AtlBaseModule.GetModuleInstance(), IDR_THEMEJSON, L"JSON", &pStream));
	RETURN_IF_FAILED(LoadThemeFromStream(pStream));

	CComQIPtr<IThemeSupport> pSkinTabControlThemeSupport = m_pSkinTabControl;
	RETURN_IF_FAILED(pSkinTabControlThemeSupport->SetTheme(this));
	RETURN_IF_FAILED(m_pSkinCommonControl->SetColorMap(m_pThemeColorMap));
	RETURN_IF_FAILED(m_pSkinCommonControl->SetFontMap(m_pThemeFontMap));

	return S_OK;
}

STDMETHODIMP CThemeDefault::GetTimelineSkin(ISkinTimeline** ppSkinTimeline)
{
	CHECK_E_POINTER(ppSkinTimeline);
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinTimeline, ppSkinTimeline));
	RETURN_IF_FAILED((*ppSkinTimeline)->SetFontMap(m_pThemeFontMap));
	RETURN_IF_FAILED((*ppSkinTimeline)->SetColorMap(m_pThemeColorMap));
	return S_OK;
}

STDMETHODIMP CThemeDefault::GetTabControlSkin(ISkinTabControl** pSkinTabControl)
{
	CHECK_E_POINTER(pSkinTabControl);
	RETURN_IF_FAILED(m_pSkinTabControl->QueryInterface(pSkinTabControl));
	return S_OK;
}

STDMETHODIMP CThemeDefault::GetCommonControlSkin(ISkinCommonControl** pSkinCommonControl)
{
	CHECK_E_POINTER(pSkinCommonControl);
	RETURN_IF_FAILED(m_pSkinCommonControl->QueryInterface(pSkinCommonControl));
	return S_OK;
}

STDMETHODIMP CThemeDefault::GetColorMap(IThemeColorMap** ppThemeColorMap)
{
	CHECK_E_POINTER(ppThemeColorMap);
	RETURN_IF_FAILED(m_pThemeColorMap->QueryInterface(ppThemeColorMap));
	return S_OK;
}

STDMETHODIMP CThemeDefault::GetFontMap(IThemeFontMap** ppThemeFontMap)
{
	CHECK_E_POINTER(ppThemeFontMap);
	RETURN_IF_FAILED(m_pThemeFontMap->QueryInterface(ppThemeFontMap));
	return S_OK;
}

STDMETHODIMP CThemeDefault::GetSkinUserAccountControl(ISkinUserAccountControl** ppSkinUserAccountControl)
{
	CHECK_E_POINTER(ppSkinUserAccountControl);
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinUserAccountControl, ppSkinUserAccountControl));
	RETURN_IF_FAILED((*ppSkinUserAccountControl)->SetFontMap(m_pThemeFontMap));
	RETURN_IF_FAILED((*ppSkinUserAccountControl)->SetColorMap(m_pThemeColorMap));
	return S_OK;
}

STDMETHODIMP CThemeDefault::GetLayoutManager(ILayoutManager** ppLayoutManager)
{
	return m_pLayoutManager->QueryInterface(ppLayoutManager);
}

STDMETHODIMP CThemeDefault::GetLayout(BSTR bstrLayoutName, IVariantObject** ppVariantObject)
{
	CHECK_E_POINTER(bstrLayoutName);
	CHECK_E_POINTER(ppVariantObject);

	if (m_layoutsMap.find(bstrLayoutName) == m_layoutsMap.end())
		return E_INVALIDARG;

	CComPtr<IVariantObject> pLayout;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pLayout));
	RETURN_IF_FAILED(m_layoutsMap[bstrLayoutName]->CopyTo(pLayout));
	return pLayout->QueryInterface(ppVariantObject);
}

STDMETHODIMP CThemeDefault::LoadThemeFromStream(IStream* pStream)
{
	CString strFile;
	CTextFile::ReadAllTextFromStream(pStream, strFile);
	auto value = shared_ptr<JSONValue>(JSON::Parse(strFile));
	if (value == nullptr)
		return E_FAIL;

	auto rootArray = value->AsArray();

	{
		//Load color table
		auto colorsObj = rootArray[0]->AsObject();
		auto colorArray = colorsObj[L"colors"]->AsArray();
		CComPtr<IObjCollection> pColorObjCollection;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pColorObjCollection));
		RETURN_IF_FAILED(CJsonConverter::ConvertArray(colorArray, pColorObjCollection));
		RETURN_IF_FAILED(m_pThemeColorMap->Initialize(pColorObjCollection));
	}

	{
		//Load font table
		auto fontsTable = rootArray[1]->AsObject();
		auto fontArray = fontsTable[L"fonts"]->AsArray();
		CComPtr<IObjCollection> pFontObjCollection;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pFontObjCollection));
		RETURN_IF_FAILED(CJsonConverter::ConvertArray(fontArray, pFontObjCollection));
		RETURN_IF_FAILED(m_pThemeFontMap->Initialize(pFontObjCollection));
	}

	{
		//Load images table
		auto imagesTable = rootArray[2]->AsObject();
		auto imageArray = imagesTable[L"images"]->AsArray();
		CComPtr<IObjCollection> pImagesObjCollection;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pImagesObjCollection));
		RETURN_IF_FAILED(CJsonConverter::ConvertArray(imageArray, pImagesObjCollection));
		RETURN_IF_FAILED(m_pImageManagerService->Initialize(pImagesObjCollection));
	}

	{
		//Load styles table
		auto stylesTable = rootArray[3]->AsObject();
		auto styleArray = stylesTable[L"styles"]->AsArray();
		CComPtr<IObjCollection> pStylesObjCollection;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pStylesObjCollection));
		RETURN_IF_FAILED(CJsonConverter::ConvertArray(styleArray, pStylesObjCollection));

		UINT uiCount = 0;
		RETURN_IF_FAILED(pStylesObjCollection->GetCount(&uiCount));
		for (size_t i = 0; i < uiCount; i++)
		{
			CComPtr<IVariantObject> pStyleObject;
			RETURN_IF_FAILED(pStylesObjCollection->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pStyleObject));
			CComVariant vName;
			RETURN_IF_FAILED(pStyleObject->GetVariantValue(L"name", &vName));
			ATLASSERT(vName.vt == VT_BSTR);
			m_stylesMap[vName.bstrVal] = pStyleObject;
		}
	}

	{
		//Load layouts
		auto layoutsTable = rootArray[4]->AsObject();
		auto layoutArray = layoutsTable[L"layouts"]->AsArray();
		CComPtr<IObjCollection> pLayoutsObjCollection;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pLayoutsObjCollection));
		RETURN_IF_FAILED(CJsonConverter::ConvertArray(layoutArray, pLayoutsObjCollection));
		RETURN_IF_FAILED(ApplyStyles(nullptr, pLayoutsObjCollection));

		UINT uiCount = 0;
		RETURN_IF_FAILED(pLayoutsObjCollection->GetCount(&uiCount));
		for (size_t i = 0; i < uiCount; i++)
		{
			CComPtr<IVariantObject> pLayoutObject;
			RETURN_IF_FAILED(pLayoutsObjCollection->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pLayoutObject));
			CComVariant vName;
			RETURN_IF_FAILED(pLayoutObject->GetVariantValue(L"name", &vName));
			ATLASSERT(vName.vt == VT_BSTR);
			m_layoutsMap[vName.bstrVal] = pLayoutObject;
		}
	}
	return S_OK;
}

STDMETHODIMP CThemeDefault::CopyProps(IVariantObject* pSourceObject, IVariantObject* pDestObject, bool filterProps)
{
	UINT uiPropsCount = 0;
	RETURN_IF_FAILED(pSourceObject->GetCount(&uiPropsCount));
	for (size_t j = 0; j < uiPropsCount; j++)
	{
		CComBSTR bstrKey;
		RETURN_IF_FAILED(pSourceObject->GetKeyByIndex(j, &bstrKey));

		if (bstrKey == L"name")
			continue;

		if (filterProps && m_inheritedProps.find(wstring(bstrKey)) == m_inheritedProps.end())
			continue;

		CComVariant vValue;
		RETURN_IF_FAILED(pDestObject->GetVariantValue(bstrKey, &vValue));

		if (vValue.vt == VT_EMPTY)
		{
			CComVariant vProp;
			RETURN_IF_FAILED(pSourceObject->GetVariantValue(bstrKey, &vProp));
			RETURN_IF_FAILED(pDestObject->SetVariantValue(bstrKey, &vProp));
		}
	}
	return S_OK;
}

STDMETHODIMP CThemeDefault::ApplyStyles(IVariantObject* pParentObject, IObjArray* pElements)
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(pElements->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pElement;
		RETURN_IF_FAILED(pElements->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pElement));

		CComVariant vStyle;
		RETURN_IF_FAILED(pElement->GetVariantValue(L"style", &vStyle));
		if (vStyle.vt == VT_BSTR)
		{
			RETURN_IF_FAILED(ApplyStyle(pElement, vStyle.bstrVal));
		}
		RETURN_IF_FAILED(ApplyStyle(pElement, L"")); //apply default
	}
	return S_OK;
}

STDMETHODIMP CThemeDefault::ApplyStyle(IVariantObject* pElement, BSTR bstrStyleName)
{
	auto it = m_stylesMap.find(bstrStyleName);
	if (it != m_stylesMap.end())
	{
		RETURN_IF_FAILED(CopyProps(it->second, pElement, false));
	}

	CComVariant vChildElements;
	RETURN_IF_FAILED(pElement->GetVariantValue(L"elements", &vChildElements));
	if (vChildElements.vt == VT_UNKNOWN)
	{
		CComQIPtr<IObjArray> pChildElements = vChildElements.punkVal;
		RETURN_IF_FAILED(ApplyStyles(pElement, pChildElements));
	}
	return S_OK;
}

STDMETHODIMP CThemeDefault::GetImageManagerService(IImageManagerService** ppImageManagerService)
{
	CHECK_E_POINTER(ppImageManagerService);
	RETURN_IF_FAILED(m_pImageManagerService->QueryInterface(ppImageManagerService));
	return S_OK;
}