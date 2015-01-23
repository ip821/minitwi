// SkinTimeline.cpp : Implementation of CSkinTimeline

#include "stdafx.h"
#include "SkinTimeline.h"
#include "Plugins.h"
#include "GdilPlusUtils.h"
#include "CustomListBox.h"

// CSkinTimeline

#define COL_NAME_LEFT 64
#define COLUMN_X_SPACING 5
#define COLUMN_Y_SPACING 0
#define PADDING_Y 7
#define ITEM_SPACING 10
#define ITEM_DELIMITER_HEIGHT 1
#define MAX_ITEM_HEIGHT 255
#define IMAGE_WIDTH_MAX 275

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
	if (m_pImageManagerService)
		m_pImageManagerService.Release();
	m_pImageManagerService = pImageManagerService;
	return S_OK;
}

STDMETHODIMP CSkinTimeline::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	m_pThemeColorMap = pThemeColorMap;
	return S_OK;
}


STDMETHODIMP CSkinTimeline::SetFontMap(IThemeFontMap* pThemeFontMap)
{
	m_pThemeFontMap = pThemeFontMap;
	return S_OK;
}

STDMETHODIMP CSkinTimeline::DrawItem(HWND hwndControl, IColumnsInfo* pColumnsInfo, TDRAWITEMSTRUCTTIMELINE* lpdis)
{
	if (m_steps.find(lpdis->lpdi->itemID) == m_steps.end())
	{
		RETURN_IF_FAILED(DrawTextColumns(hwndControl, pColumnsInfo, lpdis));
		RETURN_IF_FAILED(DrawImageColumns(pColumnsInfo, lpdis));
		return S_OK;
	}

	CRect rect = lpdis->lpdi->rcItem;
	CDCHandle cdcReal = lpdis->lpdi->hDC;

	CDC cdc;
	cdc.CreateCompatibleDC(cdcReal);
	CDCSelectBitmapManualScope cdcSelectBitmapManualScope;

	if (m_cacheBitmaps.find(lpdis->lpdi->itemID) == m_cacheBitmaps.end())
	{
		shared_ptr<CBitmap> pbitmap = make_shared<CBitmap>();
		pbitmap->CreateCompatibleBitmap(cdcReal, rect.Width(), rect.Height());
		cdcSelectBitmapManualScope.SelectBitmap(cdc, pbitmap.get()->m_hBitmap);

		m_cacheBitmaps[lpdis->lpdi->itemID] = pbitmap;

		lpdis->lpdi->hDC = cdc;
		lpdis->lpdi->rcItem.left = 0;
		lpdis->lpdi->rcItem.top = 0;
		lpdis->lpdi->rcItem.right = rect.Width();
		lpdis->lpdi->rcItem.bottom = rect.Height();
		RETURN_IF_FAILED(DrawTextColumns(hwndControl, pColumnsInfo, lpdis));
		lpdis->lpdi->rcItem = rect;
		lpdis->lpdi->hDC = cdcReal;
	}
	else
	{
		cdcSelectBitmapManualScope.SelectBitmap(cdc, m_cacheBitmaps[lpdis->lpdi->itemID].get()->m_hBitmap);
	}

	BLENDFUNCTION bf = { 0 };
	bf.BlendOp = AC_SRC_OVER;
	bf.SourceConstantAlpha = MAX_ALPHA;
	if (m_steps.find(lpdis->lpdi->itemID) != m_steps.end())
	{
		bf.SourceConstantAlpha = m_steps[lpdis->lpdi->itemID].alpha;
	}
	cdcReal.AlphaBlend(rect.left, rect.top, rect.Width(), rect.Height(), cdc, 0, 0, rect.Width(), rect.Height(), bf);
	RETURN_IF_FAILED(DrawImageColumns(pColumnsInfo, lpdis));
	return S_OK;
}

STDMETHODIMP CSkinTimeline::DrawImageColumns(IColumnsInfo* pColumnsInfo, TDRAWITEMSTRUCTTIMELINE* lpdis)
{
	std::hash_set<UINT> columnIndexesAlreadyAnimated;

	if (lpdis->puiNotAnimatedColumnIndexes)
	{
		for (size_t i = 0; i < lpdis->uiNotAnimatedColumnIndexesCount; i++)
		{
			columnIndexesAlreadyAnimated.insert(lpdis->puiNotAnimatedColumnIndexes[i]);
		}
	}

	CDCHandle cdc = lpdis->lpdi->hDC;
	cdc.SetBkMode(TRANSPARENT);

	UINT uiCount = 0;
	RETURN_IF_FAILED(pColumnsInfo->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(pColumnsInfo->GetItem(i, &pColumnsInfoItem));
		CRect rect;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRect(&rect));
		CComBSTR bstrColumnName;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(VAR_COLUMN_NAME, &bstrColumnName));
		BOOL bIsImage = FALSE;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectBoolProp(VAR_IS_IMAGE, &bIsImage));
		CComBSTR bstrValue;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(VAR_VALUE, &bstrValue));

		if (!bIsImage)
			continue;

		CBitmap bitmap;
		m_pImageManagerService->CreateImageBitmap(bstrValue, &bitmap.m_hBitmap);
		if (!bitmap.m_hBitmap)
			continue;

		TBITMAP tBitmap = { 0 };
		RETURN_IF_FAILED(m_pImageManagerService->GetImageInfo(bstrValue, &tBitmap));

		CDC cdcBitmap;
		cdcBitmap.CreateCompatibleDC(lpdis->lpdi->hDC);
		CDCSelectBitmapScope cdcSelectBitmapScope(cdcBitmap, bitmap);

		auto x = lpdis->lpdi->rcItem.left;
		auto y = lpdis->lpdi->rcItem.top;
		auto width = rect.Width();
		auto height = rect.Height();

		BLENDFUNCTION bf = { 0 };
		bf.BlendOp = AC_SRC_OVER;
		bf.SourceConstantAlpha = 0;

		auto it = m_steps.find(lpdis->lpdi->itemID);
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

		if (bstrColumnName == VAR_TWITTER_USER_IMAGE && bf.SourceConstantAlpha == MAX_ALPHA)
		{
			static DWORD dwColor = 0;
			if (!dwColor)
			{
				RETURN_IF_FAILED(m_pThemeColorMap->GetColor(VAR_BRUSH_BACKGROUND, &dwColor));
			}
			cdc.TransparentBlt(x + rect.left, y + rect.top, width, height, cdcBitmap, 0, 0, width, height, dwColor);
		}
		else
		{
			cdc.AlphaBlend(x + rect.left, y + rect.top, width, height, cdcBitmap, 0, 0, width, height, bf);
		}
	}
	return S_OK;
}

STDMETHODIMP CSkinTimeline::DrawTextColumns(HWND hwndControl, IColumnsInfo* pColumnsInfo, TDRAWITEMSTRUCTTIMELINE* lpdis)
{
	std::hash_set<UINT> columnIndexesAlreadyAnimated;

	if (lpdis->puiNotAnimatedColumnIndexes)
	{
		for (size_t i = 0; i < lpdis->uiNotAnimatedColumnIndexesCount; i++)
		{
			columnIndexesAlreadyAnimated.insert(lpdis->puiNotAnimatedColumnIndexes[i]);
		}
	}

	CListBox wndListBox(hwndControl);

	RECT clientRect = { 0 };
	wndListBox.GetClientRect(&clientRect);

	CDCHandle cdc = lpdis->lpdi->hDC;
	cdc.SetBkMode(TRANSPARENT);

	BOOL bDisabledSelection = FALSE;
	ASSERT_IF_FAILED(pColumnsInfo->IsDisabledSelection(&bDisabledSelection));

	if (lpdis->lpdi->itemState & ODS_SELECTED && !bDisabledSelection)
	{
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(VAR_BRUSH_SELECTED, &dwColor));
		CBrush brush;
		brush.CreateSolidBrush(dwColor);
		cdc.FillRect(&(lpdis->lpdi->rcItem), brush);
	}
	else
	{
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(VAR_BRUSH_BACKGROUND, &dwColor));
		CBrush brush;
		brush.CreateSolidBrush(dwColor);
		RECT rect = lpdis->lpdi->rcItem;
		rect.bottom -= COLUMN_Y_SPACING;
		cdc.FillRect(&rect, brush);
	}

	if (!bDisabledSelection)
	{
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(VAR_TWITTER_DELIMITER, &dwColor));
		CBrush brush;
		brush.CreateSolidBrush(dwColor);
		RECT rect = lpdis->lpdi->rcItem;
		rect.top = rect.bottom - ITEM_DELIMITER_HEIGHT;
		cdc.FillRect(&rect, brush);
	}

	UINT uiCount = 0;
	RETURN_IF_FAILED(pColumnsInfo->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(pColumnsInfo->GetItem(i, &pColumnsInfoItem));
		RECT rect = { 0 };
		RETURN_IF_FAILED(pColumnsInfoItem->GetRect(&rect));
		CComBSTR bstrColumnName;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(VAR_COLUMN_NAME, &bstrColumnName));
		CComBSTR bstrText;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(VAR_TEXT, &bstrText));
		BOOL bIsUrl = FALSE;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectBoolProp(VAR_IS_URL, &bIsUrl));
		BOOL bIsWordWrap = FALSE;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectBoolProp(VAR_IS_WORDWRAP, &bIsWordWrap));
		BOOL bIsImage = FALSE;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectBoolProp(VAR_IS_IMAGE, &bIsImage));
		BOOL bDoubleSize = FALSE;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectBoolProp(VAR_ITEM_DOUBLE_SIZE, &bDoubleSize));

		HFONT font = 0;
		auto bstrFontName = CComBSTR(bstrColumnName);
		if (bDoubleSize)
			bstrFontName += VAR_DOUBLE_SIZE_POSTFIX;
		RETURN_IF_FAILED(m_pThemeFontMap->GetFont(bstrFontName, &font));

		if (lpdis->iHoveredItem == static_cast<int>(lpdis->lpdi->itemID) && lpdis->iHoveredColumn == static_cast<int>(i) && bIsUrl)
		{
			RETURN_IF_FAILED(m_pThemeFontMap->GetFont(bstrFontName + VAR_SELECTED_POSTFIX, &font));
		}

		if (!bIsImage)
		{
			DWORD dwColor = 0;
			RETURN_IF_FAILED(m_pThemeColorMap->GetColor(bstrColumnName, &dwColor));
			cdc.SetTextColor(dwColor);

			auto x = lpdis->lpdi->rcItem.left;
			auto y = lpdis->lpdi->rcItem.top;

			CString str(bstrText);
			CDCSelectFontScope cdcSelectFontScope(cdc, font);
			RECT rectText = { x + rect.left, y + rect.top, x + rect.right, y + rect.bottom };
			cdc.DrawText(str, str.GetLength(), &rectText, bIsWordWrap ? DT_WORDBREAK : 0);
		}
	}

	return S_OK;
}

SIZE CSkinTimeline::AddColumn(
	HDC hdc,
	IColumnsInfo* pColumnsInfo,
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
	BOOL bDisabledSelection = FALSE,
	BOOL bDoubleSize = FALSE
	)
{
	CDC cdc;
	cdc.CreateCompatibleDC(hdc);
	HFONT font = nullptr;
	auto bstrFontName = CComBSTR(strColumnName);
	if (bDoubleSize)
		bstrFontName += VAR_DOUBLE_SIZE_POSTFIX;
	m_pThemeFontMap->GetFont(bstrFontName, &font);
	CDCSelectFontScope cdcSelectFontScope(cdc, font);

	SIZE sz = { 0 };
	if (bWordWrap)
	{
		RECT rect = { 0 };
		rect.right = size.cx;
		rect.bottom = size.cy;

		sz.cx = size.cx;
		sz.cy = DrawText(cdc, strDisplayText, strDisplayText.GetLength(), &rect, DT_WORDBREAK | DT_CALCRECT);
	}
	else
	{
		GetTextExtentPoint32(cdc, strDisplayText, strDisplayText.GetLength(), &sz);
		if (ulMinimumFixedWidth)
			sz.cx = max(ulMinimumFixedWidth, sz.cx);
	}

	switch (justify)
	{
	case Justify::Center:
		x = (size.cx / 2) - (sz.cx / 2);
		break;

	case Justify::Right:
		x = x - sz.cx;
		break;

	case Justify::None:
	default:
		break;
	}

	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	ASSERT_IF_FAILED(pColumnsInfo->AddItem(&pColumnsInfoItem));
	ASSERT_IF_FAILED(pColumnsInfoItem->SetRect(CRect(x, y, x + sz.cx, y + sz.cy)));
	if (bDisabledSelection)
	{
		ASSERT_IF_FAILED(pColumnsInfo->DisableSelection(TRUE));
	}
	ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(VAR_COLUMN_NAME, CComBSTR(strColumnName)));
	ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(VAR_TEXT, CComBSTR(strDisplayText)));
	ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(VAR_VALUE, CComBSTR(strValue)));
	if (bWordWrap)
	{
		ASSERT_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(VAR_IS_WORDWRAP, TRUE));
	}
	if (bIsUrl)
	{
		ASSERT_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(VAR_IS_URL, TRUE));
	}
	if (bDoubleSize)
	{
		ASSERT_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(VAR_ITEM_DOUBLE_SIZE, TRUE));
	}
	return sz;
}

void CSkinTimeline::GetValue(IVariantObject* pItemObject, CComBSTR& bstrColumnName, CString& strValue)
{
	CComVariant v;
	pItemObject->GetVariantValue(bstrColumnName, &v);
	if (v.vt == VT_BSTR)
		strValue = v.bstrVal;
}

STDMETHODIMP CSkinTimeline::MeasureItem(HWND hwndControl, IVariantObject* pItemObject, TMEASUREITEMSTRUCT* lpMeasureItemStruct, IColumnsInfo* pColumnsInfo)
{
	pColumnsInfo->Clear();
	CListBox wndListBox(hwndControl);

	CClientDC hdc(hwndControl);

	CRect clientRect = { 0 };
	wndListBox.GetClientRect(&clientRect);

	CString strObjectType;
	GetValue(pItemObject, CComBSTR(VAR_OBJECT_TYPE), strObjectType);

	if (strObjectType == TYPE_CUSTOM_TIMELINE_OBJECT)
	{
		CString strCustomText;
		GetValue(pItemObject, CComBSTR(VAR_TEXT), strCustomText);

		CString strCustomDisabledText;
		GetValue(pItemObject, CComBSTR(VAR_ITEM_DISABLED_TEXT), strCustomDisabledText);

		CComVariant vDisabled;
		pItemObject->GetVariantValue(VAR_ITEM_DISABLED, &vDisabled);
		auto bDisabled = vDisabled.vt == VT_BOOL && vDisabled.boolVal;
		auto y = PADDING_Y;
		CSize sz = AddColumn(
			hdc,
			pColumnsInfo,
			CString(VAR_COLUMN_SHOW_MORE),
			bDisabled ? strCustomDisabledText : strCustomText,
			bDisabled ? strCustomDisabledText : strCustomText,
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
	else if (strObjectType == TYPE_TWITTER_OBJECT || strObjectType == TYPE_LIST_OBJECT)
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
		
		CComVariant vDoubleSize;
		RETURN_IF_FAILED(pItemObject->GetVariantValue(VAR_ITEM_DOUBLE_SIZE, &vDoubleSize));

		{
			auto x = 10;
			auto y = PADDING_Y * 2;
			CComPtr<IColumnsInfoItem> pColumnsInfoItem;
			ASSERT_IF_FAILED(pColumnsInfo->AddItem(&pColumnsInfoItem));
			ASSERT_IF_FAILED(pColumnsInfoItem->SetRect(CRect(x, y, x + 48, y + 48)));
			ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(VAR_COLUMN_NAME, CComBSTR(VAR_TWITTER_USER_IMAGE)));
			ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(VAR_TEXT, L""));
			ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(VAR_VALUE, CComBSTR(strImageUrl)));
			ASSERT_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(VAR_IS_IMAGE, TRUE));
			ASSERT_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(VAR_IS_URL, TRUE));
		}

		CSize sizeRetweetedDislpayName;
		UINT uiIndex = 0;
		if (!strRetweetedDisplayName.IsEmpty())
		{
			auto x = COL_NAME_LEFT;
			auto y = PADDING_Y;

			sizeRetweetedDislpayName = AddColumn(
				hdc,
				pColumnsInfo,
				CString(VAR_TWITTER_RETWEETED_USER_DISPLAY_NAME),
				L"Retweeted by " + strRetweetedDisplayName + L" @" + strRetweetedName,
				strRetweetedDisplayName,
				x,
				y,
				CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, MAX_ITEM_HEIGHT),
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
				pColumnsInfo,
				CString(VAR_TWITTER_USER_DISPLAY_NAME),
				strDisplayName,
				strDisplayName,
				x,
				y,
				CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, MAX_ITEM_HEIGHT)
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
				pColumnsInfo,
				CString(VAR_TWITTER_USER_NAME),
				L"@" + strName,
				strName,
				x,
				y,
				CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, MAX_ITEM_HEIGHT)
				);
		}

		if (strObjectType != TYPE_LIST_OBJECT)
		{
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
					pColumnsInfo,
					CString(VAR_TWITTER_RELATIVE_TIME),
					strCreatedAt,
					strCreatedAt,
					x,
					y,
					CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, MAX_ITEM_HEIGHT),
					TRUE,
					FALSE,
					30,
					Justify::Right
					);
			}
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

			auto bDoubleSize = vDoubleSize.vt == VT_BOOL && vDoubleSize.boolVal;
			sizeText = AddColumn(
				hdc,
				pColumnsInfo,
				CString(VAR_TWITTER_NORMALIZED_TEXT),
				strText,
				strText,
				x,
				y,
				CSize((clientRect.right - clientRect.left - COLUMN_X_SPACING * 2) - COL_NAME_LEFT, MAX_ITEM_HEIGHT),
				FALSE,
				TRUE,
				0,
				Justify::None,
				FALSE,
				bDoubleSize
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
					const size_t processCount = uiCount;
					for (size_t i = 0; i < processCount; i++)
					{
						CComPtr<IVariantObject> pMediaObject;
						pObjArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pMediaObject);

						CComVariant vMediaUrlShort;
						pMediaObject->GetVariantValue(VAR_TWITTER_MEDIAURL_SHORT, &vMediaUrlShort);

						imageUrls.insert(vMediaUrlShort.bstrVal);
					}
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

				auto bDoubleSize = vDoubleSize.vt == VT_BOOL && vDoubleSize.boolVal;
				auto size = AddColumn(
					hdc,
					pColumnsInfo,
					CString(VAR_TWITTER_URL),
					CString(bstrUrl),
					CString(bstrUrl),
					x,
					lastY,
					CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, MAX_ITEM_HEIGHT),
					TRUE,
					FALSE,
					0,
					Justify::None,
					FALSE,
					bDoubleSize
					);

				lastY += size.cy;
			}
		}

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
					auto totalImageWidth = 0;
					const size_t processCount = uiCount;
					for (size_t i = 0; i < processCount; i++)
					{
						CComPtr<IVariantObject> pMediaObject;
						pObjArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pMediaObject);

						CComVariant vMediaUrlThumb;
						pMediaObject->GetVariantValue(VAR_TWITTER_MEDIAURL_THUMB, &vMediaUrlThumb);

						TBITMAP tBitmap = { 0 };
						m_pImageManagerService->GetImageInfo(vMediaUrlThumb.bstrVal, &tBitmap);
						totalImageWidth += tBitmap.Width;
					}

					totalImageWidth = min(totalImageWidth, IMAGE_WIDTH_MAX);

					const UINT oneImageWidthMax = (totalImageWidth / processCount);
					auto xOffset = (clientRect.Width() - totalImageWidth) / 2;

					UINT maxPossibleHeight = min(TIMELINE_IMAGE_HEIGHT, MAX_ITEM_HEIGHT - lastY - ITEM_SPACING);
					UINT lastHeight = 0;
					for (size_t i = 0; i < processCount; i++)
					{
						CComPtr<IVariantObject> pMediaObject;
						pObjArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pMediaObject);

						CComVariant vMediaUrl;
						pMediaObject->GetVariantValue(VAR_TWITTER_MEDIAURL, &vMediaUrl);

						CComVariant vMediaUrlThumb;
						pMediaObject->GetVariantValue(VAR_TWITTER_MEDIAURL_THUMB, &vMediaUrlThumb);

						TBITMAP tBitmap = { 0 };
						m_pImageManagerService->GetImageInfo(vMediaUrlThumb.bstrVal, &tBitmap);

						const int oneImageWidth = min(oneImageWidthMax, tBitmap.Width);

						auto x = xOffset;
						auto y = lastY;
						auto width = oneImageWidth - 4;
						auto height = min(maxPossibleHeight, tBitmap.Height);
						lastHeight = max(height, lastHeight);

						xOffset += oneImageWidth;

						CComPtr<IColumnsInfoItem> pColumnsInfoItem;
						ASSERT_IF_FAILED(pColumnsInfo->AddItem(&pColumnsInfoItem));
						ASSERT_IF_FAILED(pColumnsInfoItem->SetRect(CRect(x, y, x + width, y + height)));
						ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(VAR_COLUMN_NAME, CComBSTR(VAR_TWITTER_IMAGE)));
						ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(VAR_TEXT, L""));
						ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(VAR_VALUE, vMediaUrlThumb.bstrVal));
						ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(VAR_TWITTER_MEDIAURL, vMediaUrl.bstrVal));
						ASSERT_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(VAR_IS_IMAGE, TRUE));
						ASSERT_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(VAR_IS_URL, TRUE));
					}

					lastY += lastHeight;
				}
			}
		}

		lpMeasureItemStruct->itemHeight = min(MAX_ITEM_HEIGHT, max(48 + PADDING_Y * 2 + PADDING_Y * 2, lastY + ITEM_SPACING));
		lpMeasureItemStruct->itemWidth = sizeText.cx;
	}
	return S_OK;
}

STDMETHODIMP CSkinTimeline::AnimationRegisterItemIndex(UINT uiIndex, IColumnsInfoItem* pColumnsInfoItem, int iColumnIndex)
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

	if (iColumnIndex == CCustomListBox::INVALID_COLUMN_INDEX)
	{
		it->second.step = 0;
		it->second.alpha = 0;
	}
	else
	{
		CComBSTR bstrValue;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(VAR_VALUE, &bstrValue));

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
		m_cacheBitmaps.erase(item);
	}

	*pbContinueAnimation = m_steps.size() != 0;
	return S_OK;
}

