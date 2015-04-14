// SkinDefault.cpp : Implementation of CSkinDefault

#include "stdafx.h"
#include "ThemeDefault.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"
#include "..\model-libs\objmdl\textfile.h"

// CSkinDefault

#define FONT_NAME L"Tahoma"
#define FONT_SIZE 10

using namespace Gdiplus;
using namespace IP;
using namespace std;

HRESULT CThemeDefault::FinalConstruct()
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinTabControl, &m_pSkinTabControl));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinCommonControl, &m_pSkinCommonControl));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ThemeColorMap, &m_pThemeColorMap));

	HMODULE hModule = _AtlBaseModule.GetModuleInstance();
	if (!hModule)
		return E_UNEXPECTED;

	HRSRC hRsrc = FindResource(hModule, MAKEINTRESOURCE(IDR_THEMEJSON), L"JSON");
	if (!hRsrc)
		return HRESULT_FROM_WIN32(GetLastError());

	HGLOBAL hGlobal = LoadResource(hModule, hRsrc);

	if (!hGlobal)
		return HRESULT_FROM_WIN32(GetLastError());

	auto dwSizeInBytes = SizeofResource(hModule, hRsrc);
	LPVOID pvResourceData = LockResource(hGlobal);
	CComPtr<IStream> pStream;
	pStream.Attach(SHCreateMemStream((LPBYTE)pvResourceData, dwSizeInBytes));
	LoadThemeFromStream(pStream);

	RETURN_IF_FAILED(m_pSkinTabControl->SetColorMap(m_pThemeColorMap));
	RETURN_IF_FAILED(m_pSkinCommonControl->SetColorMap(m_pThemeColorMap));

	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ThemeFontMap, &m_pThemeFontMap));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(Twitter::Connection::Metadata::TweetObject::RetweetedUserDisplayName, FONT_NAME, FONT_SIZE - 2, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(Twitter::Connection::Metadata::TweetObject::RetweetedUserName, FONT_NAME, FONT_SIZE - 2, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(Twitter::Connection::Metadata::TweetObject::RetweetedUserDisplayName + CString(Twitter::Metadata::Item::VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE - 2, FALSE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(Twitter::Connection::Metadata::TweetObject::RetweetedUserName + CString(Twitter::Metadata::Item::VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE - 2, FALSE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(Twitter::Metadata::Item::TwitterRelativeTime, FONT_NAME, FONT_SIZE, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(Twitter::Metadata::Item::TwitterRelativeTime + CString(Twitter::Metadata::Item::VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE, FALSE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(Twitter::Connection::Metadata::UserObject::DisplayName, FONT_NAME, FONT_SIZE, TRUE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(Twitter::Connection::Metadata::UserObject::DisplayName + CString(Twitter::Metadata::Item::VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE, TRUE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(Twitter::Connection::Metadata::UserObject::Name, FONT_NAME, FONT_SIZE, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(Twitter::Connection::Metadata::UserObject::Name + CString(Twitter::Metadata::Item::VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE, FALSE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(Twitter::Connection::Metadata::TweetObject::NormalizedText, FONT_NAME, FONT_SIZE, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(Twitter::Connection::Metadata::TweetObject::NormalizedText + CString(Twitter::Metadata::Item::VAR_DOUBLE_SIZE_POSTFIX)), FONT_NAME, (DWORD)(FONT_SIZE * 1.2), FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(Twitter::Connection::Metadata::TweetObject::Url, FONT_NAME, FONT_SIZE, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(Twitter::Connection::Metadata::TweetObject::Url + CString(Twitter::Metadata::Item::VAR_DOUBLE_SIZE_POSTFIX)), FONT_NAME, (DWORD)(FONT_SIZE * 1.2), FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(Twitter::Connection::Metadata::TweetObject::Url + CString(Twitter::Metadata::Item::VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE, FALSE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(Twitter::Connection::Metadata::TweetObject::Url + CString(Twitter::Metadata::Item::VAR_DOUBLE_SIZE_POSTFIX) + CString(Twitter::Metadata::Item::VAR_SELECTED_POSTFIX)), FONT_NAME, (DWORD)(FONT_SIZE * 1.2), FALSE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(Twitter::Metadata::Column::ShowMoreColumn, FONT_NAME, FONT_SIZE, TRUE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(Twitter::Metadata::Column::ShowMoreColumn + CString(Twitter::Metadata::Item::VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE, TRUE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(Twitter::Metadata::Tabs::Header, FONT_NAME, FONT_SIZE, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(Twitter::Connection::Metadata::UserObject::DisplayNameUserAccount, FONT_NAME, FONT_SIZE * 2, TRUE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(Twitter::Metadata::Drawing::PictureWindowText, FONT_NAME, FONT_SIZE, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(Twitter::Metadata::Item::VAR_ITEM_FOLLOW_BUTTON, FONT_NAME, (DWORD)(FONT_SIZE * 1.2), FALSE, FALSE));

	RETURN_IF_FAILED(m_pSkinTabControl->SetFontMap(m_pThemeFontMap));
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

STDMETHODIMP CThemeDefault::LoadThemeFromStream(IStream* pStream)
{
	CString strFile;
	CTextFile::ReadAllTextFromStream(pStream, strFile);
	auto value = shared_ptr<JSONValue>(JSON::Parse(strFile));
	if (value == nullptr)
		return E_FAIL;

	CComPtr<IVariantObject> pColorTableObject;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pColorTableObject));
	RETURN_IF_FAILED(ConvertToColorTableObject(value.get(), pColorTableObject));
	RETURN_IF_FAILED(m_pThemeColorMap->Initialize(pColorTableObject));

	return S_OK;
}

STDMETHODIMP CThemeDefault::ConvertToColorTableObject(JSONValue* pSourceValue, IVariantObject* pDestValue)
{
	auto rootArray = pSourceValue->AsArray();
	auto tablesObj = rootArray[0]->AsObject();
	auto tablesArray = tablesObj[L"colors"]->AsArray();
	for (auto& it : tablesArray)
	{
		auto colorObj = it->AsObject();
		auto colorName = colorObj[L"name"]->AsString();
		auto colorValue = colorObj[L"color"]->AsString();
		DWORD dwColor = 0;
		if (colorValue.substr(0, 2) == L"0x")
			dwColor = wcstoul(colorValue.c_str(), NULL, 16);
		else
			dwColor = m_knownColors[colorValue];
		RETURN_IF_FAILED(pDestValue->SetVariantValue(CComBSTR(colorName.c_str()), &CComVariant(dwColor)));
	}
	return S_OK;
}