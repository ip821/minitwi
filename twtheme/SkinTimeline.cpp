// cpp.SkinTimeline : Implementation of CSkinTimeline

#include "stdafx.h"
#include "SkinTimeline.h"
#include "Plugins.h"
#include "GdilPlusUtils.h"
#include "Metadata.h"
#include "..\model-libs\\layout\Functions.h"

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

	if (m_pImageManagerService)
	{
		CComPtr<IImageManagerService> pLayoutImageManagerService;
		RETURN_IF_FAILED(m_pLayoutManager->GetImageManagerService(&pLayoutImageManagerService));
		RETURN_IF_FAILED(pLayoutImageManagerService->CopyImageTo(Twitter::Themes::Metadata::TimelineControl::RetweetImageKey, m_pImageManagerService));
	}
	return S_OK;
}

STDMETHODIMP CSkinTimeline::DrawItem(IColumnsInfo* pColumnsInfo, TDRAWITEMSTRUCTTIMELINE* lpdis)
{
	CDCHandle cdcReal = lpdis->lpdi->hDC;
	CRect rect = lpdis->lpdi->rcItem;

	auto bSelected = (lpdis->lpdi->itemState & ODS_SELECTED) != 0;
	CComPtr<IColumnsInfoItem> pItem;
	RETURN_IF_FAILED(pColumnsInfo->GetItem(0, &pItem));
	RETURN_IF_FAILED(pItem->SetVariantValue(Layout::Metadata::Element::Selected, &CComVar(bSelected)));

	CDC cdc;
	cdc.CreateCompatibleDC(cdcReal);
	CDCSelectBitmapManualScope cdcSelectBitmapManualScope;

	shared_ptr<CBitmap> pbitmap = make_shared<CBitmap>();
	pbitmap->CreateCompatibleBitmap(cdcReal, rect.Width(), rect.Height());
	cdcSelectBitmapManualScope.SelectBitmap(cdc, pbitmap.get()->m_hBitmap);

	RETURN_IF_FAILED(m_pLayoutManager->EraseBackground(cdc, pColumnsInfo));
	RETURN_IF_FAILED(m_pLayoutManager->PaintLayout(cdc, m_pImageManagerService, pColumnsInfo));

	BLENDFUNCTION bf = { 0 };
	bf.BlendOp = AC_SRC_OVER;
	bf.SourceConstantAlpha = MAX_ALPHA;
	if (m_steps.find(lpdis->lpdi->itemID) != m_steps.end())
	{
		bf.SourceConstantAlpha = m_steps[lpdis->lpdi->itemID].alpha;
	}
	cdcReal.AlphaBlend(rect.left, rect.top, rect.Width(), rect.Height(), cdc, 0, 0, rect.Width(), rect.Height(), bf);
	return S_OK;
}

void CSkinTimeline::GetValue(IVariantObject* pItemObject, const CComBSTR& bstrColumnName, CString& strValue)
{
	CComVar v;
	pItemObject->GetVariantValue(bstrColumnName, &v);
	if (v.vt == VT_BSTR)
		strValue = v.bstrVal;
}

STDMETHODIMP CSkinTimeline::MeasureItem(HDC hdc, RECT* pClientRect, IVariantObject* pItemObject, TMEASUREITEMSTRUCT* lpMeasureItemStruct, IColumnsInfo* pColumnsInfo)
{
	RETURN_IF_FAILED(pColumnsInfo->Clear());
	CComPtr<IVariantObject> pLayoutObject;

	CString strObjectType;
	GetValue(pItemObject, ObjectModel::Metadata::Object::Type, strObjectType);
	if (strObjectType == Twitter::Metadata::Types::CustomTimelineObject)
	{
		RETURN_IF_FAILED(m_pLayoutManager->GetLayout(Twitter::Themes::Metadata::TimelineControl::LayoutNameCustomItem, &pLayoutObject));
	}
	else if (strObjectType == Twitter::Connection::Metadata::ListObject::TypeId)
	{
		RETURN_IF_FAILED(m_pLayoutManager->GetLayout(Twitter::Themes::Metadata::TimelineControl::LayoutNameListItem, &pLayoutObject));
	}
	else if (strObjectType == Twitter::Connection::Metadata::TweetObject::TypeId)
	{
		RETURN_IF_FAILED(m_pLayoutManager->GetLayout(Twitter::Themes::Metadata::TimelineControl::LayoutNameItem, &pLayoutObject));
	}

	{
		CComPtr<IVariantObject> pItem;
		RETURN_IF_FAILED(HrLayoutFindItemByName(pLayoutObject, Twitter::Connection::Metadata::UserObject::Image, &pItem));
		if (pItem)
		{
			CComBSTR bstrImageUrl;
			RETURN_IF_FAILED(HrVariantObjectGetBSTR(pItemObject, Twitter::Connection::Metadata::UserObject::Image, &bstrImageUrl));
			RETURN_IF_FAILED(pItem->SetVariantValue(Layout::Metadata::ImageColumn::ImageKey, &CComVar(bstrImageUrl)));
			RETURN_IF_FAILED(pItem->SetVariantValue(Twitter::Metadata::Object::Value, &CComVar(bstrImageUrl)));
		}
	}

	{
		CComPtr<IVariantObject> pItem;
		RETURN_IF_FAILED(HrLayoutFindItemByName(pLayoutObject, Twitter::Themes::Metadata::TimelineControl::Elements::UserRetweetContainer, &pItem));
		if (pItem)
		{
			CComVar vRetweetedUserDisplayName;
			RETURN_IF_FAILED(pItemObject->GetVariantValue(Twitter::Connection::Metadata::TweetObject::RetweetedUserDisplayName, &vRetweetedUserDisplayName));
			RETURN_IF_FAILED(HrLayoutSetVariantValueRecursive(pItem, Layout::Metadata::Element::Visible, &CComVar(vRetweetedUserDisplayName.vt == VT_BSTR)));
		}
	}

	{ // Image and url containers are filled dynamically
		std::unordered_set<std::wstring> imageUrls;

		{ //Images
			CComVar vMediaUrls;
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

						CComVar vMediaUrlShort;
						pMediaObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrlShort, &vMediaUrlShort);

						imageUrls.insert(vMediaUrlShort.bstrVal);
					}
				}
			}
		}

		{ //Urls
			CComVar vUrls;
			RETURN_IF_FAILED(pItemObject->GetVariantValue(Twitter::Connection::Metadata::TweetObject::Urls, &vUrls));
			if (vUrls.vt == VT_UNKNOWN)
			{
				CComPtr<IVariantObject> pItem;
				RETURN_IF_FAILED(HrLayoutFindItemByName(pLayoutObject, Twitter::Themes::Metadata::TimelineControl::Elements::UrlContainer, &pItem));
				ATLASSERT(pItem);
				CComVar vElements;
				RETURN_IF_FAILED(pItem->GetVariantValue(Layout::Metadata::Element::Elements, &vElements));
				ATLASSERT(vElements.vt == VT_UNKNOWN);
				CComQIPtr<IObjCollection> pElements = vElements.punkVal;
				ATLASSERT(pElements);
				CComQIPtr<IBstrCollection> pBstrCollection = vUrls.punkVal;
				UINT_PTR uiCount = 0;
				RETURN_IF_FAILED(pBstrCollection->GetCount(&uiCount));
				for (size_t i = 0; i < uiCount; i++)
				{
					CComBSTR bstrUrl;
					RETURN_IF_FAILED(pBstrCollection->GetItem(i, &bstrUrl));
					if (imageUrls.find(bstrUrl.m_str) != imageUrls.end())
						continue;

					CComPtr<IVariantObject> pUrlItem;
					RETURN_IF_FAILED(m_pLayoutManager->GetLayout(Twitter::Connection::Metadata::TweetObject::Url, &pUrlItem));
					RETURN_IF_FAILED(pUrlItem->SetVariantValue(Layout::Metadata::TextColumn::Text, &CComVar(bstrUrl)));
					RETURN_IF_FAILED(pUrlItem->SetVariantValue(Twitter::Metadata::Object::Value, &CComVar(bstrUrl)));
					RETURN_IF_FAILED(pUrlItem->SetVariantValue(Twitter::Metadata::Item::VAR_IS_URL, &CComVar(true)));
					RETURN_IF_FAILED(pElements->AddObject(pUrlItem));
				}
			}
		}

		{ //Images
			CComVar vMediaUrls;
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

						CComVar vMediaUrlThumb;
						pMediaObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrlThumb, &vMediaUrlThumb);

						CComVar vWidth;
						RETURN_IF_FAILED(pMediaObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaThumbWidth, &vWidth));
						ATLASSERT(vWidth.vt == VT_I4);
						totalImageWidth += vWidth.intVal;
					}

					totalImageWidth = min(totalImageWidth, IMAGE_WIDTH_MAX);

					const UINT oneImageWidthMax = (totalImageWidth / processCount);

					UINT maxPossibleHeight = TIMELINE_IMAGE_HEIGHT;
					CComPtr<IVariantObject> pItem;
					RETURN_IF_FAILED(HrLayoutFindItemByName(pLayoutObject, Twitter::Themes::Metadata::TimelineControl::Elements::ImageContainer, &pItem));
					CComVar vElements;
					RETURN_IF_FAILED(pItem->GetVariantValue(Layout::Metadata::Element::Elements, &vElements));
					ATLASSERT(vElements.vt == VT_UNKNOWN);
					CComQIPtr<IObjCollection> pElements = vElements.punkVal;
					ATLASSERT(pElements);
					for (size_t i = 0; i < processCount; i++)
					{
						CComPtr<IVariantObject> pMediaObject;
						pObjArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pMediaObject);

						CComVar vMediaUrl;
						pMediaObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrl, &vMediaUrl);

						CComVar vMediaVideoUrl;
						pMediaObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaVideoUrl, &vMediaVideoUrl);

						CComVar vMediaUrlThumb;
						pMediaObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrlThumb, &vMediaUrlThumb);

						CComVar vHeight;
						RETURN_IF_FAILED(pMediaObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaThumbHeight, &vHeight));
						ATLASSERT(vHeight.vt == VT_I4);
						CComVar vWidth;
						RETURN_IF_FAILED(pMediaObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaThumbWidth, &vWidth));
						ATLASSERT(vWidth.vt == VT_I4);

						const int width = min(oneImageWidthMax, (UINT)vWidth.intVal);
						const int height = min(maxPossibleHeight, (UINT)vHeight.intVal);

						CComPtr<IVariantObject> pImageElement;
						RETURN_IF_FAILED(m_pLayoutManager->GetLayout(Twitter::Connection::Metadata::TweetObject::Image, &pImageElement));
						RETURN_IF_FAILED(pImageElement->SetVariantValue(Layout::Metadata::ImageColumn::ImageKey, &CComVar(vMediaUrlThumb.bstrVal)));
						RETURN_IF_FAILED(pImageElement->SetVariantValue(Layout::Metadata::ImageColumn::Height, &CComVar(height)));
						RETURN_IF_FAILED(pImageElement->SetVariantValue(Layout::Metadata::ImageColumn::Width, &CComVar(width)));
						RETURN_IF_FAILED(pImageElement->SetVariantValue(Twitter::Metadata::Item::VAR_IS_URL, &CComVar(true)));
						RETURN_IF_FAILED(pImageElement->SetVariantValue(Twitter::Metadata::Object::Value, &CComVar(vMediaUrlThumb.bstrVal)));
						RETURN_IF_FAILED(pImageElement->SetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrl, &CComVar(vMediaUrl.bstrVal)));
						RETURN_IF_FAILED(pImageElement->SetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaVideoUrl, &CComVar(vMediaVideoUrl.bstrVal)));
						RETURN_IF_FAILED(pImageElement->SetVariantValue(Twitter::Metadata::Item::VAR_IS_IMAGE, &CComVar(true)));
						RETURN_IF_FAILED(pElements->AddObject(pImageElement));
					}
				}
			}
		}
	}

	CRect rect;
	rect.right = pClientRect->right - pClientRect->left;
	rect.bottom = pClientRect->bottom - pClientRect->top;
	RETURN_IF_FAILED(m_pLayoutManager->BuildLayout(hdc, &rect, pLayoutObject, pItemObject, m_pImageManagerService, pColumnsInfo));

	{
		CComPtr<IColumnsInfoItem> pItem;
		RETURN_IF_FAILED(pColumnsInfo->GetItem(0, &pItem));
		CRect rectItem;
		RETURN_IF_FAILED(pItem->GetRect(&rectItem));
		lpMeasureItemStruct->itemHeight = rectItem.Height();
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

STDMETHODIMP CSkinTimeline::SetTheme(ITheme* pTheme)
{
	m_pThemeColorMap.Release();
	m_pThemeFontMap.Release();
	m_pLayoutManager.Release();
	if (pTheme)
	{
		RETURN_IF_FAILED(pTheme->GetFontMap(&m_pThemeFontMap));
		RETURN_IF_FAILED(pTheme->GetColorMap(&m_pThemeColorMap));
		RETURN_IF_FAILED(pTheme->GetLayoutManager(&m_pLayoutManager));
	}
	return S_OK;
}