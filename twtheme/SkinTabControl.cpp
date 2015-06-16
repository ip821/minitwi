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

STDMETHODIMP CSkinTabControl::GetResourceStream(int nId, LPCTSTR lpType, IStream** ppStream)
{
	CHECK_E_POINTER(ppStream);
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
	RETURN_IF_FAILED(pImageStream->QueryInterface(ppStream));
	return S_OK;
}

STDMETHODIMP CSkinTabControl::InitImageFromResource(int nId, LPCTSTR lpType, shared_ptr<Gdiplus::Bitmap>& pBitmap)
{
	CComPtr<IStream> pStream;
	RETURN_IF_FAILED(GetResourceStream(nId, lpType, &pStream));
	pBitmap = shared_ptr<Gdiplus::Bitmap>(Gdiplus::Bitmap::FromStream(pStream));
	return S_OK;
}

STDMETHODIMP CSkinTabControl::AppendToImageManagerService(int nId, LPCTSTR lpType, BSTR bstrKey, IImageManagerService* pImageManagerService)
{
	CComPtr<IStream> pStream;
	RETURN_IF_FAILED(GetResourceStream(nId, lpType, &pStream));
	RETURN_IF_FAILED(pImageManagerService->AddImageFromStream(bstrKey, pStream));
	return S_OK;
}

void CSkinTabControl::FinalRelease()
{

}

HRESULT CSkinTabControl::FinalConstruct()
{
	InitImageFromResource(IDR_PICTUREERROR, L"PNG", m_pBitmapError);
	InitImageFromResource(IDR_PICTUREINFO, L"PNG", m_pBitmapInfo);
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ImageManagerService, &m_pImageManagerService));
	RETURN_IF_FAILED(AppendToImageManagerService(IDR_PICTUREHOME, L"PNG", Twitter::Themes::Metadata::TabContainer::Images::Home, m_pImageManagerService));
	RETURN_IF_FAILED(AppendToImageManagerService(IDR_PICTURELISTS, L"PNG", Twitter::Themes::Metadata::TabContainer::Images::Lists, m_pImageManagerService));
	RETURN_IF_FAILED(AppendToImageManagerService(IDR_PICTURESEARCH, L"PNG", Twitter::Themes::Metadata::TabContainer::Images::Search, m_pImageManagerService));
	RETURN_IF_FAILED(AppendToImageManagerService(IDR_PICTURESETTINGS, L"PNG", Twitter::Themes::Metadata::TabContainer::Images::Settings, m_pImageManagerService));
	return S_OK;
}

CSkinTabControl::CSkinTabControl()
{
}

STDMETHODIMP CSkinTabControl::SetTheme(ITheme* pTheme)
{
	CHECK_E_POINTER(pTheme);
	m_pTheme = pTheme;
	RETURN_IF_FAILED(m_pTheme->GetColorMap(&m_pThemeColorMap));
	RETURN_IF_FAILED(m_pTheme->GetLayoutManager(&m_pLayoutManager));
	RETURN_IF_FAILED(m_pTheme->GetLayout(Twitter::Themes::Metadata::TabContainer::LayoutName, &m_pLayoutObject));
	return S_OK;
}

STDMETHODIMP CSkinTabControl::MeasureHeader(HWND hWnd, IObjArray* pObjArray, IColumnsInfo* pColumnsInfo, RECT* clientRect, UINT* puiHeight)
{
	m_hWnd = hWnd;

	CClientDC hdc(hWnd);
	CRect resultRect;
	RETURN_IF_FAILED(m_pLayoutManager->BuildLayout(hdc, clientRect, &resultRect, m_pLayoutObject, nullptr, m_pImageManagerService, pColumnsInfo));
	UINT uiInex = 0;
	RETURN_IF_FAILED(pColumnsInfo->FindItemIndex(Twitter::Themes::Metadata::TabContainer::RootContainerName, &uiInex));
	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	RETURN_IF_FAILED(pColumnsInfo->GetItem(uiInex, &pColumnsInfoItem));
	RETURN_IF_FAILED(pColumnsInfoItem->GetRect(&m_rectHeader));
	return S_OK;
}

STDMETHODIMP CSkinTabControl::EraseBackground(IColumnsInfo* pColumnsInfo, HDC hdc)
{
	if (m_rectHeader.IsRectEmpty())
		return S_OK;

	RETURN_IF_FAILED(m_pLayoutManager->EraseBackground(hdc, pColumnsInfo));
	return S_OK;
}

STDMETHODIMP CSkinTabControl::DrawHeader(IColumnsInfo* pColumnsInfo, HDC hdc, RECT rect)
{
	CDCHandle cdc(hdc);
	cdc.SetBkMode(TRANSPARENT);
	RETURN_IF_FAILED(m_pLayoutManager->PaintLayout(cdc, m_pImageManagerService, pColumnsInfo));
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
	m_pThemeColorMap->GetColor(Twitter::Metadata::Item::TwitterItemAnimationActive, &dwActiveColor);
	DWORD dwInactiveColor = 0;
	m_pThemeColorMap->GetColor(Twitter::Metadata::Item::TwitterItemAnimationInactive, &dwInactiveColor);

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
