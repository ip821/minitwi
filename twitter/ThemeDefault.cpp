// SkinDefault.cpp : Implementation of CSkinDefault

#include "stdafx.h"
#include "ThemeDefault.h"
#include "Plugins.h"

// CSkinDefault

#define FONT_NAME L"Tahoma"
#define FONT_SIZE 10

HRESULT CThemeDefault::FinalConstruct()
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinTimeline, &m_pSkinTimeline));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinTabControl, &m_pSkinTabControl));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinCommonControl, &m_pSkinCommonControl));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ThemeColorMap, &m_pThemeColorMap));

	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_BRUSH_BACKGROUND, Gdiplus::Color::White));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_BRUSH_SELECTED, Gdiplus::Color::Beige));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_USER_DISPLAY_NAME, Gdiplus::Color::SteelBlue));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_USER_NAME, Gdiplus::Color::Gray));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_NORMALIZED_TEXT, Gdiplus::Color::Black));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_DELIMITER, Gdiplus::Color::LightGray));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_URL, Gdiplus::Color::SteelBlue));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_RETWEETED_USER_DISPLAY_NAME, Gdiplus::Color::Gray));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_RELATIVE_TIME, Gdiplus::Color::Gray));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_COLUMN_SHOW_MORE, Gdiplus::Color::SteelBlue));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TAB_HEADER_SELECTED, Gdiplus::Color::Black));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TAB_HEADER, Gdiplus::Color::Gray));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_ITEM_ANIMATION_ACTIVE, Gdiplus::Color::Black));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_ITEM_ANIMATION_INACTIVE, Gdiplus::Color::Gray));

	RETURN_IF_FAILED(m_pSkinTimeline->SetColorMap(m_pThemeColorMap));
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
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(VAR_TWITTER_URL, FONT_NAME, FONT_SIZE, FALSE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(VAR_TWITTER_URL + CString(VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE, FALSE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(VAR_COLUMN_SHOW_MORE, FONT_NAME, FONT_SIZE, TRUE, FALSE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(CComBSTR(VAR_COLUMN_SHOW_MORE + CString(VAR_SELECTED_POSTFIX)), FONT_NAME, FONT_SIZE, TRUE, TRUE));
	RETURN_IF_FAILED(m_pThemeFontMap->SetFont(VAR_TAB_HEADER, FONT_NAME, FONT_SIZE, FALSE, FALSE));

	RETURN_IF_FAILED(m_pSkinTimeline->SetFontMap(m_pThemeFontMap));
	RETURN_IF_FAILED(m_pSkinTabControl->SetFontMap(m_pThemeFontMap));
	RETURN_IF_FAILED(m_pSkinCommonControl->SetFontMap(m_pThemeFontMap));

	return S_OK;
}

STDMETHODIMP CThemeDefault::GetTimelineSkin(ISkinTimeline** ppSkinTimeline)
{
	CHECK_E_POINTER(ppSkinTimeline);
	RETURN_IF_FAILED(m_pSkinTimeline->QueryInterface(ppSkinTimeline));
	return S_OK;
}

STDMETHODIMP CThemeDefault::GetTabControlSkin(ISkinTabControl** pSkinTabControl)
{
	CHECK_E_POINTER(pSkinTabControl);
	RETURN_IF_FAILED(m_pSkinTabControl->QueryInterface(pSkinTabControl));
	return S_OK;
}

STDMETHODIMP CThemeDefault::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	CHECK_E_POINTER(pThemeColorMap);
	m_pThemeColorMap = pThemeColorMap;
	RETURN_IF_FAILED(m_pSkinTimeline->SetColorMap(m_pThemeColorMap));
	return S_OK;
}

STDMETHODIMP CThemeDefault::SetImageManagerService(IImageManagerService* pImageManagerService)
{
	RETURN_IF_FAILED(m_pSkinTimeline->SetImageManagerService(pImageManagerService));
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