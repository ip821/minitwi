// ThemeColorMap.cpp : Implementation of CThemeColorMap

#include "stdafx.h"
#include "ThemeColorMap.h"


// CThemeColorMap

HRESULT CThemeColorMap::FinalConstruct()
{
	m_colors[VAR_TEXT] = Gdiplus::Color::LightSteelBlue;
	return S_OK;
}

STDMETHODIMP CThemeColorMap::GetColor(BSTR bstrColorName, DWORD* dwColor)
{
	return S_OK;
}

STDMETHODIMP CThemeColorMap::SetColor(BSTR bstrColorName, DWORD dwColor)
{
	return S_OK;
}
