// SkinTabControl.cpp : Implementation of CSkinTabControl

#include "stdafx.h"
#include "SkinTabControl.h"
#include "Plugins.h"

// CSkinTabControl

STDMETHODIMP CSkinTabControl::InitImageFromResource(int nId, LPCTSTR lpType, shared_ptr<Gdiplus::Bitmap>& pBitmap)
{
	HMODULE hModule = _AtlBaseModule.GetModuleInstance();
	if (!hModule)
		return E_UNEXPECTED;

	HRSRC hRsrc = FindResource(hModule, MAKEINTRESOURCE(nId), lpType);
	if (!hRsrc)
		return HRESULT_FROM_WIN32(GetLastError());

	HGLOBAL hGlobal = LoadResource(hModule, hRsrc);

	if (!hGlobal)
		return HRESULT_FROM_WIN32(GetLastError());

	auto dwSizeInBytes = SizeofResource(hModule, hRsrc);
	LPVOID pvResourceData = LockResource(hGlobal);
	CComPtr<IStream> pImageStream = SHCreateMemStream((LPBYTE)pvResourceData, dwSizeInBytes);
	pBitmap = make_shared<Gdiplus::Bitmap>(pImageStream);
	return S_OK;
}

CSkinTabControl::CSkinTabControl()
{
	InitImageFromResource(IDR_PICTUREHOME, L"PNG", m_pBitmapHpme);
	InitImageFromResource(IDR_PICTURESETTINGS, L"PNG", m_pBitmapSettings);
}

STDMETHODIMP CSkinTabControl::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	CHECK_E_POINTER(pThemeColorMap);
	m_pThemeColorMap = pThemeColorMap;
	return S_OK;
}

STDMETHODIMP CSkinTabControl::SetFontMap(IThemeFontMap* pThemeFontMap)
{
	CHECK_E_POINTER(pThemeFontMap);
	m_pThemeFontMap = pThemeFontMap;
	return S_OK;
}

#define PADDING_Y 5
#define PADDING_X 10

STDMETHODIMP CSkinTabControl::MeasureHeader(IObjArray* pObjArray, IColumnRects* pColumnRects, RECT* clientRect, UINT* puiHeight)
{
	UINT uiHeight = max(m_pBitmapHpme->GetHeight(), m_pBitmapSettings->GetHeight()) + PADDING_Y * 2;

	*puiHeight = uiHeight;

	CRect rectHomeColumn;
	{
		UINT uiIndex = 0;
		auto x = PADDING_X;
		auto y = PADDING_Y;
		rectHomeColumn = CRect(x, y, x + (int)m_pBitmapHpme->GetWidth(), y + (int)m_pBitmapHpme->GetHeight());
		RETURN_IF_FAILED(pColumnRects->AddRect(rectHomeColumn, &uiIndex));
	}

	{
		UINT uiIndex = 0;
		auto x = rectHomeColumn.right + PADDING_X;
		auto y = PADDING_Y;
		CRect rect = { x, y, x + (int)m_pBitmapSettings->GetWidth(), y + (int)m_pBitmapSettings->GetHeight() };
		RETURN_IF_FAILED(pColumnRects->AddRect(rect, &uiIndex));
	}

	m_rectHeader = clientRect;
	m_rectHeader.bottom = m_rectHeader.top + uiHeight;

	return S_OK;
}

STDMETHODIMP CSkinTabControl::EraseBackground(HDC hdc)
{
	if (m_rectHeader.IsRectEmpty())
		return S_OK;

	CDCHandle cdc(hdc);
	DWORD dwColor = 0;
	RETURN_IF_FAILED(m_pThemeColorMap->GetColor(VAR_BRUSH_BACKGROUND, &dwColor));
	CBrush brush;
	brush.CreateSolidBrush(dwColor);
	cdc.FillRect(m_rectHeader, brush);
	return S_OK;
}

STDMETHODIMP CSkinTabControl::DrawHeader(IColumnRects* pColumnRects, HDC hdc, RECT rect)
{
	CDCHandle cdc(hdc);
	cdc.SetBkMode(TRANSPARENT);

	UINT uiCount = 0;
	RETURN_IF_FAILED(pColumnRects->GetCount(&uiCount));

	for (size_t i = 0; i < uiCount; i++)
	{
		CRect rect;
		RETURN_IF_FAILED(pColumnRects->GetRect(i, &rect));

		CBitmap bitmap;
		if (i == 0)
		{
			m_pBitmapHpme->GetHBITMAP(Gdiplus::Color::White, &bitmap.m_hBitmap);
		}
		else if (i == 1)
		{
			m_pBitmapSettings->GetHBITMAP(Gdiplus::Color::White, &bitmap.m_hBitmap);
		}

		CDC cdcBitmap;
		cdcBitmap.CreateCompatibleDC(cdc);
		cdcBitmap.SelectBitmap(bitmap);
		auto x = rect.left;
		auto y = rect.top;
		auto width = rect.right - rect.left;
		auto height = rect.bottom - rect.top;
		static Gdiplus::Color color(Gdiplus::Color::White);
		TransparentBlt(cdc, x, y, width, height, cdcBitmap, 0, 0, width, height, color.ToCOLORREF());
	}
	return S_OK;
}
