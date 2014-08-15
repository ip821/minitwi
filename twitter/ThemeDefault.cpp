// SkinDefault.cpp : Implementation of CSkinDefault

#include "stdafx.h"
#include "ThemeDefault.h"
#include "Plugins.h"

// CSkinDefault

HRESULT CThemeDefault::FinalConstruct()
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinTimeline, &m_pSkinTimeline));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ThemeColorMap, &m_pThemeColorMap));
	RETURN_IF_FAILED(m_pSkinTimeline->SetColorMap(m_pThemeColorMap));

	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_BRUSH_BACKGROUND, Gdiplus::Color::White));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_BRUSH_SELECTED, Gdiplus::Color::Beige));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_USER_DISPLAY_NAME, Gdiplus::Color::SteelBlue));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_USER_NAME, Gdiplus::Color::Gray));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_TEXT, Gdiplus::Color::Black));
	RETURN_IF_FAILED(m_pThemeColorMap->SetColor(VAR_TWITTER_DELIMITER, Gdiplus::Color::LightGray));

	return S_OK;
}

STDMETHODIMP CThemeDefault::GetTimelineSkin(ISkinTimeline** ppSkinTimeline)
{
	CHECK_E_POINTER(ppSkinTimeline);
	RETURN_IF_FAILED(m_pSkinTimeline->QueryInterface(ppSkinTimeline));
	return S_OK;
}

STDMETHODIMP CThemeDefault::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	CHECK_E_POINTER(pThemeColorMap);
	m_pThemeColorMap = pThemeColorMap;
	RETURN_IF_FAILED(m_pSkinTimeline->SetColorMap(m_pThemeColorMap));
	return S_OK;
}