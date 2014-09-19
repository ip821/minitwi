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

STDMETHODIMP CSkinTimeline::GetImageManagerService(IImageManagerService** ppImageManagerService)
{
	CHECK_E_POINTER(ppImageManagerService);
	RETURN_IF_FAILED(m_pImageManagerService->QueryInterface(ppImageManagerService));
	return S_OK;
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

STDMETHODIMP CSkinTimeline::DrawItem(
	HWND hwndControl, 
	IColumnRects* pColumnRects, 
	TDRAWITEMSTRUCT* lpdi, 
	int iHoveredItem, 
	int iHoveredColumn,
	UINT* puiNotAnimatedColumnIndexes,
	UINT uiNotAnimatedColumnIndexesCount)
{
	CRect rect = lpdi->rcItem;
	CDCHandle cdcReal = lpdi->hDC;

	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(cdcReal, rect.Width(), rect.Height());

	CDC cdc;
	cdc.CreateCompatibleDC(cdcReal);
	cdc.SelectBitmap(bitmap);

	lpdi->hDC = cdc;
	lpdi->rcItem.left = 0;
	lpdi->rcItem.top = 0;
	lpdi->rcItem.right = rect.Width();
	lpdi->rcItem.bottom = rect.Height();
	RETURN_IF_FAILED(DrawItemInternal(hwndControl, pColumnRects, lpdi, iHoveredItem, iHoveredColumn, puiNotAnimatedColumnIndexes, uiNotAnimatedColumnIndexesCount));
	lpdi->rcItem = rect;
	lpdi->hDC = cdcReal;

	BLENDFUNCTION bf = { 0 };
	bf.BlendOp = AC_SRC_OVER;
	bf.SourceConstantAlpha = MAX_ALPHA;
	if (m_steps.find(lpdi->itemID) != m_steps.end())
	{
		bf.SourceConstantAlpha = m_steps[lpdi->itemID].alpha;
	}
	cdcReal.AlphaBlend(rect.left, rect.top, rect.Width(), rect.Height(), cdc, 0, 0, rect.Width(), rect.Height(), bf);
	return S_OK;
}

STDMETHODIMP CSkinTimeline::DrawItemInternal(HWND hwndControl, IColumnRects* pColumnRects, TDRAWITEMSTRUCT* lpdi, int iHoveredItem, int iHoveredColumn, UINT* puiNotAnimatedColumnIndexes, UINT uiNotAnimatedColumnIndexesCount)
{
	std::hash_set<UINT> columnIndexesAlreadyAnimated;

	if (puiNotAnimatedColumnIndexes)
	{
		for (size_t i = 0; i < uiNotAnimatedColumnIndexesCount; i++)
		{
			columnIndexesAlreadyAnimated.insert(puiNotAnimatedColumnIndexes[i]);
		}
	}

	CListBox wndListBox(hwndControl);
	auto selectedItemId = wndListBox.GetCurSel();

	RECT clientRect = { 0 };
	wndListBox.GetClientRect(&clientRect);

	CDCHandle cdc = lpdi->hDC;
	cdc.SetBkMode(TRANSPARENT);
	
	BOOL bDisabledSelection = FALSE;
	pColumnRects->IsDisabledSelection(&bDisabledSelection);

	if (lpdi->itemState & ODS_SELECTED && !bDisabledSelection)
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

	if (!bDisabledSelection)
	{
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(VAR_TWITTER_DELIMITER, &dwColor));
		CBrush brush;
		brush.CreateSolidBrush(dwColor);
		RECT rect = lpdi->rcItem;
		rect.top = rect.bottom - ITEM_DELIMITER_HEIGHT;
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
			if (bitmap.m_hBitmap)
			{
				TBITMAP tBitmap = { 0 };
				m_pImageManagerService->GetImageInfo(bstrValue, &tBitmap);

				CDC cdcBitmap;
				cdcBitmap.CreateCompatibleDC(cdc);
				cdcBitmap.SelectBitmap(bitmap);

				auto x = lpdi->rcItem.left;
				auto y = lpdi->rcItem.top;
				auto width = min(rect.right - rect.left, (int)tBitmap.Width);
				auto height = min(rect.bottom - rect.top, (int)tBitmap.Height);

				BLENDFUNCTION bf = { 0 };
				bf.BlendOp = AC_SRC_OVER;
				bf.SourceConstantAlpha = 0;

				auto it = m_steps.find(lpdi->itemID);
				if (it == m_steps.end())
				{
					if (columnIndexesAlreadyAnimated.find(i) != columnIndexesAlreadyAnimated.end())
					{
						bf.SourceConstantAlpha = MAX_ALPHA;
					}
				}
				else
				{
					if (it->second.columns.find(i) != it->second.columns.end())
						bf.SourceConstantAlpha = it->second.columns[i].alpha;
					else if (columnIndexesAlreadyAnimated.find(i) != columnIndexesAlreadyAnimated.end())
					{
						bf.SourceConstantAlpha = MAX_ALPHA;
					}
				}
				cdc.AlphaBlend(x + rect.left, y + rect.top, width, height, cdcBitmap, 0, 0, width, height, bf);
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
	Justify justify = Justify::None,
	BOOL bDisabledSelection = FALSE
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
	if (bDisabledSelection)
		pColumnRects->DisableSelection(TRUE);
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
			Justify::Center,
			TRUE
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

					const size_t processCount = min(uiCount, 3); //process no more than 3 images
					const int oneImageWidth = (IMAGE_WIDTH / processCount);
					for (size_t i = 0; i < processCount; i++)
					{
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

						auto x = i * oneImageWidth + xOffset;
						auto y = lastY;
						auto width = oneImageWidth;
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

STDMETHODIMP CSkinTimeline::AnimationRegisterItemIndex(UINT uiIndex, IColumnRects* pColumnRects, int iColumnIndex)
{
	map<UINT, AnimationItemData>::iterator it = m_steps.find(uiIndex);
	if (it == m_steps.end())
	{
		AnimationItemData s;
		s.step = STEPS + 1;
		s.alpha = MAX_ALPHA;
		m_steps[uiIndex] = s;
		it = m_steps.find(uiIndex);
	}

	if (iColumnIndex == -1)
	{
		it->second.step = 0;
		it->second.alpha = 0;
	}
	else
	{
		CComBSTR bstrValue;
		RETURN_IF_FAILED(pColumnRects->GetRectStringProp(iColumnIndex, VAR_VALUE, &bstrValue));

		AnimationItemImageData si;
		si.index = uiIndex;
		it->second.columns[iColumnIndex] = si;
	}

	return S_OK;
}

STDMETHODIMP CSkinTimeline::AnimationGetParams(UINT* puiMilliseconds)
{
	CHECK_E_POINTER(puiMilliseconds);
	*puiMilliseconds = 50;
	return S_OK;
}

STDMETHODIMP CSkinTimeline::AnimationGetIndexes(UINT* puiIndexArray, UINT* puiCount)
{
	CHECK_E_POINTER(puiCount);
	*puiCount = m_steps.size();
	if (puiIndexArray)
	{
		auto index = 0;
		for (auto& item : m_steps)
		{
			puiIndexArray[index] = item.first;
			index++;
		}
	}
	return S_OK;
}

STDMETHODIMP CSkinTimeline::AnimationNextFrame(BOOL* pbContinueAnimation)
{
	CHECK_E_POINTER(pbContinueAnimation);

	vector<UINT> vIndexesToRemove;
	for (auto& item : m_steps)
	{
		if (item.second.step < STEPS + 1)
			item.second.step += 1;

		if (item.second.alpha < MAX_ALPHA)
			item.second.alpha += STEP_ALPHA;

		vector<int> vKeysToRemove;
		for (auto& imageItem : item.second.columns)
		{
			imageItem.second.step += 1;
			imageItem.second.alpha += STEP_ALPHA;

			if (imageItem.second.step == STEPS)
				imageItem.second.alpha = MAX_ALPHA;

			if (imageItem.second.step > STEPS)
				vKeysToRemove.push_back(imageItem.first);
		}

		for (auto& keyToRemove : vKeysToRemove)
		{
			item.second.columns.erase(keyToRemove);
		}

		if (item.second.step == STEPS)
			item.second.alpha = MAX_ALPHA;

		if (item.second.step > STEPS && item.second.columns.size() == 0)
			vIndexesToRemove.push_back(item.first);
	}

	for (auto& item : vIndexesToRemove)
	{
		m_steps.erase(item);
	}

	*pbContinueAnimation = m_steps.size() != 0;
	return S_OK;
}

