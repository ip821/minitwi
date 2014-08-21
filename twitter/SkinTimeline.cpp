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

#define COL_NAME_LEFT 16
#define COLUMN_X_SPACING 5
#define COLUMN_Y_SPACING 0
#define PADDING_Y 7
#define ITEM_SPACING 10
#define ITEM_DELIMITER_HEIGHT 1

CSkinTimeline::CSkinTimeline()
{
	m_tz = { 0 };
	GetTimeZoneInformation(&m_tz);
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
		pColumnRects->GetRectProp(i, VAR_COLUMN_NAME, &bstrColumnName);
		CComBSTR bstrText;
		pColumnRects->GetRectProp(i, VAR_TEXT, &bstrText);
		CComBSTR bstrIsUrl;
		pColumnRects->GetRectProp(i, VAR_IS_URL, &bstrIsUrl);
		CComBSTR bstrIsWordWrap;
		pColumnRects->GetRectProp(i, VAR_IS_WORDWRAP, &bstrIsWordWrap);

		HFONT font = 0;
		m_pThemeFontMap->GetFont(bstrColumnName, &font);

		if (iHoveredItem == lpdi->itemID && iHoveredColumn == i && bstrIsUrl == L"1")
		{
			m_pThemeFontMap->GetFont(bstrColumnName + VAR_SELECTED_POSTFIX, &font);
		}

		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(bstrColumnName, &dwColor));
		cdc.SetTextColor(dwColor);

		auto x = lpdi->rcItem.left;
		auto y = lpdi->rcItem.top;

		CString str(bstrText);
		cdc.SelectFont(font);
		RECT rectText = { x + rect.left, y + rect.top, x + rect.right, y + rect.bottom };
		cdc.DrawText(str, str.GetLength(), &rectText, bstrIsWordWrap == L"1" ? DT_WORDBREAK : 0);
	}

	return S_OK;
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
	BOOL bWordWrap = FALSE
	)
{
	CDC cdc;
	cdc.CreateCompatibleDC(hdc);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(cdc, size.cx, size.cy);
	cdc.SelectBitmap(bitmap);

	RECT rect = { 0 };
	rect.right = size.cx;
	rect.bottom = size.cy;
	
	HFONT font = 0;
	m_pThemeFontMap->GetFont(CComBSTR(strColumnName), &font);
	cdc.SelectFont(font);

	SIZE sz = { 0 };
	if (bWordWrap)
	{
		sz.cx = size.cx;
		sz.cy = DrawText(cdc, strDisplayText, strDisplayText.GetLength(), &rect, DT_WORDBREAK);
	}
	else
	{
		GetTextExtentPoint32(cdc, strDisplayText, strDisplayText.GetLength(), &sz);
	}

	UINT uiIndex = 0;
	pColumnRects->AddRect(CRect(x, y, x + sz.cx, y + sz.cy), &uiIndex);
	pColumnRects->SetRectProp(uiIndex, VAR_COLUMN_NAME, CComBSTR(strColumnName));
	pColumnRects->SetRectProp(uiIndex, VAR_TEXT, CComBSTR(strDisplayText));
	pColumnRects->SetRectProp(uiIndex, VAR_VALUE, CComBSTR(strValue));
	if (bWordWrap)
		pColumnRects->SetRectProp(uiIndex, VAR_IS_WORDWRAP, CComBSTR(L"1"));
	if (bIsUrl)
		pColumnRects->SetRectProp(uiIndex, VAR_IS_URL, CComBSTR(L"1"));
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

	CString strRetweetedDisplayName;
	GetValue(pItemObject, CComBSTR(VAR_TWITTER_RETWEETED_USER_DISPLAY_NAME), strRetweetedDisplayName);

	CString strRetweetedName;
	GetValue(pItemObject, CComBSTR(VAR_TWITTER_RETWEETED_USER_NAME), strRetweetedName);

	CString strDisplayName;
	GetValue(pItemObject, CComBSTR(VAR_TWITTER_USER_DISPLAY_NAME), strDisplayName);

	CString strCreatedAt;
	GetValue(pItemObject, CComBSTR(VAR_TWITTER_CREATED_AT), strCreatedAt);

	{
		boost::local_time::wlocal_time_input_facet* inputFacet = new boost::local_time::wlocal_time_input_facet();
		inputFacet->format(L"%a %b %d %H:%M:%S +0000 %Y");
		std::wistringstream inputStream;
		inputStream.imbue(std::locale(inputStream.getloc(), inputFacet));
		inputStream.str(std::wstring(strCreatedAt));
		boost::posix_time::ptime pt;
		inputStream >> pt;

		boost::posix_time::ptime ptNow(pt.date(), pt.time_of_day() - boost::posix_time::minutes(m_tz.Bias) - boost::posix_time::minutes(m_tz.DaylightBias));
		boost::local_time::wlocal_time_facet* outputFacet = new boost::local_time::wlocal_time_facet();
		std::wostringstream outputStream;
		outputStream.imbue(std::locale(std::locale(""), outputFacet));
		outputFacet->format(L"%b %d %H:%M:%S %Y");
		outputStream << ptNow;
		strCreatedAt = outputStream.str().c_str();
	}

	CString strName;
	GetValue(pItemObject, CComBSTR(VAR_TWITTER_USER_NAME), strName);

	CString strText;
	GetValue(pItemObject, CComBSTR(VAR_TWITTER_NORMALIZED_TEXT), strText);

	RECT clientRect = { 0 };
	wndListBox.GetClientRect(&clientRect);

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
		auto x = COL_NAME_LEFT + sizeDislpayName.cx + COLUMN_X_SPACING + sizeName.cx + COLUMN_X_SPACING;
		auto y = COLUMN_Y_SPACING + PADDING_Y;

		if (sizeRetweetedDislpayName.cy)
		{
			y += sizeRetweetedDislpayName.cy + COLUMN_Y_SPACING;
		}

		sizeDateTime = AddColumn(
			hdc,
			pColumnRects,
			CString(VAR_TWITTER_CREATED_AT),
			strCreatedAt,
			strCreatedAt,
			x,
			y,
			CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, 255)
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
			CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, 255),
			FALSE,
			TRUE
			);
	}

	auto lastY = sizeDislpayName.cy + COLUMN_Y_SPACING + sizeText.cy + PADDING_Y;

	if (sizeRetweetedDislpayName.cy)
	{
		lastY += sizeRetweetedDislpayName.cy + COLUMN_Y_SPACING;
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

	lpMeasureItemStruct->itemHeight = min(255, lastY + ITEM_SPACING);
	lpMeasureItemStruct->itemWidth = sizeText.cx;
	return S_OK;
}
