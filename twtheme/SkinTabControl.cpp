// SkinTabControl.cpp : Implementation of CSkinTabControl

#include "stdafx.h"
#include "SkinTabControl.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"
#include "GdilPlusUtils.h"
#include "LayoutFunctions.h"

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

	InitImageFromResource(IDR_PICTUREERROR, L"PNG", m_pBitmapError);
	InitImageFromResource(IDR_PICTUREINFO, L"PNG", m_pBitmapInfo);
	RETURN_IF_FAILED(m_pTheme->GetImageManagerService(&m_pImageManagerService));

	return S_OK;
}

STDMETHODIMP CSkinTabControl::MeasureHeader(HWND hWnd, IObjArray* pObjArray, IColumnsInfo* pColumnsInfo, RECT* clientRect, UINT* puiHeight)
{
	{
		CComPtr<IVariantObject> pElement;
		RETURN_IF_FAILED(LayoutFindItemByName(m_pLayoutObject, Twitter::Themes::Metadata::TabContainer::MarqueeProgressBox, &pElement));
		RETURN_IF_FAILED(pElement->SetVariantValue(Twitter::Themes::Metadata::Element::Visible, &CComVariant(m_bAnimation == TRUE)));
	}

	{
		CComPtr<IVariantObject> pElement;
		RETURN_IF_FAILED(LayoutFindItemByName(m_pLayoutObject, Twitter::Themes::Metadata::TabContainer::InfoImage, &pElement));

		if (m_bstrMessage == L"")
		{
			RETURN_IF_FAILED(pElement->SetVariantValue(Twitter::Themes::Metadata::Element::Visible, &CComVariant(false)));
		}
		else
		{
			RETURN_IF_FAILED(pElement->SetVariantValue(Twitter::Themes::Metadata::Element::Visible, &CComVariant(true)));

			if (m_bError)
			{
				RETURN_IF_FAILED(pElement->SetVariantValue(Twitter::Themes::Metadata::ImageColumn::ImageKey, &CComVariant(Twitter::Themes::Metadata::TabContainer::Images::Error)));
			}
			else
			{
				RETURN_IF_FAILED(pElement->SetVariantValue(Twitter::Themes::Metadata::ImageColumn::ImageKey, &CComVariant(Twitter::Themes::Metadata::TabContainer::Images::Info)));
			}
		}
	}

	CClientDC hdc(hWnd);
	CRect resultRect;
	RETURN_IF_FAILED(m_pLayoutManager->BuildLayout(hdc, clientRect, &resultRect, m_pLayoutObject, nullptr, m_pImageManagerService, pColumnsInfo));
	{
		UINT uiIndex = 0;
		RETURN_IF_FAILED(pColumnsInfo->FindItemIndex(Twitter::Themes::Metadata::TabContainer::LayoutName, &uiIndex));
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(pColumnsInfo->GetItem(uiIndex, &pColumnsInfoItem));
		RETURN_IF_FAILED(pColumnsInfoItem->GetRect(&m_rectHeader));
		*puiHeight = m_rectHeader.Height();
	}

	{
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(pColumnsInfo->FindItemByName(Twitter::Themes::Metadata::TabContainer::InfoContainer, &pColumnsInfoItem));
		RETURN_IF_FAILED(pColumnsInfoItem->GetRect(&m_rectInfoImage));
	}

	if (m_wndTooltip.IsWindow())
	{
		m_wndTooltip.DestroyWindow();
	}

	if (m_bstrMessage != L"")
	{
		if (!m_wndTooltip.IsWindow())
		{
			m_wndTooltip.Create(NULL, 0, 0, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON, WS_EX_TOPMOST);
			m_wndTooltip.SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		}

		m_wndTooltip.UpdateTipText(m_bstrMessage.m_str, m_hWnd, TOOLTIP_ID);

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

	m_pColumnsInfo = pColumnsInfo;
	return S_OK;
}

STDMETHODIMP CSkinTabControl::EraseBackground(IColumnsInfo* pColumnsInfo, HDC hdc)
{
	if (m_rectHeader.IsRectEmpty())
		return S_OK;

	RETURN_IF_FAILED(m_pLayoutManager->EraseBackground(hdc, pColumnsInfo));
	return S_OK;
}

STDMETHODIMP CSkinTabControl::DrawHeader(HDC hdc, IColumnsInfo* pColumnsInfo)
{
	RETURN_IF_FAILED(m_pLayoutManager->PaintLayout(hdc, &(CPoint()), m_pImageManagerService, pColumnsInfo));
	return S_OK;
}

STDMETHODIMP CSkinTabControl::SetErrorInfo(HWND hWnd, BOOL bError, BSTR bstrMessage)
{
	m_hWnd = hWnd;
	m_bError = bError;
	m_bstrMessage = bstrMessage;
	return S_OK;
}

STDMETHODIMP CSkinTabControl::AnimationGetParams(UINT* puiMilliseconds)
{
	CHECK_E_POINTER(puiMilliseconds);
	*puiMilliseconds = 200;
	return S_OK;
}

STDMETHODIMP CSkinTabControl::AnimationStart()
{
	m_bAnimation = TRUE;
	return S_OK;
}

STDMETHODIMP CSkinTabControl::AnimationStop()
{
	m_bAnimation = FALSE;
	return S_OK;
}

STDMETHODIMP CSkinTabControl::AnimationNextFrame()
{
	CComPtr<IColumnsInfoItem> pColumnsItem;
	RETURN_IF_FAILED(m_pColumnsInfo->FindItemByName(Twitter::Themes::Metadata::TabContainer::MarqueeProgressBox, &pColumnsItem));
	CComBSTR bstrValue;
	RETURN_IF_FAILED(pColumnsItem->GetRectStringProp(Twitter::Themes::Metadata::MarqueeProgressColumn::Value, &bstrValue));
	CComBSTR bstrItemCount;
	RETURN_IF_FAILED(pColumnsItem->GetRectStringProp(Twitter::Themes::Metadata::MarqueeProgressColumn::ItemCount, &bstrItemCount));

	auto value = _wtoi(bstrValue);
	auto itemCount = _wtoi(bstrItemCount);
	value++;
	if (value == itemCount - 1)
		value = 0;
	RETURN_IF_FAILED(pColumnsItem->SetRectStringProp(Twitter::Themes::Metadata::MarqueeProgressColumn::Value, CComBSTR(to_wstring(value).c_str())));
	return S_OK;
}

STDMETHODIMP CSkinTabControl::GetInfoRect(RECT* pRect)
{
	CHECK_E_POINTER(pRect);
	*pRect = m_rectInfoImage;
	return S_OK;
}
