#include "stdafx.h"
#include "SkinUserAccountControl.h"
#include "..\twiconn\Plugins.h"
#include "GdilPlusUtils.h"

#define DISTANCE_DISPLAY_NAME 20 / 200
#define DISTANCE_DESCRIPTION_Y 20
#define DISTANCE_COUNTERS_Y 15
#define DISTANCE_DESCRIPTION_X 20
#define DISTANCE_USER_IMAGE_X 20
#define ALPHA_USER_IMAGE 200

STDMETHODIMP CSkinUserAccountControl::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	CHECK_E_POINTER(pThemeColorMap);
	m_pThemeColorMap = pThemeColorMap;
	return S_OK;
}

STDMETHODIMP CSkinUserAccountControl::SetFontMap(IThemeFontMap* pThemeFontMap)
{
	CHECK_E_POINTER(pThemeFontMap);
	m_pThemeFontMap = pThemeFontMap;
	return S_OK;
}

STDMETHODIMP CSkinUserAccountControl::SetImageManagerService(IImageManagerService* pImageManagerService)
{
	CHECK_E_POINTER(pImageManagerService);
	m_pImageManagerService = pImageManagerService;
	return S_OK;
}

STDMETHODIMP CSkinUserAccountControl::EraseBackground(HDC hdc, LPRECT lpRect, IVariantObject* pVariantObject)
{
	CRect rect = *lpRect;
	auto dwBackColor = Color((ARGB)Color::DarkSlateGray).ToCOLORREF();
	CBitmap bitmap;
	TBITMAP tBitmap = { 0 };

	CComVariant vBitmapUrl;
	pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Banner, &vBitmapUrl);
	if (vBitmapUrl.vt == VT_BSTR)
	{
		m_pImageManagerService->CreateImageBitmap(vBitmapUrl.bstrVal, &bitmap.m_hBitmap);
		m_pImageManagerService->GetImageInfo(vBitmapUrl.bstrVal, &tBitmap);
	}

	CDCHandle cdc(hdc);

	if (bitmap.IsNull())
	{
		CBrush brush;
		brush.CreateSolidBrush(dwBackColor);
		cdc.FillRect(&rect, brush);
	}
	else
	{
		CDC cdcBitmap;
		cdcBitmap.CreateCompatibleDC(cdc);
		CDCSelectBitmapScope cdcSelectBitmapScope(cdcBitmap, bitmap);

		auto x = 0;
		auto y = 0;
		int width = tBitmap.Width;
		int height = tBitmap.Height;

		if (width > rect.Width())
		{
			x = width / 2 - rect.Width() / 2;
			width = rect.Width();
		}

		if (height > rect.Height())
		{
			y = height / 2 - rect.Height() / 2;
			height = rect.Height();
		}

		BLENDFUNCTION bf = { 0 };
		bf.BlendOp = AC_SRC_OVER;
		bf.SourceConstantAlpha = (BYTE)m_alpha;
		cdc.AlphaBlend(0, 0, rect.Width(), rect.Height(), cdcBitmap, x, y, width, height, bf);
	}
	return S_OK;
}

STDMETHODIMP CSkinUserAccountControl::Draw(HDC hdc, LPRECT lpRect, IVariantObject* pVariantObject, IColumnsInfo* pColumnsInfo)
{
	CRect rect = *lpRect;
	CDCHandle cdc(hdc);

	CRect rectDisplayName;
	CRect rectScreenName;
	CRect rectBox;
	CRect rectFollowButton;
	RETURN_IF_FAILED(MeasureInternal(cdc, rect, pVariantObject, &rectScreenName, &rectDisplayName, &rectBox, &rectFollowButton));

	DWORD dwTextColor = 0;
	RETURN_IF_FAILED(m_pThemeColorMap->GetColor(Twitter::Metadata::Drawing::AccountControlText, &dwTextColor));

	cdc.SetBkMode(TRANSPARENT);
	cdc.SetTextColor(dwTextColor);

	CComVariant vDisplayName;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::DisplayName, &vDisplayName));
	ATLASSERT(vDisplayName.vt == VT_BSTR);

	HFONT font = 0;
	RETURN_IF_FAILED(m_pThemeFontMap->GetFont(Twitter::Connection::Metadata::UserObject::DisplayNameUserAccount, &font));
	CDCSelectFontScope cdcSelectFontScope(cdc, font);

	CComBSTR bstrDisplayName(vDisplayName.bstrVal);
	CSize szDisplayName;
	cdc.GetTextExtent(bstrDisplayName, bstrDisplayName.Length(), &szDisplayName);

	CComVariant vScreenName;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Name, &vScreenName));
	ATLASSERT(vScreenName.vt == VT_BSTR);
	CComBSTR bstrScreenName(CString(L"@") + vScreenName.bstrVal);

	DrawRoundedRect(cdc, rectDisplayName);
	cdc.DrawText(bstrDisplayName, bstrDisplayName.Length(), &rectDisplayName, 0);

	DrawRoundedRect(cdc, rectScreenName);
	cdc.DrawText(bstrScreenName, bstrScreenName.Length(), &rectScreenName, 0);

	DrawRoundedRect(cdc, rectBox, true);

	CComVariant vUserImage;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Image, &vUserImage));
	CBitmap bitmapUserImage;
	if (vUserImage.vt == VT_BSTR && SUCCEEDED(m_pImageManagerService->CreateImageBitmap(vUserImage.bstrVal, &bitmapUserImage.m_hBitmap)))
	{
		TBITMAP tBitmap = { 0 };
		RETURN_IF_FAILED(m_pImageManagerService->GetImageInfo(vUserImage.bstrVal, &tBitmap));

		CDC cdcBitmap;
		cdcBitmap.CreateCompatibleDC(cdc);
		CDCSelectBitmapScope cdcSelectBitmapScope(cdcBitmap, bitmapUserImage);

		auto maxWidth = max(rectDisplayName.Width(), rectScreenName.Width());
		auto boxHeight = rectDisplayName.Width() + rectScreenName.Width() + 1 - 10;
		auto boxWidth = boxHeight;
		auto boxLeft = rect.Width() / 2 - maxWidth / 2 - boxWidth / 2;
		auto boxTop = rectDisplayName.top + ((rectScreenName.bottom - rectDisplayName.top) / 2 - boxHeight / 2);
		auto diff_x = (boxWidth / 2) - (tBitmap.Width / 2);
		auto diff_y = (boxHeight / 2) - (tBitmap.Height / 2);
		CRect rectBimtpaUserImage;
		rectBimtpaUserImage.left = boxLeft + diff_x;
		rectBimtpaUserImage.top = boxTop + diff_y;
		rectBimtpaUserImage.right = rectBimtpaUserImage.left + tBitmap.Width;
		rectBimtpaUserImage.bottom = rectBimtpaUserImage.top + tBitmap.Height;
		static Color colorTransparent((ARGB)Color::White);
		cdc.TransparentBlt(rectBimtpaUserImage.left, rectBimtpaUserImage.top, rectBimtpaUserImage.Width(), rectBimtpaUserImage.Height(), cdcBitmap, 0, 0, tBitmap.Width, tBitmap.Height, colorTransparent.ToCOLORREF());
	}

	RETURN_IF_FAILED(m_pThemeFontMap->GetFont(Twitter::Connection::Metadata::TweetObject::NormalizedText, &font));
	CDCSelectFontScope cdcSelectFontScope2(cdc, font);

	CComVariant vDescription;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Description, &vDescription));
	ATLASSERT(vDescription.vt == VT_BSTR);
	CComBSTR bstrDescription(vDescription.bstrVal);
	CSize szDescription;
	szDescription.cx = rect.Width() - DISTANCE_DESCRIPTION_X * 2;
	{
		CRect rect1;
		rect1.right = szDescription.cx;
		rect1.bottom = rect.Height();
		szDescription.cy = cdc.DrawTextEx(bstrDescription, bstrDescription.Length(), &rect1, DT_WORDBREAK | DT_CENTER | DT_CALCRECT, NULL);
		szDescription.cx = rect1.Width();
	}
	CRect rectDescription;
	rectDescription.left = rect.Width() / 2 - szDescription.cx / 2;
	rectDescription.top = rectScreenName.bottom + DISTANCE_DESCRIPTION_Y;
	rectDescription.right = rectDescription.left + szDescription.cx;
	rectDescription.bottom = rectDescription.top + szDescription.cy;
	DrawRoundedRect(cdc, rectDescription);
	cdc.DrawTextEx(bstrDescription, bstrDescription.Length(), &rectDescription, DT_WORDBREAK | DT_CENTER, NULL);

	auto bottom = rect.bottom - DISTANCE_COUNTERS_Y;
	bottom = DrawCounter(cdc, DISTANCE_DESCRIPTION_X, bottom, rect.Width(), pVariantObject, Twitter::Connection::Metadata::UserObject::FollowersCount, L"Followers: ");
	bottom = DrawCounter(cdc, DISTANCE_DESCRIPTION_X, bottom, rect.Width(), pVariantObject, Twitter::Connection::Metadata::UserObject::FriendsCount, L"Following: ");
	bottom = DrawCounter(cdc, DISTANCE_DESCRIPTION_X, bottom, rect.Width(), pVariantObject, Twitter::Connection::Metadata::UserObject::TweetsCount, L"Tweets: ");

	BOOL bFollowButtonDisabled = FALSE;
	BOOL bFollowing = FALSE;
	UINT uiCount = 0;
	RETURN_IF_FAILED(pColumnsInfo->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(pColumnsInfo->GetItem(i, &pColumnsInfoItem));
		CComBSTR bstrColumnName;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(Twitter::Metadata::Column::Name, &bstrColumnName));
		if (bstrColumnName != Twitter::Metadata::Item::VAR_ITEM_FOLLOW_BUTTON)
			continue;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectBoolProp(Twitter::Metadata::Item::TwitterItemFollowButtonRectDisabled, &bFollowButtonDisabled));
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectBoolProp(Twitter::Metadata::Item::VAR_IS_FOLLOWING, &bFollowing));
	}

	HFONT hFontFollowButton = 0;
	RETURN_IF_FAILED(m_pThemeFontMap->GetFont(Twitter::Metadata::Item::VAR_ITEM_FOLLOW_BUTTON, &hFontFollowButton));
	cdc.SelectFont(hFontFollowButton);

	DWORD dwFollowButtonColor = 0;
	auto str = bFollowing ? m_strFollowing : m_strFollow;

	if (bFollowButtonDisabled)
	{
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(Twitter::Metadata::Item::TwitterItemFollowButtonRectDisabled, &dwFollowButtonColor));
	}
	else
	{
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(bFollowing ? Twitter::Metadata::Item::TwitterItemFollowButtonRectPushed : Twitter::Metadata::Item::TwitterItemFollowButtonRect, &dwFollowButtonColor));
	}

	DrawRoundedRect(cdc, rectFollowButton, true, dwFollowButtonColor);
	cdc.DrawText(str, str.GetLength(), rectFollowButton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	return S_OK;
}

int CSkinUserAccountControl::DrawCounter(HDC hdc, int x, int y, int width, IVariantObject* pVariantObject, BSTR bstrName, BSTR bstrMessage)
{
	CComVariant v;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(bstrName, &v));
	ATLASSERT(v.vt == VT_I4);

	CString strText = CString(bstrMessage) + boost::lexical_cast<wstring>(v.intVal).c_str();
	CDCHandle cdc(hdc);
	CSize sz;
	cdc.GetTextExtent(strText, strText.GetLength(), &sz);
	CRect rect;
	rect.left = x;
	rect.top = y - sz.cy;
	rect.right = rect.left + sz.cx;
	rect.bottom = y;
	DrawRoundedRect(cdc, rect);
	cdc.DrawText(strText, strText.GetLength(), &rect, 0);
	return y - sz.cy - 1;
}

STDMETHODIMP CSkinUserAccountControl::AnimationSetValue(DWORD dwValue)
{
	m_alpha = dwValue;
	return S_OK;
}

STDMETHODIMP CSkinUserAccountControl::Measure(HWND hWnd, LPRECT lpRect, IColumnsInfo* pColumnsInfo, IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pColumnsInfo);

	CRect rect = *lpRect;
	CClientDC cdc(hWnd);

	CRect rectUserImage;
	CRect rectFollowButton;
	RETURN_IF_FAILED(MeasureInternal(cdc, rect, pVariantObject, nullptr, nullptr, &rectUserImage, &rectFollowButton));

	{
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(pColumnsInfo->AddItem(&pColumnsInfoItem));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRect(rectUserImage));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Column::Name, Twitter::Connection::Metadata::UserObject::Image));
	}

	{
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(pColumnsInfo->AddItem(&pColumnsInfoItem));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRect(rectFollowButton));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Column::Name, Twitter::Metadata::Item::VAR_ITEM_FOLLOW_BUTTON));
	}

	return S_OK;
}

HRESULT CSkinUserAccountControl::MeasureInternal(HDC hdc, RECT clientRect, IVariantObject* pVariantObject, LPRECT lpRectScreenName, LPRECT lpRectDisplayName, LPRECT lpRectUserImage, LPRECT lpRectFollowButton)
{
	CDCHandle cdc(hdc);
	CRect rect = clientRect;

	CComVariant vDisplayName;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::DisplayName, &vDisplayName));
	ATLASSERT(vDisplayName.vt == VT_BSTR);

	HFONT font = 0;
	RETURN_IF_FAILED(m_pThemeFontMap->GetFont(Twitter::Connection::Metadata::UserObject::DisplayNameUserAccount, &font));
	CDCSelectFontScope cdcSelectFontScope(cdc, font);

	CComBSTR bstrDisplayName(vDisplayName.bstrVal);
	CSize szDisplayName;
	cdc.GetTextExtent(bstrDisplayName, bstrDisplayName.Length(), &szDisplayName);

	CComVariant vScreenName;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Name, &vScreenName));
	ATLASSERT(vScreenName.vt == VT_BSTR);
	CComBSTR bstrScreenName(CString(L"@") + vScreenName.bstrVal);
	CSize szScreenName;
	cdc.GetTextExtent(bstrScreenName, bstrScreenName.Length(), &szScreenName);

	auto maxWidth = max(szDisplayName.cx, szScreenName.cx);
	auto boxHeight = szDisplayName.cy + szScreenName.cy + 1 - 10;
	auto boxWidth = boxHeight;
	auto boxLeft = rect.Width() / 2 - maxWidth / 2 - boxWidth / 2;

	CRect rectDisplayName;
	rectDisplayName.left = boxLeft + boxWidth + DISTANCE_USER_IMAGE_X;
	rectDisplayName.top = rect.Height() * DISTANCE_DISPLAY_NAME;
	rectDisplayName.right = rectDisplayName.left + szDisplayName.cx;
	rectDisplayName.bottom = rectDisplayName.top + szDisplayName.cy;

	CRect rectScreenName;
	rectScreenName.left = boxLeft + boxWidth + DISTANCE_USER_IMAGE_X;
	rectScreenName.top = rectDisplayName.bottom + 1;
	rectScreenName.right = rectScreenName.left + szScreenName.cx;
	rectScreenName.bottom = rectScreenName.top + szScreenName.cy;

	auto boxTop = rectDisplayName.top + ((rectScreenName.bottom - rectDisplayName.top) / 2 - boxHeight / 2);
	CRect rectBox = CRect(boxLeft, boxTop, boxLeft + boxWidth, boxTop + boxHeight);

	HFONT hFontFollowButton = 0;
	RETURN_IF_FAILED(m_pThemeFontMap->GetFont(Twitter::Metadata::Item::VAR_ITEM_FOLLOW_BUTTON, &hFontFollowButton));
	cdc.SelectFont(hFontFollowButton);
	CSize szFollowButtonCaption;
	cdc.GetTextExtent(m_strFollowing, m_strFollowing.GetLength(), &szFollowButtonCaption);
	CRect rectFollowButton;
	rectFollowButton.bottom = rect.bottom - 15;
	rectFollowButton.top = rectFollowButton.bottom - szFollowButtonCaption.cy - 20;
	rectFollowButton.right = rect.right - 15;
	rectFollowButton.left = rectFollowButton.right - szFollowButtonCaption.cx - 20;

	if (lpRectScreenName)
		*lpRectScreenName = rectScreenName;
	if (lpRectDisplayName)
		*lpRectDisplayName = rectDisplayName;
	if (lpRectUserImage)
		*lpRectUserImage = rectBox;
	if (lpRectFollowButton)
		*lpRectFollowButton = rectFollowButton;

	return S_OK;
}
