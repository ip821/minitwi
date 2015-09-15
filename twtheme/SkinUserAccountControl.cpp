#include "stdafx.h"
#include "SkinUserAccountControl.h"
#include "..\twiconn\Plugins.h"
#include "GdilPlusUtils.h"
#include "Metadata.h"
#include "..\model-libs\layout\Functions.h"

#define DISTANCE_DISPLAY_NAME 20 / 200
#define DISTANCE_DESCRIPTION_Y 20
#define DISTANCE_COUNTERS_Y 15
#define DISTANCE_DESCRIPTION_X 20
#define DISTANCE_USER_IMAGE_X 20
#define ALPHA_USER_IMAGE 200

STDMETHODIMP CSkinUserAccountControl::SetImageManagerService(IImageManagerService* pImageManagerService)
{
	CHECK_E_POINTER(pImageManagerService);
	m_pImageManagerService = pImageManagerService;
	return S_OK;
}

STDMETHODIMP CSkinUserAccountControl::EraseBackground(HDC hdc)
{
	RETURN_IF_FAILED(m_pLayoutManager->EraseBackground(hdc, m_pColumnsInfo));
	return S_OK;
}

STDMETHODIMP CSkinUserAccountControl::Draw(HDC hdc)
{
	RETURN_IF_FAILED(m_pLayoutManager->PaintLayout(hdc, m_pImageManagerService, m_pColumnsInfo));
	return S_OK;
}

STDMETHODIMP CSkinUserAccountControl::StartAnimation(SkinUserAccountControlAnimationType animationType)
{
	if (animationType == SkinUserAccountControlAnimationType::BackgroundImage)
	{
		CComPtr<IVariantObject> pItemUserBannerImage;
		ASSERT_IF_FAILED(HrLayoutFindItemByName(m_pLayout, Twitter::Themes::Metadata::UserAccountControl::UserAccountControlLayoutBackgroundImage, &pItemUserBannerImage));
		ASSERT_IF_FAILED(pItemUserBannerImage->SetVariantValue(Layout::Metadata::ImageColumn::Alpha, &CComVar((DWORD)0)));
		ASSERT_IF_FAILED(pItemUserBannerImage->SetVariantValue(Layout::Metadata::Element::Visible, &CComVar(true)));
	}
	else if (animationType == SkinUserAccountControlAnimationType::UserImage)
	{
		CComPtr<IVariantObject> pItemUserImage;
		ASSERT_IF_FAILED(HrLayoutFindItemByName(m_pLayout, Twitter::Themes::Metadata::UserAccountControl::TwitterUserImage, &pItemUserImage));
		ASSERT_IF_FAILED(pItemUserImage->SetVariantValue(Layout::Metadata::ImageColumn::Alpha, &CComVar((DWORD)0)));
		ASSERT_IF_FAILED(pItemUserImage->SetVariantValue(Layout::Metadata::Element::Visible, &CComVar(true)));
	}
	return S_OK;
}

STDMETHODIMP CSkinUserAccountControl::AnimationSetValue(SkinUserAccountControlAnimationType animationType, DWORD dwValue)
{
	if (animationType == SkinUserAccountControlAnimationType::BackgroundImage)
	{
		CComPtr<IVariantObject> pItemUserBannerImage;
		ASSERT_IF_FAILED(HrLayoutFindItemByName(m_pLayout, Twitter::Themes::Metadata::UserAccountControl::UserAccountControlLayoutBackgroundImage, &pItemUserBannerImage));
		RETURN_IF_FAILED(pItemUserBannerImage->SetVariantValue(Layout::Metadata::ImageColumn::Alpha, &CComVar(dwValue)));
	}
	else if (animationType == SkinUserAccountControlAnimationType::UserImage)
	{
		CComPtr<IVariantObject> pItemUserBannerImage;
		ASSERT_IF_FAILED(HrLayoutFindItemByName(m_pLayout, Twitter::Themes::Metadata::UserAccountControl::TwitterUserImage, &pItemUserBannerImage));
		RETURN_IF_FAILED(pItemUserBannerImage->SetVariantValue(Layout::Metadata::ImageColumn::Alpha, &CComVar(dwValue)));
	}
	return S_OK;
}

STDMETHODIMP CSkinUserAccountControl::Measure(HDC hdc, LPRECT lpRect, IVariantObject* pVariantObject, BOOL bFollowing, BOOL bFollowButtonDisabled)
{
	{
		CComPtr<IVariantObject> pItemButtonString;
		RETURN_IF_FAILED(HrLayoutFindItemByName(m_pLayout, Twitter::Themes::Metadata::UserAccountControl::FollowButtonText, &pItemButtonString));
		RETURN_IF_FAILED(pItemButtonString->SetVariantValue(Layout::Metadata::TextColumn::Text, &CComVar(bFollowing ? L"Following" : L"  Follow  ")));
	}

	{
		CComPtr<IVariantObject> pItemButtonContainer;
		RETURN_IF_FAILED(HrLayoutFindItemByName(m_pLayout, Twitter::Themes::Metadata::UserAccountControl::FollowButtonContainer, &pItemButtonContainer));
		RETURN_IF_FAILED(pItemButtonContainer->SetVariantValue(Layout::Metadata::Element::Disabled, &CComVar(bFollowButtonDisabled ? true : false)));
		RETURN_IF_FAILED(pItemButtonContainer->SetVariantValue(Layout::Metadata::Element::Selected, &CComVar(bFollowing ? true : false)));
	}

	CComPtr<IVariantObject> pItemUserBannerImage;
	RETURN_IF_FAILED(HrLayoutFindItemByName(m_pLayout, Twitter::Themes::Metadata::UserAccountControl::UserAccountControlLayoutBackgroundImage, &pItemUserBannerImage));

	if (pVariantObject)
	{
		CComVar vUserImage;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Image, &vUserImage));
		CComPtr<IVariantObject> pItemUserImage;
		RETURN_IF_FAILED(HrLayoutFindItemByName(m_pLayout, Twitter::Themes::Metadata::UserAccountControl::TwitterUserImage, &pItemUserImage));
		RETURN_IF_FAILED(pItemUserImage->SetVariantValue(Layout::Metadata::ImageColumn::ImageKey, &vUserImage));

		CComVar vBannerUrl;
		pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Banner, &vBannerUrl);
		if (vBannerUrl.vt == VT_BSTR)
		{
			RETURN_IF_FAILED(pItemUserBannerImage->SetVariantValue(Layout::Metadata::ImageColumn::ImageKey, &vBannerUrl));
		}
	}

	RETURN_IF_FAILED(m_pColumnsInfo->Clear());
	RETURN_IF_FAILED(m_pLayoutManager->BuildLayout(hdc, lpRect, m_pLayout, pVariantObject, m_pImageManagerService, m_pColumnsInfo));
	return S_OK;
}

HRESULT CSkinUserAccountControl::SetTheme(ITheme* pTheme)
{
	CHECK_E_POINTER(pTheme);
	RETURN_IF_FAILED(pTheme->GetLayout(Twitter::Themes::Metadata::UserAccountControl::LayoutName, &m_pLayout));
	RETURN_IF_FAILED(pTheme->GetLayoutManager(&m_pLayoutManager));

	CComPtr<IVariantObject> pItemUserBannerImage;
	RETURN_IF_FAILED(HrLayoutFindItemByName(m_pLayout, Twitter::Themes::Metadata::UserAccountControl::UserAccountControlLayoutBackgroundImage, &pItemUserBannerImage));
	RETURN_IF_FAILED(pItemUserBannerImage->SetVariantValue(Layout::Metadata::Element::Visible, &CComVar(false)));

	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ColumnsInfo, &m_pColumnsInfo));

	return S_OK;
}

STDMETHODIMP CSkinUserAccountControl::GetRects(RECT* pRectUserImage, RECT* pRectFollowButton)
{
	{
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(m_pColumnsInfo->FindItemByName(Twitter::Connection::Metadata::UserObject::Image, &pColumnsInfoItem));
		RETURN_IF_FAILED(pColumnsInfoItem->GetRect(pRectUserImage));
	}

	{
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(m_pColumnsInfo->FindItemByName(Twitter::Themes::Metadata::UserAccountControl::FollowButtonContainer, &pColumnsInfoItem));
		RETURN_IF_FAILED(pColumnsInfoItem->GetRect(pRectFollowButton));
	}
	return S_OK;
}