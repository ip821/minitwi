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

STDMETHODIMP CThemeColorMap::Initialize(IObjCollection* pObjectCollection)
{
	CHECK_E_POINTER(pObjectCollection);
	UINT_PTR uiCount = 0;
	RETURN_IF_FAILED(pObjectCollection->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pColorObject;
		RETURN_IF_FAILED(pObjectCollection->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pColorObject));
		CComVariant vName;
		RETURN_IF_FAILED(pColorObject->GetVariantValue(L"name", &vName));
		CComVariant vColor;
		RETURN_IF_FAILED(pColorObject->GetVariantValue(L"color", &vColor));
		ATLASSERT(vName.vt == VT_BSTR && vColor.vt == VT_BSTR);
		auto value = wstring(vColor.bstrVal);
		DWORD dwColor = 0;
		if (value.substr(0, 2) == L"0x")
			dwColor = wcstoul(value.c_str(), NULL, 16);
		else
			dwColor = m_knownColors[value];
		SetColor(vName.bstrVal, dwColor);
	}
	return S_OK;
}