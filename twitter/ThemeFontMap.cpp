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
