// SkinDefault.cpp : Implementation of CSkinDefault

#include "stdafx.h"
#include "ThemeDefault.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"

// CSkinDefault

#define FONT_NAME L"Tahoma"
#define FONT_SIZE 10

using namespace Gdiplus;
using namespace IP;

HRESULT CThemeDefault::FinalConstruct()
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinTabControl, &m_pSkinTabControl));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinCommonControl, &m_pSkinCommonControl));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ThemeColorMap, &m_pThemeColorMap));

	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_BRUSH_BACKGROUND, (ARGB)Color::White));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_BRUSH_SELECTED, (ARGB)Color::Beige));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(Twitter::Connection::Metadata::UserObject::DisplayName, (ARGB)Color::SteelBlue));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(Twitter::Connection::Metadata::UserObject::Name, (ARGB)Color::Gray));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(Twitter::Connection::Metadata::TweetObject::NormalizedText, (ARGB)Color::Black));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_DELIMITER, (ARGB)Color::LightGray));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(Twitter::Connection::Metadata::TweetObject::Url, (ARGB)Color::SteelBlue));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(Twitter::Connection::Metadata::TweetObject::RetweetedUserDisplayName, (ARGB)Color::Gray));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_RELATIVE_TIME, (ARGB)Color::Gray));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_COLUMN_SHOW_MORE, (ARGB)Color::SteelBlue));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TAB_HEADER_SELECTED, (ARGB)Color::Black));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TAB_HEADER, (ARGB)Color::Gray));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_ITEM_ANIMATION_ACTIVE, (ARGB)Color::Black));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_ITEM_ANIMATION_INACTIVE, (ARGB)Color::Gray));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_PICTURE_WINDOW_TEXT, (ARGB)Color::White));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_ACCOUNT_CONTROL_TEXT, (ARGB)Color::White));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_ITEM_FOLLOW_BUTTON_RECT, (ARGB)Color::DarkCyan));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_ITEM_FOLLOW_BUTTON_RECT_PUSHED, (ARGB)Color::DarkCyan));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_ITEM_FOLLOW_BUTTON_RECT_DISABLED, (ARGB)Color::Gray));

	RETURN_IF_FAILED(m_pSkinTabControl->SetColorMap(m_pThemeColorMap));
	RETURN_IF_FAILED(m_pSkinCommonControl->SetColorMap(m_pThemeColorMap));

	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ThemeFontMap, &m_pThemeFontMap));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(Twitter::Connection::Metadata::TweetObject::RetweetedUserDisplayName, FONT_NAME, FONT_SIZE - 2, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(VAR_TWITTER_RELATIVE_TIME, FONT_NAME, FONT_SIZE, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(VAR_TWITTER_RELATIVE_TIME + CString(VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE, FALSE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(Twitter::Connection::Metadata::UserObject::DisplayName, FONT_NAME, FONT_SIZE, TRUE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(Twitter::Connection::Metadata::UserObject::DisplayName + CString(VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE, TRUE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(Twitter::Connection::Metadata::UserObject::Name, FONT_NAME, FONT_SIZE, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(Twitter::Connection::Metadata::UserObject::Name + CString(VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE, FALSE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(Twitter::Connection::Metadata::TweetObject::NormalizedText, FONT_NAME, FONT_SIZE, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(Twitter::Connection::Metadata::TweetObject::NormalizedText + CString(VAR_DOUBLE_SIZE_POSTFIX)), FONT_NAME, (DWORD)(FONT_SIZE * 1.2), FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(Twitter::Connection::Metadata::TweetObject::Url, FONT_NAME, FONT_SIZE, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(Twitter::Connection::Metadata::TweetObject::Url + CString(VAR_DOUBLE_SIZE_POSTFIX)), FONT_NAME, (DWORD)(FONT_SIZE * 1.2), FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(Twitter::Connection::Metadata::TweetObject::Url + CString(VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE, FALSE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(Twitter::Connection::Metadata::TweetObject::Url + CString(VAR_DOUBLE_SIZE_POSTFIX) + CString(VAR_SELECTED_POSTFIX)), FONT_NAME, (DWORD)(FONT_SIZE * 1.2), FALSE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(VAR_COLUMN_SHOW_MORE, FONT_NAME, FONT_SIZE, TRUE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(VAR_COLUMN_SHOW_MORE + CString(VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE, TRUE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(VAR_TAB_HEADER, FONT_NAME, FONT_SIZE, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(Twitter::Connection::Metadata::UserObject::DisplayNameUserAccount, FONT_NAME, FONT_SIZE * 2, TRUE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(VAR_PICTURE_WINDOW_TEXT, FONT_NAME, FONT_SIZE, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(VAR_ITEM_FOLLOW_BUTTON, FONT_NAME, (DWORD)(FONT_SIZE * 1.2), FALSE, FALSE));

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