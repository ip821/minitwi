// SkinDefault.cpp : Implementation of CSkinDefault

#include "stdafx.h"
#include "ThemeDefault.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"

// CSkinDefault

#define FONT_NAME L"Tahoma"
#define FONT_SIZE 10

using namespace Gdiplus;

HRESULT CThemeDefault::FinalConstruct()
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinTabControl, &m_pSkinTabControl));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinCommonControl, &m_pSkinCommonControl));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ThemeColorMap, &m_pThemeColorMap));

	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_BRUSH_BACKGROUND, (ARGB)Color::White));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_BRUSH_SELECTED, (ARGB)Color::Beige));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_USER_DISPLAY_NAME, (ARGB)Color::SteelBlue));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_USER_NAME, (ARGB)Color::Gray));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_NORMALIZED_TEXT, (ARGB)Color::Black));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_DELIMITER, (ARGB)Color::LightGray));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_URL, (ARGB)Color::SteelBlue));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_RETWEETED_USER_DISPLAY_NAME, (ARGB)Color::Gray));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_RELATIVE_TIME, (ARGB)Color::Gray));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_COLUMN_SHOW_MORE, (ARGB)Color::SteelBlue));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TAB_HEADER_SELECTED, (ARGB)Color::Black));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TAB_HEADER, (ARGB)Color::Gray));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_ITEM_ANIMATION_ACTIVE, (ARGB)Color::Black));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_ITEM_ANIMATION_INACTIVE, (ARGB)Color::Gray));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_PICTURE_WINDOW_TEXT, (ARGB)Color::White));

	RETURN_IF_FAILED(m_pSkinTabControl->SetColorMap(m_pThemeColorMap));
	RETURN_IF_FAILED(m_pSkinCommonControl->SetColorMap(m_pThemeColorMap));

	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ThemeFontMap, &m_pThemeFontMap));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(VAR_TWITTER_RETWEETED_USER_DISPLAY_NAME, FONT_NAME, FONT_SIZE - 2, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(VAR_TWITTER_RELATIVE_TIME, FONT_NAME, FONT_SIZE, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(VAR_TWITTER_RELATIVE_TIME + CString(VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE, FALSE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(VAR_TWITTER_USER_DISPLAY_NAME, FONT_NAME, FONT_SIZE, TRUE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(VAR_TWITTER_USER_DISPLAY_NAME + CString(VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE, TRUE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(VAR_TWITTER_USER_NAME, FONT_NAME, FONT_SIZE, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(VAR_TWITTER_USER_NAME + CString(VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE, FALSE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(VAR_TWITTER_NORMALIZED_TEXT, FONT_NAME, FONT_SIZE, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(VAR_TWITTER_NORMALIZED_TEXT + CString(VAR_DOUBLE_SIZE_POSTFIX)), FONT_NAME, (DWORD)(FONT_SIZE * 1.2), FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(VAR_TWITTER_URL, FONT_NAME, FONT_SIZE, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(VAR_TWITTER_URL + CString(VAR_DOUBLE_SIZE_POSTFIX)), FONT_NAME, (DWORD)(FONT_SIZE * 1.2), FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(VAR_TWITTER_URL + CString(VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE, FALSE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(VAR_TWITTER_URL + CString(VAR_DOUBLE_SIZE_POSTFIX) + CString(VAR_SELECTED_POSTFIX)), FONT_NAME, (DWORD)(FONT_SIZE * 1.2), FALSE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(VAR_COLUMN_SHOW_MORE, FONT_NAME, FONT_SIZE, TRUE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(VAR_COLUMN_SHOW_MORE + CString(VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE, TRUE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(VAR_TAB_HEADER, FONT_NAME, FONT_SIZE, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(VAR_TWITTER_USER_DISPLAY_NAME_USER_ACCOUNT, FONT_NAME, FONT_SIZE * 2, TRUE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(VAR_PICTURE_WINDOW_TEXT, FONT_NAME, FONT_SIZE, FALSE, FALSE));

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