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

STDMETHODIMP CThemeColorMap::Initialize(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	UINT_PTR uiCount = 0;
	RETURN_IF_FAILED(pVariantObject->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComBSTR bstrKey;
		RETURN_IF_FAILED(pVariantObject->GetKeyByIndex(i, &bstrKey));
		CComVariant v;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(bstrKey, &v));
		ATLASSERT(v.vt == VT_UI4);
		SetColor(bstrKey, v.ulVal);
	}
	return S_OK;
}