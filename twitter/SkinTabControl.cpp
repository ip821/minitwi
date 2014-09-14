// SkinTabControl.cpp : Implementation of CSkinTabControl

#include "stdafx.h"
#include "SkinTabControl.h"
#include "Plugins.h"

// CSkinTabControl

const size_t MAX_COUNT = 3;
const int ITEM_SIZE = 10;
const int ITEM_DISTANCE = 5;
const int ITEM_OFFSET_Y = 2;
const int TOOLTIP_ID = 1;

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
	InitImageFromResource(IDR_PICTUREHOME, L"PNG", m_pBitmapHome);
	InitImageFromResource(IDR_PICTURESETTINGS, L"PNG", m_pBitmapSettings);
	InitImageFromResource(IDR_PICTUREERROR, L"PNG", m_pBitmapError);
	InitImageFromResource(IDR_PICTUREINFO, L"PNG", m_pBitmapInfo);
}

STDMETHODIMP CSkinTabControl::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	CHECK_E_POINTER(pThemeColorMap);
	m_pThemeColorMap = pThemeColorMap;
	return S_OK;
}

STDMETHODIMP CSkinTabControl::GetColorMap(IThemeColorMap** ppThemeColorMap)
{
	CHECK_E_POINTER(ppThemeColorMap);
	m_pThemeColorMap->QueryInterface(ppThemeColorMap);
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
#define IMAGE_TO_TEXT_DISTANCE 0

STDMETHODIMP CSkinTabControl::MeasureHeader(HWND hWnd, IObjArray* pObjArray, IColumnRects* pColumnRects, RECT* clientRect, UINT* puiHeight)
{
	m_hWnd = hWnd;

	CRect clientRect2 = clientRect;
	CDC hdc(GetDC(hWnd));
	CDC cdc;
	cdc.CreateCompatibleDC(hdc);

	HFONT font = 0;
	m_pThemeFontMap->GetFont(CComBSTR(VAR_TAB_HEADER), &font);
	cdc.SelectFont(font);

	UINT uiHeight = max(m_pBitmapHome->GetHeight(), m_pBitmapSettings->GetHeight()) + PADDING_Y * 2;

	*puiHeight = uiHeight;

	CRect rectHomeColumn;
	{
		UINT uiIndex = 0;
		auto x = PADDING_X;
		auto y = PADDING_Y;
		rectHomeColumn = CRect(x, y, x + (int)m_pBitmapHome->GetWidth(), y + (int)m_pBitmapHome->GetHeight());

		CComPtr<IControl2> pControl2;
		RETURN_IF_FAILED(pObjArray->GetAt(0, __uuidof(IControl2), (LPVOID*)&pControl2));
		CComBSTR bstr;
		RETURN_IF_FAILED(pControl2->GetText(&bstr));

		CSize sz;
		GetTextExtentPoint32(cdc, bstr, bstr.Length(), &sz);
		rectHomeColumn.right += PADDING_X + IMAGE_TO_TEXT_DISTANCE + sz.cx;

		RETURN_IF_FAILED(pColumnRects->AddRect(rectHomeColumn, &uiIndex));
		RETURN_IF_FAILED(pColumnRects->SetRectStringProp(uiIndex, VAR_TEXT, bstr));
		RETURN_IF_FAILED(pColumnRects->SetRectBoolProp(0, VAR_TAB_HEADER_SELECTED, FALSE));
	}

	{
		UINT uiIndex = 0;
		auto x = rectHomeColumn.right + PADDING_X;
		auto y = PADDING_Y;
		CRect rect = { x, y, x + (int)m_pBitmapSettings->GetWidth(), y + (int)m_pBitmapSettings->GetHeight() };

		CComPtr<IControl2> pControl2;
		RETURN_IF_FAILED(pObjArray->GetAt(1, __uuidof(IControl2), (LPVOID*)&pControl2));
		CComBSTR bstr;
		RETURN_IF_FAILED(pControl2->GetText(&bstr));

		CSize sz;
		GetTextExtentPoint32(hdc, bstr, bstr.Length(), &sz);
		rect.right += PADDING_X + IMAGE_TO_TEXT_DISTANCE + sz.cx;

		RETURN_IF_FAILED(pColumnRects->AddRect(rect, &uiIndex));
		RETURN_IF_FAILED(pColumnRects->SetRectStringProp(uiIndex, VAR_TEXT, bstr));
		RETURN_IF_FAILED(pColumnRects->SetRectBoolProp(1, VAR_TAB_HEADER_SELECTED, FALSE));
	}

	m_rectHeader = clientRect;
	m_rectHeader.bottom = m_rectHeader.top + uiHeight;

	{ // info image rect
		int imageWidth = m_pBitmapError->GetWidth();
		int imageHeight = m_pBitmapError->GetHeight();

		int left = imageWidth + ITEM_DISTANCE;
		m_rectInfoImage = m_rectHeader;
		m_rectInfoImage.left = m_rectInfoImage.right - left;
		m_rectInfoImage.top += m_rectInfoImage.Height() / 2 - imageHeight / 2 + ITEM_OFFSET_Y;
	}

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

STDMETHODIMP CSkinTabControl::DrawHeader(IColumnRects* pColumnRects, HDC hdc, RECT rect, int selectedPageIndex)
{
	CDCHandle cdc(hdc);
	cdc.SetBkMode(TRANSPARENT);
	RETURN_IF_FAILED(DrawTabs(pColumnRects, cdc, rect, selectedPageIndex));
	return S_OK;
}

STDMETHODIMP CSkinTabControl::DrawTabs(IColumnRects* pColumnRects, CDCHandle& cdc, RECT rect, int selectedPageIndex)
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(pColumnRects->GetCount(&uiCount));

	for (size_t i = 0; i < uiCount; i++)
	{
		CRect rect;
		RETURN_IF_FAILED(pColumnRects->GetRect(i, &rect));

		UINT imageWidth = 0;
		UINT imageHeight = 0;
		CBitmap bitmap;
		if (i == 0)
		{
			m_pBitmapHome->GetHBITMAP(Gdiplus::Color::Transparent, &bitmap.m_hBitmap);
			imageWidth = m_pBitmapHome->GetWidth();
			imageHeight = m_pBitmapHome->GetHeight();
		}
		else if (i == 1)
		{
			m_pBitmapSettings->GetHBITMAP(Gdiplus::Color::Transparent, &bitmap.m_hBitmap);
			imageWidth = m_pBitmapSettings->GetWidth();
			imageHeight = m_pBitmapSettings->GetHeight();
		}

		CDC cdcBitmap;
		cdcBitmap.CreateCompatibleDC(cdc);
		cdcBitmap.SelectBitmap(bitmap);
		auto x = rect.left;
		auto y = rect.top;
		auto width = imageWidth;
		auto height = imageHeight;
		static Gdiplus::Color color(Gdiplus::Color::Transparent);
		TransparentBlt(cdc, x, y, width, height, cdcBitmap, 0, 0, width, height, color.ToCOLORREF());

		CComBSTR bstr;
		pColumnRects->GetRectStringProp(i, VAR_TEXT, &bstr);

		HFONT font = 0;
		m_pThemeFontMap->GetFont(CComBSTR(VAR_TAB_HEADER), &font);
		cdc.SelectFont(font);

		CRect rectText = rect;
		rectText.left += imageWidth + PADDING_X + IMAGE_TO_TEXT_DISTANCE;

		CSize sz;
		GetTextExtentPoint32(cdc, bstr, bstr.Length(), &sz);
		rectText.top = rect.top + ((rect.Height() / 2) - sz.cy / 2);

		BOOL bSelected = selectedPageIndex == i;

		DWORD dwColor = 0;
		if (bSelected)
		{
			RETURN_IF_FAILED(m_pThemeColorMap->GetColor(VAR_TAB_HEADER_SELECTED, &dwColor));
		}
		else
		{
			RETURN_IF_FAILED(m_pThemeColorMap->GetColor(VAR_TAB_HEADER, &dwColor));
		}
		cdc.SetTextColor(dwColor);

		DrawText(cdc, bstr, bstr.Length(), &rectText, 0);
	}
	return S_OK;
}

STDMETHODIMP CSkinTabControl::DrawAnimation(HDC hdc)
{
	CDCHandle cdc(hdc);

	m_iFrameCount++;
	if (m_iFrameCount == MAX_COUNT)
		m_iFrameCount = 0;

	int left = ITEM_SIZE * MAX_COUNT + ITEM_DISTANCE * MAX_COUNT;
	CRect rect = m_rectHeader;
	rect.left = rect.right - left;

	rect.top += rect.Height() / 2 - ITEM_SIZE / 2 + ITEM_OFFSET_Y;

	DWORD dwActiveColor = 0;
	m_pThemeColorMap->GetColor(VAR_ITEM_ANIMATION_ACTIVE, &dwActiveColor);
	DWORD dwInactiveColor = 0;
	m_pThemeColorMap->GetColor(VAR_ITEM_ANIMATION_INACTIVE, &dwInactiveColor);

	CBrush brushActive;
	brushActive.CreateSolidBrush(dwActiveColor);
	CBrush brushInactive;
	brushInactive.CreateSolidBrush(dwInactiveColor);

	for (size_t i = 0; i < MAX_COUNT; i++)
	{
		auto x = rect.left + ITEM_SIZE * i + ITEM_DISTANCE * (max(0, i - 1));
		auto y = rect.top;
		CRect rectItem = { (int)x, y, (int)x + ITEM_SIZE, y + ITEM_SIZE };
		cdc.FillRect(rectItem, i == m_iFrameCount ? brushActive : brushInactive);
	}
	return S_OK;
}

STDMETHODIMP CSkinTabControl::DrawInfoImage(HDC hdc, BOOL bError, BSTR bstrMessage)
{
	if (m_wndTooltip.IsWindow())
	{
		m_wndTooltip.UpdateTipText(bstrMessage, m_hWnd, TOOLTIP_ID);
	}

	int imageWidth = 0;
	int imageHeight = 0;
	CBitmap bitmap;
	if (bError)
	{
		imageWidth = m_pBitmapError->GetWidth();
		imageHeight = m_pBitmapError->GetHeight();
		m_pBitmapError->GetHBITMAP(Gdiplus::Color::Transparent, &bitmap.m_hBitmap);
	}
	else
	{
		imageWidth = m_pBitmapInfo->GetWidth();
		imageHeight = m_pBitmapInfo->GetHeight();
		m_pBitmapInfo->GetHBITMAP(Gdiplus::Color::Transparent, &bitmap.m_hBitmap);
	}

	CDC cdcBitmap;
	cdcBitmap.CreateCompatibleDC(hdc);
	cdcBitmap.SelectBitmap(bitmap);
	auto x = m_rectInfoImage.left;
	auto y = m_rectInfoImage.top;
	auto width = imageWidth;
	auto height = imageHeight;
	static Gdiplus::Color color(Gdiplus::Color::Transparent);
	TransparentBlt(hdc, x, y, width, height, cdcBitmap, 0, 0, width, height, color.ToCOLORREF());
	return S_OK;
}

STDMETHODIMP CSkinTabControl::Notify(TabControlNotifyReason reason)
{
	if (reason == TabControlNotifyReason::InfoImageIsOn)
	{
		if (!m_wndTooltip.IsWindow())
		{
			m_wndTooltip.Create(NULL, 0, 0, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON, WS_EX_TOPMOST);
			m_wndTooltip.SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		}

		TOOLINFO ti = { 0 };
		ti.cbSize = sizeof(ti);
		ti.hwnd = m_hWnd;
		ti.uFlags = TTF_SUBCLASS | TTF_CENTERTIP;
		ti.rect = m_rectInfoImage;
		ti.uId = TOOLTIP_ID;
		ti.hinst = NULL;
		m_wndTooltip.Activate(TRUE);
		m_wndTooltip.AddTool(&ti);
	}
	else if (reason == TabControlNotifyReason::InfoImageIsOff)
	{
		m_wndTooltip.DestroyWindow();
	}
	return S_OK;
}

STDMETHODIMP CSkinTabControl::GetInfoRect(RECT* pRect)
{
	CHECK_E_POINTER(pRect);
	*pRect = m_rectInfoImage;
	return S_OK;
}
