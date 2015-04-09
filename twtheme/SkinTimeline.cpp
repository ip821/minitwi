// SkinTimeline.cpp : Implementation of CSkinTimeline

#include "stdafx.h"
#include "SkinTimeline.h"
#include "Plugins.h"
#include "GdilPlusUtils.h"

// CSkinTimeline

#define COL_NAME_LEFT 64
#define COLUMN_X_SPACING 5
#define COLUMN_Y_SPACING 0
#define PADDING_Y 7
#define ITEM_SPACING 10
#define ITEM_DELIMITER_HEIGHT 1
#define IMAGE_WIDTH_MAX 275
#define RETWEET_IMAGE_KEY L"RetweetImage"

CSkinTimeline::CSkinTimeline()
{
}

HRESULT CSkinTimeline::FinalConstruct()
{
	InitImageFromResource(IDR_RETWEET, L"PNG", m_pBitmapRetweet);
	return S_OK;
}

void CSkinTimeline::FinalRelease()
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

	CBitmap bmp;
	m_pBitmapRetweet->GetHBITMAP(Color::Transparent, &bmp.m_hBitmap);
	m_pImageManagerService->AddImageFromHBITMAP(RETWEET_IMAGE_KEY, bmp);
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
	CDCHandle cdcReal = lpdis->lpdi->hDC;
	CRect rect = lpdis->lpdi->rcItem;

	if (m_steps.find(lpdis->lpdi->itemID) == m_steps.end())
	{
		RETURN_IF_FAILED(DrawTextColumns(hwndControl, pColumnsInfo, lpdis));
		RETURN_IF_FAILED(DrawImageColumns(pColumnsInfo, lpdis));
		return S_OK;
	}


	CDC cdc;
	cdc.CreateCompatibleDC(cdcReal);
	CDCSelectBitmapManualScope cdcSelectBitmapManualScope;

	shared_ptr<CBitmap> pbitmap = make_shared<CBitmap>();
	pbitmap->CreateCompatibleBitmap(cdcReal, rect.Width(), rect.Height());
	cdcSelectBitmapManualScope.SelectBitmap(cdc, pbitmap.get()->m_hBitmap);

	lpdis->lpdi->hDC = cdc;
	lpdis->lpdi->rcItem.left = 0;
	lpdis->lpdi->rcItem.top = 0;
	lpdis->lpdi->rcItem.right = rect.Width();
	lpdis->lpdi->rcItem.bottom = rect.Height();
	RETURN_IF_FAILED(DrawTextColumns(hwndControl, pColumnsInfo, lpdis));
	lpdis->lpdi->rcItem = rect;
	lpdis->lpdi->hDC = cdcReal;

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
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(Twitter::Metadata::Column::Name, &bstrColumnName));
		BOOL bIsImage = FALSE;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectBoolProp(Twitter::Metadata::Item::VAR_IS_IMAGE, &bIsImage));
		CComBSTR bstrValue;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(Twitter::Metadata::Object::Value, &bstrValue));

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

		if (bstrColumnName == Twitter::Connection::Metadata::UserObject::Image && bf.SourceConstantAlpha == MAX_ALPHA)
		{
			static DWORD dwColor = 0;
			if (!dwColor)
			{
				RETURN_IF_FAILED(m_pThemeColorMap->GetColor(Twitter::Metadata::Drawing::BrushBackground, &dwColor));
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
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(Twitter::Metadata::Drawing::BrushSelected, &dwColor));
		CBrush brush;
		brush.CreateSolidBrush(dwColor);
		cdc.FillRect(&(lpdis->lpdi->rcItem), brush);
	}
	else
	{
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(Twitter::Metadata::Drawing::BrushBackground, &dwColor));
		CBrush brush;
		brush.CreateSolidBrush(dwColor);
		RECT rect = lpdis->lpdi->rcItem;
		rect.bottom -= COLUMN_Y_SPACING;
		cdc.FillRect(&rect, brush);
	}

	if (!bDisabledSelection)
	{
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(Twitter::Metadata::Item::VAR_TWITTER_DELIMITER, &dwColor));
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
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(Twitter::Metadata::Column::Name, &bstrColumnName));
		CComBSTR bstrText;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(Twitter::Metadata::Object::Text, &bstrText));
		BOOL bIsUrl = FALSE;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectBoolProp(Twitter::Metadata::Item::VAR_IS_URL, &bIsUrl));
		BOOL bIsWordWrap = FALSE;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectBoolProp(Twitter::Metadata::Item::VAR_IS_WORDWRAP, &bIsWordWrap));
		BOOL bIsImage = FALSE;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectBoolProp(Twitter::Metadata::Item::VAR_IS_IMAGE, &bIsImage));
		BOOL bDoubleSize = FALSE;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectBoolProp(Twitter::Metadata::Item::VAR_ITEM_DOUBLE_SIZE, &bDoubleSize));

		HFONT font = 0;
		auto bstrFontName = CComBSTR(bstrColumnName);
		if (bDoubleSize)
			bstrFontName += Twitter::Metadata::Item::VAR_DOUBLE_SIZE_POSTFIX;
		RETURN_IF_FAILED(m_pThemeFontMap->GetFont(bstrFontName, &font));

		if (lpdis->iHoveredItem == static_cast<int>(lpdis->lpdi->itemID) && lpdis->iHoveredColumn == static_cast<int>(i) && bIsUrl)
		{
			CComBSTR bstrFontNameTemp = bstrFontName;
			bstrFontNameTemp.Append(Twitter::Metadata::Item::VAR_SELECTED_POSTFIX);
			RETURN_IF_FAILED(m_pThemeFontMap->GetFont(bstrFontNameTemp, &font));
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
		bstrFontName += Twitter::Metadata::Item::VAR_DOUBLE_SIZE_POSTFIX;
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
	ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Column::Name, CComBSTR(strColumnName)));
	ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Object::Text, CComBSTR(strDisplayText)));
	ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Object::Value, CComBSTR(strValue)));
	if (bWordWrap)
	{
		ASSERT_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(Twitter::Metadata::Item::VAR_IS_WORDWRAP, TRUE));
	}
	if (bIsUrl)
	{
		ASSERT_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(Twitter::Metadata::Item::VAR_IS_URL, TRUE));
	}
	if (bDoubleSize)
	{
		ASSERT_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(Twitter::Metadata::Item::VAR_ITEM_DOUBLE_SIZE, TRUE));
	}
	return sz;
}

void CSkinTimeline::GetValue(IVariantObject* pItemObject, const CComBSTR& bstrColumnName, CString& strValue)
{
	CComVariant v;
	pItemObject->GetVariantValue(bstrColumnName, &v);
	if (v.vt == VT_BSTR)
		strValue = v.bstrVal;
}

STDMETHODIMP CSkinTimeline::InitImageFromResource(int nId, LPCTSTR lpType, shared_ptr<Gdiplus::Bitmap>& pBitmap)
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

STDMETHODIMP CSkinTimeline::MeasureItem(HWND hwndControl, IVariantObject* pItemObject, TMEASUREITEMSTRUCT* lpMeasureItemStruct, IColumnsInfo* pColumnsInfo)
{
	pColumnsInfo->Clear();
	CListBox wndListBox(hwndControl);

	CClientDC hdc(hwndControl);

	CRect clientRect = { 0 };
	wndListBox.GetClientRect(&clientRect);

	CString strObjectType;
	GetValue(pItemObject, ObjectModel::Metadata::Object::Type, strObjectType);

	if (strObjectType == Twitter::Metadata::Types::CustomTimelineObject)
	{
		CString strCustomText;
		GetValue(pItemObject, Twitter::Metadata::Object::Text, strCustomText);

		CString strCustomDisabledText;
		GetValue(pItemObject, Twitter::Metadata::Item::VAR_ITEM_DISABLED_TEXT, strCustomDisabledText);

		CComVariant vDisabled;
		pItemObject->GetVariantValue(Twitter::Metadata::Item::VAR_ITEM_DISABLED, &vDisabled);
		auto bDisabled = vDisabled.vt == VT_BOOL && vDisabled.boolVal;
		auto y = PADDING_Y;
		CSize sz = AddColumn(
			hdc,
			pColumnsInfo,
			CString(Twitter::Metadata::Column::ShowMoreColumn),
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
	else if (strObjectType == Twitter::Connection::Metadata::TweetObject::TypeId || strObjectType == Twitter::Connection::Metadata::ListObject::TypeId)
	{
		CString strRetweetedDisplayName;
		GetValue(pItemObject, Twitter::Connection::Metadata::TweetObject::RetweetedUserDisplayName, strRetweetedDisplayName);

		CString strRetweetedName;
		GetValue(pItemObject, Twitter::Connection::Metadata::TweetObject::RetweetedUserName, strRetweetedName);

		CString strDisplayName;
		GetValue(pItemObject, Twitter::Connection::Metadata::UserObject::DisplayName, strDisplayName);

		CString strCreatedAt;
		GetValue(pItemObject, Twitter::Metadata::Item::VAR_TWITTER_RELATIVE_TIME, strCreatedAt);

		CString strName;
		GetValue(pItemObject, Twitter::Connection::Metadata::UserObject::Name, strName);

		CString strText;
		GetValue(pItemObject, Twitter::Connection::Metadata::TweetObject::NormalizedText, strText);

		CString strImageUrl;
		GetValue(pItemObject, Twitter::Connection::Metadata::UserObject::Image, strImageUrl);

		CComVariant vDoubleSize;
		RETURN_IF_FAILED(pItemObject->GetVariantValue(Twitter::Metadata::Item::VAR_ITEM_DOUBLE_SIZE, &vDoubleSize));

		CSize sizeRetweetedDislpayName;
		UINT uiIndex = 0;
		if (!strRetweetedDisplayName.IsEmpty())
		{
			TBITMAP tBitmap = { 0 };
			ASSERT_IF_FAILED(m_pImageManagerService->GetImageInfo(RETWEET_IMAGE_KEY, &tBitmap));

			{
				auto x = COL_NAME_LEFT;
				auto y = COLUMN_Y_SPACING + PADDING_Y;

				CComPtr<IColumnsInfoItem> pColumnsInfoItem;
				ASSERT_IF_FAILED(pColumnsInfo->AddItem(&pColumnsInfoItem));
				ASSERT_IF_FAILED(pColumnsInfoItem->SetRect(CRect(x, y, x + tBitmap.Width, y + tBitmap.Height)));
				ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Column::Name, RETWEET_IMAGE_KEY));
				ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Object::Text, L""));
				ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Object::Value, CComBSTR(RETWEET_IMAGE_KEY)));
				ASSERT_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(Twitter::Metadata::Item::VAR_IS_IMAGE, TRUE));
				ASSERT_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(Twitter::Metadata::Item::VAR_IS_URL, FALSE));

				tBitmap.Width += 3;
			}

			{
				auto x = COL_NAME_LEFT + tBitmap.Width;
				auto y = PADDING_Y;

				sizeRetweetedDislpayName = AddColumn(
					hdc,
					pColumnsInfo,
					CString(Twitter::Connection::Metadata::TweetObject::RetweetedUserDisplayName),
					CString(L"Retweeted by "),
					CString(L"Retweeted by "),
					x,
					y,
					CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, clientRect.Height()),
					FALSE,
					FALSE
					);
			}

			{
				auto x = COL_NAME_LEFT + sizeRetweetedDislpayName.cx + tBitmap.Width;
				auto y = PADDING_Y;

				CSize temp = AddColumn(
					hdc,
					pColumnsInfo,
					CString(Twitter::Connection::Metadata::TweetObject::RetweetedUserDisplayName),
					strRetweetedDisplayName,
					strRetweetedName,
					x,
					y,
					CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, clientRect.Height()),
					TRUE,
					FALSE
					);

				sizeRetweetedDislpayName.cx += temp.cx;
			}

			{
				auto x = COL_NAME_LEFT + sizeRetweetedDislpayName.cx + tBitmap.Width;
				auto y = PADDING_Y;

				CSize temp = AddColumn(
					hdc,
					pColumnsInfo,
					CString(Twitter::Connection::Metadata::TweetObject::RetweetedUserName),
					L"@" + strRetweetedName,
					strRetweetedName,
					x,
					y,
					CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, clientRect.Height()),
					TRUE,
					FALSE
					);

				sizeRetweetedDislpayName.cx += temp.cx;
			}
		}

		{
			auto x = 10;
			auto y = COLUMN_Y_SPACING + PADDING_Y + 4;

			if (sizeRetweetedDislpayName.cy)
			{
				y += sizeRetweetedDislpayName.cy + COLUMN_Y_SPACING;
			}

			CComPtr<IColumnsInfoItem> pColumnsInfoItem;
			ASSERT_IF_FAILED(pColumnsInfo->AddItem(&pColumnsInfoItem));
			ASSERT_IF_FAILED(pColumnsInfoItem->SetRect(CRect(x, y, x + 48, y + 48)));
			ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Column::Name, Twitter::Connection::Metadata::UserObject::Image));
			ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Object::Text, L""));
			ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Connection::Metadata::MediaObject::MediaUrl, CComBSTR(strImageUrl)));
			ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Object::Value, CComBSTR(strImageUrl)));
			ASSERT_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(Twitter::Metadata::Item::VAR_IS_IMAGE, TRUE));
			ASSERT_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(Twitter::Metadata::Item::VAR_IS_URL, TRUE));
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
				CString(Twitter::Connection::Metadata::UserObject::DisplayName),
				strDisplayName,
				strDisplayName,
				x,
				y,
				CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, clientRect.Height())
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
				CString(Twitter::Connection::Metadata::UserObject::Name),
				L"@" + strName,
				strName,
				x,
				y,
				CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, clientRect.Height())
				);
		}

		if (strObjectType != Twitter::Connection::Metadata::ListObject::TypeId)
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
					CString(Twitter::Metadata::Item::VAR_TWITTER_RELATIVE_TIME),
					strCreatedAt,
					strCreatedAt,
					x,
					y,
					CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, clientRect.Height()),
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
				CString(Twitter::Connection::Metadata::TweetObject::NormalizedText),
				strText,
				strText,
				x,
				y,
				CSize((clientRect.right - clientRect.left - COLUMN_X_SPACING * 2) - COL_NAME_LEFT, clientRect.Height()),
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
			pItemObject->GetVariantValue(Twitter::Connection::Metadata::TweetObject::MediaUrls, &vMediaUrls);
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
						pMediaObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrlShort, &vMediaUrlShort);

						imageUrls.insert(vMediaUrlShort.bstrVal);
					}
				}
			}
		}

		CComVariant vUrls;
		pItemObject->GetVariantValue(Twitter::Connection::Metadata::TweetObject::Urls, &vUrls);
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
					CString(Twitter::Connection::Metadata::TweetObject::Url),
					CString(bstrUrl),
					CString(bstrUrl),
					x,
					lastY,
					CSize((clientRect.right - clientRect.left) - COL_NAME_LEFT, clientRect.Height()),
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
			pItemObject->GetVariantValue(Twitter::Connection::Metadata::TweetObject::MediaUrls, &vMediaUrls);
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
						pMediaObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrlThumb, &vMediaUrlThumb);

						TBITMAP tBitmap = { 0 };
						if (m_pImageManagerService->GetImageInfo(vMediaUrlThumb.bstrVal, &tBitmap) != S_OK)
						{
							CComVariant vHeight;
							RETURN_IF_FAILED(pMediaObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaThumbHeight, &vHeight));
							CComVariant vWidth;
							RETURN_IF_FAILED(pMediaObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaThumbWidth, &vWidth));
							tBitmap.Width = vWidth.intVal;
							tBitmap.Height = vHeight.intVal;
						}
						totalImageWidth += tBitmap.Width;
					}

					totalImageWidth = min(totalImageWidth, IMAGE_WIDTH_MAX);

					const UINT oneImageWidthMax = (totalImageWidth / processCount);
					auto xOffset = (clientRect.Width() - totalImageWidth) / 2;

					UINT maxPossibleHeight = TIMELINE_IMAGE_HEIGHT;
					UINT lastHeight = 0;
					for (size_t i = 0; i < processCount; i++)
					{
						CComPtr<IVariantObject> pMediaObject;
						pObjArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pMediaObject);

						CComVariant vMediaUrl;
						pMediaObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrl, &vMediaUrl);

						CComVariant vMediaVideoUrl;
						pMediaObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaVideoUrl, &vMediaVideoUrl);

						CComVariant vMediaUrlThumb;
						pMediaObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrlThumb, &vMediaUrlThumb);

						TBITMAP tBitmap = { 0 };
						if (m_pImageManagerService->GetImageInfo(vMediaUrlThumb.bstrVal, &tBitmap) != S_OK)
						{
							CComVariant vHeight;
							RETURN_IF_FAILED(pMediaObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaThumbHeight, &vHeight));
							CComVariant vWidth;
							RETURN_IF_FAILED(pMediaObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaThumbWidth, &vWidth));
							tBitmap.Width = vWidth.intVal;
							tBitmap.Height = vHeight.intVal;
						}

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
						ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Column::Name, Twitter::Connection::Metadata::TweetObject::Image));
						ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Object::Text, L""));
						ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Object::Value, vMediaUrlThumb.bstrVal));
						ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Connection::Metadata::MediaObject::MediaUrl, vMediaUrl.bstrVal));
						ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Connection::Metadata::MediaObject::MediaVideoUrl, vMediaVideoUrl.bstrVal));
						ASSERT_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(Twitter::Metadata::Item::VAR_IS_IMAGE, TRUE));
						ASSERT_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(Twitter::Metadata::Item::VAR_IS_URL, TRUE));
					}

					lastY += lastHeight;
				}
			}
		}

		lpMeasureItemStruct->itemHeight = max(48 + PADDING_Y * 2 + PADDING_Y * 2, lastY + ITEM_SPACING);
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

	if (iColumnIndex == INVALID_COLUMN_INDEX)
	{
		it->second.step = 0;
		it->second.alpha = 0;
	}
	else
	{
		CComBSTR bstrValue;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(Twitter::Metadata::Object::Value, &bstrValue));

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

