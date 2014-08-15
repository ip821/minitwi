// ThemeColorMap.cpp : Implementation of CThemeColorMap

#include "stdafx.h"
#include "ThemeColorMap.h"


// CThemeColorMap

HRESULT CThemeColorMap::FinalConstruct()
{
	return S_OK;
}

STDMETHODIMP CThemeColorMap::GetColor(BSTR bstrColorName, DWORD* dwColor)
{
	CHECK_E_POINTER(dwColor);
	*dwColor = Gdiplus::Color(m_colors[bstrColorName]).ToCOLORREF();
	return S_OK;
}

STDMETHODIMP CThemeColorMap::SetColor(BSTR bstrColorName, DWORD dwColor)
{
	m_colors[bstrColorName] = dwColor;
	return S_OK;
}
