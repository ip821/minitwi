#include "stdafx.h"
#include "ThemeFontMap.h"

STDMETHODIMP CThemeFontMap::GetFont(BSTR bstrColorName, HFONT* phFont)
{
	CHECK_E_POINTER(phFont);
	*phFont = m_fonts[bstrColorName];
	return S_OK;
}

STDMETHODIMP CThemeFontMap::SetFont(BSTR bstrFontName, BSTR bstrFontFamily, DWORD dwSize, BOOL bBold, BOOL bUnderline)
{
	LOGFONT logFont = { 0 };
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = dwSize * 10;
	SecureHelper::strncpy_x(logFont.lfFaceName, _countof(logFont.lfFaceName), bstrFontFamily, _TRUNCATE);

	if (bBold)
		logFont.lfWeight = FW_BOLD;

	if (bUnderline)
		logFont.lfUnderline = TRUE;

	CFont font;
	font.CreatePointFontIndirect(&logFont);
	m_fonts[bstrFontName] = font.Detach();
	return S_OK;
}

STDMETHODIMP CThemeFontMap::Initialize(IObjCollection* pObjectCollection)
{
	CHECK_E_POINTER(pObjectCollection);
	UINT_PTR uiCount = 0;
	RETURN_IF_FAILED(pObjectCollection->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pFontObject;
		RETURN_IF_FAILED(pObjectCollection->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pFontObject));

		CComVariant vName;
		CComVariant vFontFamily;
		CComVariant vSize;
		CComVariant vBold;
		CComVariant vUnderline;

		RETURN_IF_FAILED(pFontObject->GetVariantValue(L"name", &vName));
		RETURN_IF_FAILED(pFontObject->GetVariantValue(L"family", &vFontFamily));
		RETURN_IF_FAILED(pFontObject->GetVariantValue(L"size", &vSize));
		RETURN_IF_FAILED(pFontObject->GetVariantValue(L"bold", &vBold));
		RETURN_IF_FAILED(pFontObject->GetVariantValue(L"underline", &vUnderline));

		ATLASSERT(vName.vt == VT_BSTR && vFontFamily.vt == VT_BSTR && vSize.vt == VT_R8 && vBold.vt == VT_BOOL && vUnderline.vt == VT_BOOL);

		SetFont(vName.bstrVal, vFontFamily.bstrVal, (DWORD)vSize.dblVal, vBold.boolVal, vUnderline.boolVal);
	}
	return S_OK;
}