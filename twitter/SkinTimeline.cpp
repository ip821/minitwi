// SkinTimeline.cpp : Implementation of CSkinTimeline

#include "stdafx.h"
#include "SkinTimeline.h"
#include "Plugins.h"
#include <boost\date_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/date_time/date_facet.hpp>
#include <boost/date_time/time_facet.hpp>
#include <clocale>
#include <iosfwd>
#include <ctime>

// CSkinTimeline

#define COL_NAME_LEFT 64
#define COLUMN_X_SPACING 5
#define COLUMN_Y_SPACING 0
#define PADDING_Y 7
#define ITEM_SPACING 10
#define ITEM_DELIMITER_HEIGHT 1

CSkinTimeline::CSkinTimeline()
{
}

STDMETHODIMP CSkinTimeline::SetImageManagerService(IImageManagerService* pImageManagerService)
{
	m_pImageManagerService = pImageManagerService;
	return S_OK;
}

STDMETHODIMP CSkinTimeline::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	CHECK_E_POINTER(pThemeColorMap);
	m_pThemeColorMap = pThemeColorMap;
	return S_OK;
}


STDMETHODIMP CSkinTimeline::SetFontMap(IThemeFontMap* pThemeFontMap)
{
	CHECK_E_POINTER(pThemeFontMap);
	m_pThemeFontMap = pThemeFontMap;
	return S_OK;
}

STDMETHODIMP CSkinTimeline::DrawItem(HWND hwndControl, IColumnRects* pColumnRects, TDRAWITEMSTRUCT* lpdi, int iHoveredItem, int iHoveredColumn)
{
	CListBox wndListBox(hwndControl);
	auto selectedItemId = wndListBox.GetCurSel();

	RECT clientRect = { 0 };
	wndListBox.GetClientRect(&clientRect);

	CDCHandle cdc = lpdi->hDC;
	cdc.SetBkMode(TRANSPARENT);

	if (lpdi->itemState & ODS_SELECTED)
	{
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(VAR_BRUSH_SELECTED, &dwColor));
		CBrush brush;
		brush.CreateSolidBrush(dwColor);
		cdc.FillRect(&(lpdi->rcItem), brush);
	}
	else
	{
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(VAR_BRUSH_BACKGROUND, &dwColor));
		CBrush brush;
		brush.CreateSolidBrush(dwColor);
		RECT rect = lpdi->rcItem;
		rect.bottom -= COLUMN_Y_SPACING;
		cdc.FillRect(&rect, brush);
	}

	{
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(VAR_TWITTER_DELIMITER, &dwColor));
		CBrush brush;
		brush.CreateSolidBrush(dwColor);
		RECT rect = lpdi->rcItem;
		rect.bottom = rect.top + ITEM_DELIMITER_HEIGHT;
		cdc.FillRect(&rect, brush);
	}

	UINT uiCount = 0;
	pColumnRects->GetCount(&uiCount);
	for (size_t i = 0; i < uiCount; i++)
	{
		RECT rect = { 0 };
		pColumnRects->GetRect(i, &rect);
		CComBSTR bstrColumnName;
		pColumnRects->GetRectStringProp(i, VAR_COLUMN_NAME, &bstrColumnName);
		CComBSTR bstrText;
		pColumnRects->GetRectStringProp(i, VAR_TEXT, &bstrText);
		BOOL bIsUrl = FALSE;
		pColumnRects->GetRectBoolProp(i, VAR_IS_URL, &bIsUrl);
		BOOL bIsWordWrap = FALSE;
		pColumnRects->GetRectBoolProp(i, VAR_IS_WORDWRAP, &bIsWordWrap);
		BOOL bIsImage = FALSE;
		pColumnRects->GetRectBoolProp(i, VAR_IS_IMAGE, &bIsImage);

		HFONT font = 0;
		m_pThemeFontMap->GetFont(bstrColumnName, &font);

		if (iHoveredItem == lpdi->itemID && iHoveredColumn == i && bIsUrl)
		{
			m_pThemeFontMap->GetFont(bstrColumnName + VAR_SELECTED_POSTFIX, &font);
		}

		if (bIsImage)
		{
			CComBSTR bstrValue;
			pColumnRects->GetRectStringProp(i, VAR_VALUE, &bstrValue);

			CBitmap bitmap;
			m_pImageManagerService->CreateImageBitmap(bstrValue, &bitmap.m_hBitmap);
			TBITMAP tBitmap = { 0 };
			m_pImageManagerService->GetImageInfo(bstrValue, &tBitmap);
			if (bitmap.m_hBitmap)
			{
				CDC cdcBitmap;
				cdcBitmap.CreateCompatibleDC(cdc);
				cdcBitmap.SelectBitmap(bitmap);
				auto x = lpdi->rcItem.left;
				auto y = lpdi->rcItem.top;
				auto width = min(rect.right - rect.left, (int)tBitmap.Width);
				auto height = min(rect.bottom - rect.top, (int)tBitmap.Height);
				static Gdiplus::Color color(Gdiplus::Color::White);
				TransparentBlt(cdc, x + rect.left, y + rect.top, width, height, cdcBitmap, 0, 0, width, height, color.ToCOLORREF());
			}
		}
		else
		{
			DWORD dwColor = 0;
			RETURN_IF_FAILED(m_pThemeColorMap->GetColor(bstrColumnName, &dwColor));
			cdc.SetTextColor(dwColor);

			auto x = lpdi->rcItem.left;
			auto y = lpdi->rcItem.top;

			CString str(bstrText);
			cdc.SelectFont(font);
			RECT rectText = { x + rect.left, y + rect.top, x + rect.right, y + rect.bottom };
			cdc.DrawText(str, str.GetLength(), &rectText, bIsWordWrap ? DT_WORDBREAK : 0);
		}
	}

	return S_OK;
}

void CSkinTimeline::PrepareDC(HDC hdc, SIZE size, CString strColumnName, CDC& cdc)
{
	cdc.CreateCompatibleDC(hdc);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(cdc, size.cx, size.cy);
	cdc.SelectBitmap(bitmap);

	HFONT font = 0;
	m_pThemeFontMap->GetFont(CComBSTR(strColumnName), &font);
	cdc.SelectFont(font);
}

SIZE CSkinTimeline::AddColumn(
	HDC hdc,
	IColumnRects* pColumnRects,
	CString& strColumnName,
	CString& strDisplayText,
	CString& strValue,
	int x,
	int y,
	SIZE size,
	BOOL bIsUrl = TRUE,
	BOOL bWordWrap = FALSE,
	LONG ulMinimumFixedWidth = 0,
	Justify justify = Justify::None
	)
{
	CDC cdc;
	PrepareDC(hdc, size, strColumnName, cdc);

	SIZE sz = { 0 };
	if (bWordWrap)
	{
		RECT rect = { 0 };
		rect.right = size.cx;
		rect.bottom = size.cy;

		sz.cx = size.cx;
		sz.cy = DrawText(cdc, strDisplayText, strDisplayText.GetLength(), &rect, DT_WORDBREAK);
	}
	else
	{
		GetTextExtentPoint32(cdc, strDisplayText, strDisplayText.GetLength(), &sz);
		if (ulMinimumFixedWidth)
			sz.cx = max(ulMinimumFixedWidth, sz.cx);
	}

	if (justify == Justify::Center)
	{
		x = (size.cx / 2) - (sz.cx / 2);
	}
	else if (justify == Justify::Right)
	{
		x = x - sz.cx;
	}

	UINT uiIndex = 0;
	pColumnRects->AddRect(CRect(x, y, x + sz.cx, y + sz.cy), &uiIndex);
	pColumnRects->SetRectStringProp(uiIndex, VAR_COLUMN_NAME, CComBSTR(strColumnName));
	pColumnRects->SetRectStringProp(uiIndex, VAR_TEXT, CComBSTR(strDisplayText));
	pColumnRects->SetRectStringProp(uiIndex, VAR_VALUE, CComBSTR(strValue));
	if (bWordWrap)
		pColumnRects->SetRectBoolProp(uiIndex, VAR_IS_WORDWRAP, TRUE);
	if (bIsUrl)
		pColumnRects->SetRectBoolProp(uiIndex, VAR_IS_URL, TRUE);
	return sz;
}

void CSkinTimeline::GetValue(IVariantObject* pItemObject, CComBSTR& bstrColumnName, CString& strValue)
{
	CComVariant v;
	pItemObject->GetVariantValue(bstrColumnName, &v);
	if (v.vt == VT_BSTR)
		strValue = v.bstrVal;
}

STDMETHODIMP CSkinTimeline::MeasureItem(HWND hwndControl, IVariantObject* pItemObject, TMEASUREITEMSTRUCT* lpMeasureItemStruct, IColumnRects* pColumnRects)
{
	pColumnRects->Clear();
	CListBox wndListBox(hwndControl);

	CDC hdc(GetDC(hwndControl));

	RECT clientRect = { 0 };
	wndListBox.GetClientRect(&clientRect);

	CString strObjectType;
	GetValue(pItemObject, CComBSTR(VAR_OBJECT_TYPE), strObjectType);

	if (strObjectType == TYPE_SHOWMORE_OBJECT)
	{
		CComVariant vDisabled;
		pItemObject->GetVariantValue(VAR_ITEM_DISABLED, &vDisabled);
		auto bDisabled = vDisabled.vt == VT_BOOL && vDisabled.boolVal;
		auto y = PADDING_Y;
		CSize sz = AddColumn(
			hdc,
			pColumnRects,
			CString(VAR_COLUMN_SHOW_MORE),
			bDisabled ? CString(L"Updating...") : CString(L"Show more"),
			bDisabled ? CString(L"Updating...") : CString(L"Show more"),
			0,
			y,
			CSize(clientRect.right - clientRect.left),
			!bDisabled,
			FALSE,
			0,
			Justify::Center
			);

		lpMeasureItemStruct->itemHeight = y + sz.cy + PADDING_Y;
		lpMeasureItemStruct->itemWidth = sz.cx;
	}
	else if (strObjectType == TYPE_TWITTER_OBJECT)
	{
		CString strRetweetedDisplayName;
		GetValue(pItemObject, CComBSTR(VAR_TWITTER_RETWEETED_USER_DISPLAY_NAME), strRetweetedDisplayName);

		CString strRetweetedName;
		GetValue(pItemObject, CComBSTR(VAR_TWITTER_RETWEETED_USER_NAME), strRetweetedName);

		CString strDisplayName;
		GetValue(pItemObject, CComBSTR(VAR_TWITTER_USER_DISPLAY_NAME), strDisplayName);

		CString strCreatedAt;
		GetValue(pItemObject, CComBSTR(VAR_TWITTER_RELATIVE_TIME), strCreatedAt);

		CString strName;
		GetValue(pItemObject, CComBSTR(VAR_TWITTER_USER_NAME), strName);

		CString strText;
		GetValue(pItemObject, CComBSTR(VAR_TWITTER_NORMALIZED_TEXT), strText);

		CString strImageUrl;
		GetValue(pItemObject, CComBSTR(VAR_TWITTER_USER_IMAGE), strImageUrl);

		{
			auto x = 10;
			auto y = PADDING_Y * 2;
			UINT uiIndex = 0;
			pColumnRects->AddRect(CRect(x, y, x + 48, y + 48), &uiIndex);
			pColumnRects->SetRectStringProp(uiIndex, VAR_COLUMN_NAME, CComBSTR(VAR_TWITTER_USER_IMAGE));
			pColumnRects->SetRectStringProp(uiIndex, VAR_TEXT, L"");
			pColumnRects->SetRectStringProp(uiIndex, VAR_VALUE, CComBSTR(strImageUrl));
			pColumnRects->SetRectBoolProp(uiIndex, VAR_IS_IMAGE, TRUE);
		}

		CSize sizeRetweetedDislpayName;
		UINT uiIndex = 0;
		if (!strRetweetedDisplayName.IsEmpty())
		{
			auto x = COL_NAME_LEFT;
			auto y = PADDING_Y;

			sizeRetweetedDislpayName = AddColumn(
				hdc,
				pColumnRects,
				CString(VAR_TWITTER_RETWEETED_USER_DISPLAY_NAME),
				L"Retweeted by " + strRetweetedDisplayName + L" @" + strRetweetedName,
				strRetweetedDisplayName,
				x,
				y,
				CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, 255),
				FALSE,
				FALSE
				);
		}

		CSize sizeDislpayName;
		uiIndex = 0;
		{
			auto x = COL_NAME_LEFT;
			auto y = COLUMN_Y_SPACING + PADDING_Y;

			if (sizeRetweetedDislpayName.cy)
			{
				y += sizeRetweetedDislpayName.cy + COLUMN_Y_SPACING;
			}

			sizeDislpayName = AddColumn(
				hdc,
				pColumnRects,
				CString(VAR_TWITTER_USER_DISPLAY_NAME),
				strDisplayName,
				strDisplayName,
				x,
				y,
				CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, 255)
				);
		}

		CSize sizeName;
		{
			auto x = COL_NAME_LEFT + sizeDislpayName.cx + COLUMN_X_SPACING;
			auto y = COLUMN_Y_SPACING + PADDING_Y;

			if (sizeRetweetedDislpayName.cy)
			{
				y += sizeRetweetedDislpayName.cy + COLUMN_Y_SPACING;
			}

			sizeName = AddColumn(
				hdc,
				pColumnRects,
				CString(VAR_TWITTER_USER_NAME),
				L"@" + strName,
				strName,
				x,
				y,
				CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, 255)
				);
		}

		CSize sizeDateTime;
		{
			auto x = clientRect.right - clientRect.left;
			auto y = COLUMN_Y_SPACING + PADDING_Y;

			if (sizeRetweetedDislpayName.cy)
			{
				y += sizeRetweetedDislpayName.cy + COLUMN_Y_SPACING;
			}

			if (strCreatedAt.IsEmpty())
				strCreatedAt = L"0s";

			sizeDateTime = AddColumn(
				hdc,
				pColumnRects,
				CString(VAR_TWITTER_RELATIVE_TIME),
				strCreatedAt,
				strCreatedAt,
				x,
				y,
				CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, 255),
				TRUE,
				FALSE,
				30,
				Justify::Right
				);
		}

		CSize sizeText;
		if (!strText.IsEmpty())
		{
			auto x = COL_NAME_LEFT;
			auto y = sizeDislpayName.cy + COLUMN_Y_SPACING + COLUMN_Y_SPACING + PADDING_Y;

			if (sizeRetweetedDislpayName.cy)
			{
				y += sizeRetweetedDislpayName.cy + COLUMN_Y_SPACING;
			}

			sizeText = AddColumn(
				hdc,
				pColumnRects,
				CString(VAR_TWITTER_NORMALIZED_TEXT),
				strText,
				strText,
				x,
				y,
				CSize((clientRect.right - clientRect.left - COLUMN_X_SPACING * 2) - COL_NAME_LEFT, 255),
				FALSE,
				TRUE
				);
		}

		auto lastY = sizeDislpayName.cy + COLUMN_Y_SPACING + sizeText.cy + PADDING_Y;

		if (sizeRetweetedDislpayName.cy)
		{
			lastY += sizeRetweetedDislpayName.cy + COLUMN_Y_SPACING;
		}

		std::hash_set<std::wstring> imageUrls;
		{ //Images
			CComVariant vMediaUrls;
			pItemObject->GetVariantValue(VAR_TWITTER_MEDIAURLS, &vMediaUrls);
			if (vMediaUrls.vt == VT_UNKNOWN)
			{
				CComQIPtr<IObjArray> pObjArray = vMediaUrls.punkVal;
				UINT_PTR uiCount = 0;
				pObjArray->GetCount(&uiCount);

				if (uiCount)
				{
					const int IMAGE_HEIGHT = 100;
					const int IMAGE_WIDTH = 275;
					auto xOffset = ((clientRect.right - clientRect.left) - (IMAGE_WIDTH * uiCount)) / 2;

					int i = 0;
					{ //Use only first image due to size
						CComPtr<IVariantObject> pMediaObject;
						pObjArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pMediaObject);

						CComVariant vMediaUrl;
						pMediaObject->GetVariantValue(VAR_TWITTER_MEDIAURL, &vMediaUrl);

						CComVariant vMediaUrlThumb;
						pMediaObject->GetVariantValue(VAR_TWITTER_MEDIAURL_THUMB, &vMediaUrlThumb);

						CComVariant vMediaUrlShort;
						pMediaObject->GetVariantValue(VAR_TWITTER_MEDIAURL_SHORT, &vMediaUrlShort);

						imageUrls.insert(vMediaUrlShort.bstrVal);

						TBITMAP tBitmap = { 0 };
						m_pImageManagerService->GetImageInfo(vMediaUrlThumb.bstrVal, &tBitmap);

						auto x = i * IMAGE_WIDTH + xOffset;
						auto y = lastY;
						auto width = IMAGE_WIDTH;
						auto height = IMAGE_HEIGHT;

						UINT uiIndex = 0;
						pColumnRects->AddRect(CRect(x, y, x + width, y + height), &uiIndex);
						pColumnRects->SetRectStringProp(uiIndex, VAR_COLUMN_NAME, CComBSTR(VAR_TWITTER_IMAGE));
						pColumnRects->SetRectStringProp(uiIndex, VAR_TEXT, L"");
						pColumnRects->SetRectStringProp(uiIndex, VAR_VALUE, vMediaUrlThumb.bstrVal);
						pColumnRects->SetRectStringProp(uiIndex, VAR_TWITTER_MEDIAURL, vMediaUrl.bstrVal);
						pColumnRects->SetRectBoolProp(uiIndex, VAR_IS_IMAGE, TRUE);
						pColumnRects->SetRectBoolProp(uiIndex, VAR_IS_URL, TRUE);
					}

					lastY += IMAGE_HEIGHT;
				}
			}
		}

		CComVariant vUrls;
		pItemObject->GetVariantValue(VAR_TWITTER_URLS, &vUrls);
		if (vUrls.vt == VT_UNKNOWN)
		{
			auto x = COL_NAME_LEFT;

			lastY += COLUMN_Y_SPACING;

			CComQIPtr<IBstrCollection> pBstrCollection = vUrls.punkVal;
			UINT_PTR uiCount = 0;
			pBstrCollection->GetCount(&uiCount);
			for (size_t i = 0; i < uiCount; i++)
			{
				CComBSTR bstrUrl;
				pBstrCollection->GetItem(i, &bstrUrl);
				if (imageUrls.find(bstrUrl.m_str) != imageUrls.end())
					continue;

				auto size = AddColumn(
					hdc,
					pColumnRects,
					CString(VAR_TWITTER_URL),
					CString(bstrUrl),
					CString(bstrUrl),
					x,
					lastY,
					CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, 255)
					);

				lastY += size.cy;
			}
		}

		lpMeasureItemStruct->itemHeight = min(255, max(48 + PADDING_Y * 2 + PADDING_Y * 2, lastY + ITEM_SPACING));
		lpMeasureItemStruct->itemWidth = sizeText.cx;
	}
	return S_OK;
}
