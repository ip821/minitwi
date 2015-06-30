// SkinTabControl.cpp : Implementation of CSkinTabControl

#include "stdafx.h"
#include "SkinTabControl.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"
#include "GdilPlusUtils.h"
#include "LayoutFunctions.h"
#include "Metadata.h"
#include "..\model-libs\layout\Functions.h"

// CSkinTabControl

const int TOOLTIP_ID = 1;

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
	RETURN_IF_FAILED(m_pTheme->GetImageManagerService(&m_pImageManagerService));

	return S_OK;
}

STDMETHODIMP CSkinTabControl::SetColumnsInfo(IColumnsInfo* pColumnsInfo)
{
	m_pColumnsInfo = pColumnsInfo;
	return S_OK;
}

STDMETHODIMP CSkinTabControl::SetSelectedIndex(UINT uiIndex)
{
	CComVar vContainers;
	RETURN_IF_FAILED(m_pLayoutObject->GetVariantValue(Layout::Metadata::Element::Elements, &vContainers));
	ATLASSERT(vContainers.vt == VT_UNKNOWN);
	CComQIPtr<IObjArray> pContainers = vContainers.punkVal;
	UINT uiCount = 0;
	RETURN_IF_FAILED(pContainers->GetCount(&uiCount));

	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pContainer;
		ASSERT_IF_FAILED(pContainers->GetAt(i, __uuidof(IVariantObject),(LPVOID*)&pContainer));
		ASSERT_IF_FAILED(HrLayoutSetVariantValueRecursive(pContainer, Layout::Metadata::Element::Selected, &CComVar((BOOL)(uiIndex == static_cast<int>(i)))));
	}
	return S_OK;
}

STDMETHODIMP CSkinTabControl::MeasureHeader(HDC hdc, IObjArray* pObjArray, RECT* clientRect, UINT* puiHeight)
{
	{
		CComPtr<IVariantObject> pElement;
		RETURN_IF_FAILED(HrLayoutFindItemByName(m_pLayoutObject, Twitter::Themes::Metadata::TabContainer::MarqueeProgressBox, &pElement));
		RETURN_IF_FAILED(pElement->SetVariantValue(Layout::Metadata::Element::Visible, &CComVar(m_bAnimation == TRUE)));
	}

	{
		CComPtr<IVariantObject> pElement;
		RETURN_IF_FAILED(HrLayoutFindItemByName(m_pLayoutObject, Twitter::Themes::Metadata::TabContainer::InfoImage, &pElement));

		if (m_bstrMessage == L"")
		{
			RETURN_IF_FAILED(pElement->SetVariantValue(Layout::Metadata::Element::Visible, &CComVar(false)));
		}
		else
		{
			RETURN_IF_FAILED(pElement->SetVariantValue(Layout::Metadata::Element::Visible, &CComVar(true)));

			if (m_bError)
			{
				RETURN_IF_FAILED(pElement->SetVariantValue(Layout::Metadata::ImageColumn::ImageKey, &CComVar(Twitter::Themes::Metadata::TabContainer::Images::Error)));
			}
			else
			{
				RETURN_IF_FAILED(pElement->SetVariantValue(Layout::Metadata::ImageColumn::ImageKey, &CComVar(Twitter::Themes::Metadata::TabContainer::Images::Info)));
			}
		}
	}

	RETURN_IF_FAILED(m_pLayoutManager->BuildLayout(hdc, clientRect, m_pLayoutObject, nullptr, m_pImageManagerService, m_pColumnsInfo));
	{
		UINT uiIndex = 0;
		RETURN_IF_FAILED(m_pColumnsInfo->FindItemIndex(Twitter::Themes::Metadata::TabContainer::LayoutName, &uiIndex));
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(m_pColumnsInfo->GetItem(uiIndex, &pColumnsInfoItem));
		RETURN_IF_FAILED(pColumnsInfoItem->GetRect(&m_rectHeader));
		*puiHeight = m_rectHeader.Height();
	}

	{
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(m_pColumnsInfo->FindItemByName(Twitter::Themes::Metadata::TabContainer::InfoImage, &pColumnsInfoItem));
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

		TOOLINFO ti = { 0 };
		ti.cbSize = sizeof(ti);
		ti.hwnd = m_hWnd;
		ti.uFlags = TTF_SUBCLASS | TTF_CENTERTIP;
		ti.rect = m_rectInfoImage;
		ti.uId = TOOLTIP_ID;
		ti.hinst = NULL;
		m_wndTooltip.Activate(TRUE);
		m_wndTooltip.AddTool(&ti);
		m_wndTooltip.UpdateTipText(m_bstrMessage.m_str, m_hWnd, TOOLTIP_ID);
	}
	return S_OK;
}

STDMETHODIMP CSkinTabControl::EraseBackground(HDC hdc)
{
	if (m_rectHeader.IsRectEmpty())
		return S_OK;

	RETURN_IF_FAILED(m_pLayoutManager->EraseBackground(hdc, m_pColumnsInfo));
	return S_OK;
}

STDMETHODIMP CSkinTabControl::DrawHeader(HDC hdc)
{
	RETURN_IF_FAILED(m_pLayoutManager->PaintLayout(hdc, m_pImageManagerService, m_pColumnsInfo));
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
	CComVar vValue;
	RETURN_IF_FAILED(pColumnsItem->GetVariantValue(Layout::Metadata::MarqueeProgressColumn::Value, &vValue));
	CComVar vItemCount;
	RETURN_IF_FAILED(pColumnsItem->GetVariantValue(Layout::Metadata::MarqueeProgressColumn::ItemCount, &vItemCount));

	ATLASSERT(vValue.vt == VT_I4);
	ATLASSERT(vItemCount.vt == VT_I4);

	auto value = vValue.intVal;
	auto itemCount = vItemCount.intVal;
	value++;
	if (value == itemCount - 1)
		value = 0;
	RETURN_IF_FAILED(pColumnsItem->SetVariantValue(Layout::Metadata::MarqueeProgressColumn::Value, &CComVar(value)));
	return S_OK;
}

STDMETHODIMP CSkinTabControl::GetInfoRect(RECT* pRect)
{
	CHECK_E_POINTER(pRect);
	*pRect = m_rectInfoImage;
	return S_OK;
}
