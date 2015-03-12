// SkinTabControl.cpp : Implementation of CSkinTabControl

#include "stdafx.h"
#include "SkinTabControl.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"
#include "GdilPlusUtils.h"

// CSkinTabControl

const size_t MAX_COUNT = 3;
const int ITEM_SIZE = 10;
const int ITEM_DISTANCE = 5;
const int ITEM_OFFSET_Y = 1;
const int TOOLTIP_ID = 1;
#define ITEM_DELIMITER_HEIGHT 1

#define PADDING_Y 5
#define PADDING_X 10
#define IMAGE_TO_TEXT_DISTANCE 0

#define INDEX_CONTROL_HOME 0
#define INDEX_CONTROL_LISTS 1
#define INDEX_CONTROL_SEARCH 2
#define INDEX_CONTROL_SETTINGS 3

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
	CComPtr<IStream> pImageStream;
	pImageStream.Attach(SHCreateMemStream((LPBYTE)pvResourceData, dwSizeInBytes));
	pBitmap = shared_ptr<Gdiplus::Bitmap>(Gdiplus::Bitmap::FromStream(pImageStream));
	return S_OK;
}

void CSkinTabControl::FinalRelease()
{

}

HRESULT CSkinTabControl::FinalConstruct()
{
	InitImageFromResource(IDR_PICTUREHOME, L"PNG", m_pBitmapHome);
	InitImageFromResource(IDR_PICTURESEARCH, L"PNG", m_pBitmapSearch);
	InitImageFromResource(IDR_PICTURESETTINGS, L"PNG", m_pBitmapSettings);
	InitImageFromResource(IDR_PICTUREERROR, L"PNG", m_pBitmapError);
	InitImageFromResource(IDR_PICTUREINFO, L"PNG", m_pBitmapInfo);
	InitImageFromResource(IDR_PICTURELISTS, L"PNG", m_pBitmapLists);
	return S_OK;
}

CSkinTabControl::CSkinTabControl()
{
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

STDMETHODIMP CSkinTabControl::MeasureHeader(HWND hWnd, IObjArray* pObjArray, IColumnsInfo* pColumnsInfo, RECT* clientRect, UINT* puiHeight)
{
	m_hWnd = hWnd;

	CRect clientRect2 = clientRect;
	CClientDC hdc(hWnd);
	CDC cdc;
	cdc.CreateCompatibleDC(hdc);

	HFONT font = 0;
	m_pThemeFontMap->GetFont(Twitter::Metadata::Tabs::Header, &font);
	CDCSelectFontScope cdcSelectFontScope(cdc, font);

	UINT uiHeight = max(m_pBitmapHome->GetHeight(), m_pBitmapSettings->GetHeight()) + PADDING_Y * 2;

	*puiHeight = uiHeight;

	CRect rectHomeColumn;
	{
		auto x = PADDING_X;
		auto y = PADDING_Y;
		rectHomeColumn = CRect(x, y, x + (int)m_pBitmapHome->GetWidth(), y + (int)m_pBitmapHome->GetHeight());

		CComPtr<IControl2> pControl2;
		RETURN_IF_FAILED(pObjArray->GetAt(INDEX_CONTROL_HOME, __uuidof(IControl2), (LPVOID*)&pControl2));
		CComBSTR bstr;
		RETURN_IF_FAILED(pControl2->GetText(&bstr));

		CSize sz;
		GetTextExtentPoint32(cdc, bstr, bstr.Length(), &sz);
		rectHomeColumn.right += PADDING_X + IMAGE_TO_TEXT_DISTANCE + sz.cx;

		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(pColumnsInfo->AddItem(&pColumnsInfoItem));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRect(rectHomeColumn));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Object::Text, bstr));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(Twitter::Metadata::Tabs::HeaderSelected, FALSE));
	}

	CRect rectListsColumn;
	{
		auto x = rectHomeColumn.right + PADDING_X;
		auto y = PADDING_Y;
		rectListsColumn = CRect(x, y, x + (int)m_pBitmapLists->GetWidth(), y + (int)m_pBitmapLists->GetHeight());

		CComPtr<IControl2> pControl2;
		RETURN_IF_FAILED(pObjArray->GetAt(INDEX_CONTROL_LISTS, __uuidof(IControl2), (LPVOID*)&pControl2));
		CComBSTR bstr;
		RETURN_IF_FAILED(pControl2->GetText(&bstr));

		CSize sz;
		GetTextExtentPoint32(hdc, bstr, bstr.Length(), &sz);
		rectListsColumn.right += PADDING_X + IMAGE_TO_TEXT_DISTANCE + sz.cx;

		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(pColumnsInfo->AddItem(&pColumnsInfoItem));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRect(rectListsColumn));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Object::Text, bstr));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(Twitter::Metadata::Tabs::HeaderSelected, FALSE));
	}

	CRect rectSearchColumn;
	{
		auto x = rectListsColumn.right + PADDING_X;
		auto y = PADDING_Y;
		rectSearchColumn = CRect(x, y, x + (int)m_pBitmapSearch->GetWidth(), y + (int)m_pBitmapSearch->GetHeight());

		CComPtr<IControl2> pControl2;
		RETURN_IF_FAILED(pObjArray->GetAt(INDEX_CONTROL_SEARCH, __uuidof(IControl2), (LPVOID*)&pControl2));
		CComBSTR bstr;
		RETURN_IF_FAILED(pControl2->GetText(&bstr));

		CSize sz;
		GetTextExtentPoint32(hdc, bstr, bstr.Length(), &sz);
		rectSearchColumn.right += PADDING_X + IMAGE_TO_TEXT_DISTANCE + sz.cx;

		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(pColumnsInfo->AddItem(&pColumnsInfoItem));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRect(rectSearchColumn));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Object::Text, bstr));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(Twitter::Metadata::Tabs::HeaderSelected, FALSE));
	}

	{
		auto x = rectSearchColumn.right + PADDING_X;
		auto y = PADDING_Y;
		CRect rect = { x, y, x + (int)m_pBitmapSearch->GetWidth(), y + (int)m_pBitmapSearch->GetHeight() };

		CComPtr<IControl2> pControl2;
		RETURN_IF_FAILED(pObjArray->GetAt(INDEX_CONTROL_SETTINGS, __uuidof(IControl2), (LPVOID*)&pControl2));
		CComBSTR bstr;
		RETURN_IF_FAILED(pControl2->GetText(&bstr));

		CSize sz;
		GetTextExtentPoint32(hdc, bstr, bstr.Length(), &sz);
		rect.right += PADDING_X + IMAGE_TO_TEXT_DISTANCE + sz.cx;

		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(pColumnsInfo->AddItem(&pColumnsInfoItem));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRect(rect));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Object::Text, bstr));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(Twitter::Metadata::Tabs::HeaderSelected, FALSE));
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
	RETURN_IF_FAILED(m_pThemeColorMap->GetColor(Twitter::Metadata::Drawing::BrushBackground, &dwColor));
	CBrush brush;
	brush.CreateSolidBrush(dwColor);
	cdc.FillRect(m_rectHeader, brush);
	return S_OK;
}

STDMETHODIMP CSkinTabControl::DrawHeader(IColumnsInfo* pColumnsInfo, HDC hdc, RECT rect)
{
	CDCHandle cdc(hdc);
	cdc.SetBkMode(TRANSPARENT);
	RETURN_IF_FAILED(DrawTabs(pColumnsInfo, cdc, rect));
	return S_OK;
}

STDMETHODIMP CSkinTabControl::DrawTabs(IColumnsInfo* pColumnsInfo, CDCHandle& cdc, RECT rect)
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(pColumnsInfo->GetCount(&uiCount));

	{
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(Twitter::Metadata::Item::VAR_TWITTER_DELIMITER, &dwColor));
		CBrush brush;
		brush.CreateSolidBrush(dwColor);
#pragma warning(suppress: 6246)
		CRect rect = m_rectHeader;
		rect.top = rect.bottom - ITEM_DELIMITER_HEIGHT;
		cdc.FillRect(&rect, brush);
	}

	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(pColumnsInfo->GetItem(i, &pColumnsInfoItem));

		CRect rect;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRect(&rect));

		UINT imageWidth = 0;
		UINT imageHeight = 0;
		CBitmap bitmap;
		if (i == INDEX_CONTROL_HOME)
		{
			m_pBitmapHome->GetHBITMAP(Gdiplus::Color::Transparent, &bitmap.m_hBitmap);
			imageWidth = m_pBitmapHome->GetWidth();
			imageHeight = m_pBitmapHome->GetHeight();
		}
		else if (i == INDEX_CONTROL_LISTS)
		{
			m_pBitmapLists->GetHBITMAP(Gdiplus::Color::Transparent, &bitmap.m_hBitmap);
			imageWidth = m_pBitmapLists->GetWidth();
			imageHeight = m_pBitmapLists->GetHeight();
		}
		else if (i == INDEX_CONTROL_SEARCH)
		{
			m_pBitmapSearch->GetHBITMAP(Gdiplus::Color::Transparent, &bitmap.m_hBitmap);
			imageWidth = m_pBitmapSearch->GetWidth();
			imageHeight = m_pBitmapSearch->GetHeight();
		}
		else if (i == INDEX_CONTROL_SETTINGS)
		{
			m_pBitmapSettings->GetHBITMAP(Gdiplus::Color::Transparent, &bitmap.m_hBitmap);
			imageWidth = m_pBitmapSettings->GetWidth();
			imageHeight = m_pBitmapSettings->GetHeight();
		}

		CDC cdcBitmap;
		cdcBitmap.CreateCompatibleDC(cdc);
		CDCSelectBitmapScope cdcSelectBitmapScope(cdcBitmap, bitmap);
		
		auto x = rect.left;
		auto y = rect.top;
		auto width = imageWidth;
		auto height = imageHeight;
		static Gdiplus::Color color(Gdiplus::Color::Transparent);
		TransparentBlt(cdc, x, y, width, height, cdcBitmap, 0, 0, width, height, color.ToCOLORREF());

		CComBSTR bstr;
		pColumnsInfoItem->GetRectStringProp(Twitter::Metadata::Object::Text, &bstr);

		HFONT font = 0;
		m_pThemeFontMap->GetFont(Twitter::Metadata::Tabs::Header, &font);
		CDCSelectFontScope cdcSelectFontScope(cdc, font);

		CRect rectText = rect;
		rectText.left += imageWidth + PADDING_X + IMAGE_TO_TEXT_DISTANCE;

		CSize sz;
		GetTextExtentPoint32(cdc, bstr, bstr.Length(), &sz);
		rectText.top = rect.top + ((rect.Height() / 2) - sz.cy / 2);

		BOOL bSelected = FALSE;
		pColumnsInfoItem->GetRectBoolProp(Twitter::Metadata::Tabs::HeaderSelected, &bSelected);

		DWORD dwColor = 0;
		if (bSelected)
		{
			RETURN_IF_FAILED(m_pThemeColorMap->GetColor(Twitter::Metadata::Tabs::HeaderSelected, &dwColor));
		}
		else
		{
			RETURN_IF_FAILED(m_pThemeColorMap->GetColor(Twitter::Metadata::Tabs::Header, &dwColor));
		}
		cdc.SetTextColor(dwColor);

		DrawText(cdc, bstr, bstr.Length(), &rectText, 0);
	}
	return S_OK;
}

STDMETHODIMP CSkinTabControl::DrawAnimation(HDC hdc)
{
	CDCHandle cdc(hdc);

	int left = ITEM_SIZE * MAX_COUNT + ITEM_DISTANCE * MAX_COUNT;
	CRect rect = m_rectHeader;
	rect.left = rect.right - left;

	rect.top += rect.Height() / 2 - ITEM_SIZE / 2 + ITEM_OFFSET_Y;

	DWORD dwActiveColor = 0;
	m_pThemeColorMap->GetColor(Twitter::Metadata::Item::VAR_ITEM_ANIMATION_ACTIVE, &dwActiveColor);
	DWORD dwInactiveColor = 0;
	m_pThemeColorMap->GetColor(Twitter::Metadata::Item::VAR_ITEM_ANIMATION_INACTIVE, &dwInactiveColor);

	CBrush brushActive;
	brushActive.CreateSolidBrush(dwActiveColor);
	CBrush brushInactive;
	brushInactive.CreateSolidBrush(dwInactiveColor);

	for (int i = 0; i < MAX_COUNT; i++)
	{
		auto x = rect.left + ITEM_SIZE * i + ITEM_DISTANCE * (max(0, i));
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
	CDCSelectBitmapScope cdcSelectBitmapScope(cdcBitmap, bitmap);
	
	auto x = m_rectInfoImage.left;
	auto y = m_rectInfoImage.top;
	auto width = imageWidth;
	auto height = imageHeight;
	static Gdiplus::Color color(Gdiplus::Color::Transparent);
	TransparentBlt(hdc, x, y, width, height, cdcBitmap, 0, 0, width, height, color.ToCOLORREF());
	return S_OK;
}

STDMETHODIMP CSkinTabControl::StartInfoImage()
{
	if (m_wndTooltip.IsWindow())
	{
		m_wndTooltip.DestroyWindow();
	}

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
	return S_OK;
}

STDMETHODIMP CSkinTabControl::StopInfoImage()
{
	if (m_wndTooltip.IsWindow())
		m_wndTooltip.DestroyWindow();
	return S_OK;
}

STDMETHODIMP CSkinTabControl::AnimationGetParams(UINT* puiMilliseconds)
{
	CHECK_E_POINTER(puiMilliseconds);
	*puiMilliseconds = 200;
	return S_OK;
}

STDMETHODIMP CSkinTabControl::AnimationNextFrame()
{
	m_iFrameCount++;
	if (m_iFrameCount == MAX_COUNT)
		m_iFrameCount = 0;
	return S_OK;
}

STDMETHODIMP CSkinTabControl::GetInfoRect(RECT* pRect)
{
	CHECK_E_POINTER(pRect);
	*pRect = m_rectInfoImage;
	return S_OK;
}
