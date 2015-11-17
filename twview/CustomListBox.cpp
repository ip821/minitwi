#include "stdafx.h"
#include "CustomListBox.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"
#include "..\model-libs\layout\GdilPlusUtils.h"

#define TARGET_RESOLUTION 1

CCustomListBox::CCustomListBox()
{
	m_HoveredItemIndex = INVALID_ITEM_INDEX;
	m_handCursor.LoadSysCursor(IDC_HAND);
	m_arrowCursor.LoadSysCursor(IDC_ARROW);
	HrCoCreateInstance(CLSID_ObjectCollection, &m_pItems);
}

CCustomListBox::~CCustomListBox()
{
}

HWND CCustomListBox::Create(HWND hWndParent, ATL::_U_RECT rect, LPCTSTR szWindowName, DWORD dwStyle, DWORD dwExStyle, ATL::_U_MENUorID MenuOrID, LPVOID lpCreateParam)
{
	HWND hWnd = CWindowImpl::Create(hWndParent, rect.m_lpRect, szWindowName, dwStyle, dwExStyle, MenuOrID.m_hMenu, lpCreateParam);
	return hWnd;
}

LRESULT CCustomListBox::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_animationTimerFade.SetHWND(m_hWnd);
	return 0;
}

void CCustomListBox::Clear()
{
	auto nID = GetDlgCtrlID();
	UINT uiCount = 0;
	ASSERT_IF_FAILED(m_pItems->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pVariantObject;
		ASSERT_IF_FAILED(m_pItems->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));

		NMITEMREMOVED nm = { 0 };
		nm.nmhdr.hwndFrom = m_hWnd;
		nm.nmhdr.idFrom = nID;
		nm.nmhdr.code = NM_ITEM_REMOVED;
		nm.pVariantObject = pVariantObject.p;
		nm.pColumnsInfo = m_columnsInfo[i];
		::SendMessage(GetParent(), WM_NOTIFY, (WPARAM)nID, (LPARAM)&nm);
	}
	ResetContent();
	ASSERT_IF_FAILED(m_pItems->Clear());
	m_columnsInfo.clear();
	m_animatedColumns.clear();
}

void CCustomListBox::RemoveItemByIndex(UINT uiIndex)
{
	auto nID = GetDlgCtrlID();

	DeleteString(uiIndex);

	CComPtr<IVariantObject> pVariantObject;
	ASSERT_IF_FAILED(m_pItems->GetAt(uiIndex, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));

	NMITEMREMOVED nm = { 0 };
	nm.nmhdr.hwndFrom = m_hWnd;
	nm.nmhdr.idFrom = nID;
	nm.nmhdr.code = NM_ITEM_REMOVED;
	nm.pVariantObject = pVariantObject.p;
	nm.pColumnsInfo = m_columnsInfo[uiIndex];
	::SendMessage(GetParent(), WM_NOTIFY, (WPARAM)nID, (LPARAM)&nm);

	ASSERT_IF_FAILED(m_pItems->RemoveObjectAt(uiIndex));
	m_columnsInfo.erase(m_columnsInfo.begin() + uiIndex);

	CComVar vId;
	ASSERT_IF_FAILED(pVariantObject->GetVariantValue(ObjectModel::Metadata::Object::Id, &vId));
	m_animatedColumns.erase(pVariantObject.p);
}

void CCustomListBox::RefreshItem(UINT uiIndex)
{
	MEASUREITEMSTRUCT s = { 0 };
	s.itemID = uiIndex;
	MeasureItem(&s);
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, LB_SETITEMHEIGHT, uiIndex, MAKELONG(s.itemHeight, 0));
	}
}

void CCustomListBox::DoSize(int cx, int cy)
{
	CStaticListBoxImpl::DoSize(cx, cy);
	if (!m_hWnd)
		return;

	auto itemCount = GetCount();
	for (int i = 0; i < itemCount; i++)
	{
		RefreshItem(i);
	}
	return;
}

void CCustomListBox::SetSkinTimeline(ISkinTimeline* pSkin)
{
	m_pSkinTimeline = pSkin;
}

void CCustomListBox::DrawItem(LPDRAWITEMSTRUCT lpdi)
{
	if (lpdi->itemID == (UINT)INVALID_ITEM_INDEX)
		return;

	CComPtr<IVariantObject> pVariantObject;
	ASSERT_IF_FAILED(m_pItems->GetAt(lpdi->itemID, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));
	auto& columns = m_animatedColumns[pVariantObject.p];
	vector<UINT> vIndexes(columns.begin(), columns.end());
	UINT* pui = nullptr;
	if (vIndexes.size())
		pui = &vIndexes[0];
	TDRAWITEMSTRUCTTIMELINE distl = { 0 };
	distl.lpdi = reinterpret_cast<TDRAWITEMSTRUCT*>(lpdi);
	distl.iHoveredItem = m_HoveredItemIndex;
	distl.puiNotAnimatedColumnIndexes = pui;
	distl.uiNotAnimatedColumnIndexesCount = vIndexes.size();
	ASSERT_IF_FAILED(m_pSkinTimeline->DrawItem(m_columnsInfo[lpdi->itemID], &distl));
}

void CCustomListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CComPtr<IVariantObject> pVariantObject;
	ASSERT_IF_FAILED(m_pItems->GetAt(lpMeasureItemStruct->itemID, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));
	CRect clientRect;
	GetClientRect(&clientRect);
	CClientDC hdc(m_hWnd);
	ASSERT_IF_FAILED(m_pSkinTimeline->MeasureItem(hdc, clientRect, pVariantObject.p, (TMEASUREITEMSTRUCT*)lpMeasureItemStruct, m_columnsInfo[lpMeasureItemStruct->itemID]));
}

void CCustomListBox::IsEmpty(BOOL* pbEmpty)
{
	UINT uiCount = 0;
	ASSERT_IF_FAILED(m_pItems->GetCount(&uiCount));
	*pbEmpty = uiCount == 0;
}

HRESULT CCustomListBox::GetItems(IObjArray** ppObjectArray)
{
	RETURN_IF_FAILED(m_pItems->QueryInterface(ppObjectArray));
	return S_OK;
}

void CCustomListBox::OnItemsUpdated()
{
	UINT uiCount = 0;
	ASSERT_IF_FAILED(m_pItems->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pVariantObject;
		ASSERT_IF_FAILED(m_pItems->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));

		CComVar v;
		ASSERT_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Metadata::Item::TwitterRelativeTime, &v));
		if (v.vt == VT_BSTR)
		{
			CComPtr<IColumnsInfoItem> pColumnsInfoItem;
			ASSERT_IF_FAILED(m_columnsInfo[i]->FindItemByName(Twitter::Metadata::Item::TwitterRelativeTime, &pColumnsInfoItem));
			if (pColumnsInfoItem)
			{
				ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Object::Text, v.bstrVal));
				ASSERT_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Twitter::Metadata::Object::Value, v.bstrVal));
			}
		}
	}
}

void CCustomListBox::InsertItem(IVariantObject* pItemObject, int index)
{
	ATLASSERT(m_pSettings);
	CComVar vId;
	ASSERT_IF_FAILED(pItemObject->GetVariantValue(ObjectModel::Metadata::Object::Id, &vId));

	CComPtr<IColumnsInfo> pColumnsInfo;
	ASSERT_IF_FAILED(HrCoCreateInstance(CLSID_ColumnsInfo, &pColumnsInfo));
	if (index == -1)
	{
		ASSERT_IF_FAILED(m_pItems->AddObject(pItemObject));
		m_columnsInfo.push_back(pColumnsInfo);
		index = m_columnsInfo.size() - 1;
	}
	else
	{
		ASSERT_IF_FAILED(m_pItems->InsertObject(pItemObject, index));
		m_columnsInfo.insert(m_columnsInfo.begin() + index, pColumnsInfo);
	}

	SendMessage(LB_INSERTSTRING, index, 0);

	ASSERT_IF_FAILED(m_pSkinTimeline->AnimationRegisterItemIndex(index, NULL, INVALID_COLUMN_INDEX));
	UpdateAnimatedColumns(pColumnsInfo, index, pItemObject, TRUE);
	m_bAnimationNeeded = TRUE;
}

void CCustomListBox::UpdateAnimatedColumns(IColumnsInfo* pColumnsInfo, int itemIndex, IVariantObject* pVariantObject, BOOL bRegisterForAnimation)
{
	CComPtr<IImageManagerService> pImageManagerService;
	ASSERT_IF_FAILED(m_pSkinTimeline->GetImageManagerService(&pImageManagerService));

	CComPtr<IObjArray> pImageItems;
	ASSERT_IF_FAILED(pColumnsInfo->FindItemsByProperty(Twitter::Metadata::Item::VAR_IS_IMAGE, TRUE, &pImageItems));

	UINT uiCount = 0;
	ASSERT_IF_FAILED(pImageItems->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		ASSERT_IF_FAILED(pImageItems->GetAt(i, __uuidof(IColumnsInfoItem), (LPVOID*)&pColumnsInfoItem));

		BOOL bImage = FALSE;
		ASSERT_IF_FAILED(pColumnsInfoItem->GetRectBoolProp(Twitter::Metadata::Item::VAR_IS_IMAGE, &bImage));
		if (!bImage)
			continue;

		CComBSTR bstrValue;
		ASSERT_IF_FAILED(pColumnsInfoItem->GetRectStringProp(Twitter::Metadata::Object::Value, &bstrValue));

		BOOL bContains = FALSE;
		ASSERT_IF_FAILED(pImageManagerService->ContainsImageKey(bstrValue, &bContains));

		if (!bContains)
			continue;

		if (m_animatedColumns[pVariantObject].find(i) != m_animatedColumns[pVariantObject].end())
			continue;

		m_animatedColumns[pVariantObject].insert(i);
		if (bRegisterForAnimation)
		{
			ASSERT_IF_FAILED(m_pSkinTimeline->AnimationRegisterItemIndex(itemIndex, pColumnsInfoItem, i));
		}
	}
}

LRESULT CCustomListBox::OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	auto x = GET_X_LPARAM(lParam);
	auto y = GET_Y_LPARAM(lParam);

	auto prevItemIndex = m_HoveredItemIndex;
	auto prevColumnName = m_bstrHoveredColumnName;

	SetCursor(m_arrowCursor);

	m_HoveredItemIndex = INVALID_ITEM_INDEX;
	m_bstrHoveredColumnName.Empty();

	if (m_columnsInfo.empty())
		return 0;

	CRect rectPrevious;

	{ //Current point
		BOOL bOutside = FALSE;
		int uiItem = ItemFromPoint(CPoint(x, y), bOutside);
		RECT itemRect = { 0 };
		GetItemRect(uiItem, &itemRect);
		if (!bOutside && uiItem != INVALID_ITEM_INDEX)
		{
			CComPtr<IColumnsInfo> pColumnsInfo = m_columnsInfo[uiItem];
			CPoint pt(x, y);
			pt.Offset(0, -itemRect.top);
			CComPtr<IColumnsInfoItem> pColumnsInfoItem;
			ASSERT_IF_FAILED(pColumnsInfo->FindItemByPoint(&pt, &pColumnsInfoItem));
			if (pColumnsInfoItem)
			{
				BOOL bIsUrl = FALSE;
				ASSERT_IF_FAILED(pColumnsInfoItem->GetRectBoolProp(Twitter::Metadata::Item::VAR_IS_URL, &bIsUrl));

				if (bIsUrl)
				{
					SetCursor(m_handCursor);
					m_HoveredItemIndex = uiItem;
					ASSERT_IF_FAILED(HrVariantObjectGetBSTR(pColumnsInfoItem, Layout::Metadata::Element::Name, &m_bstrHoveredColumnName));
					CRect rect;
					ASSERT_IF_FAILED(pColumnsInfoItem->GetRect(&rect));
					rectPrevious = rect;
					rect.top += itemRect.top;
					rect.bottom += itemRect.top;
					if (rectPrevious != m_prevColumnRect || prevItemIndex != uiItem)
					{
						ASSERT_IF_FAILED(pColumnsInfoItem->SetVariantValue(Layout::Metadata::Element::Selected, &CComVar(true)));
						InvalidateRect(rect);
					}
				}
			}

			if (m_HoveredItemIndex == INVALID_ITEM_INDEX)
			{
				m_HoveredItemIndex = uiItem;
				m_bstrHoveredColumnName.Empty();
			}
		}
	}

	{ //Previous point
		if (m_PrevHoveredItemIndex != INVALID_ITEM_INDEX && (size_t)m_PrevHoveredItemIndex < m_columnsInfo.size() && (m_bstrPrevHoveredColumnName != m_bstrHoveredColumnName || rectPrevious != m_prevColumnRect || m_PrevHoveredItemIndex != m_HoveredItemIndex))
		{
			CComPtr<IColumnsInfo> pColumnsInfo = m_columnsInfo[m_PrevHoveredItemIndex];
			CComPtr<IColumnsInfoItem> pColumnsInfoItem;
			ASSERT_IF_FAILED(pColumnsInfo->FindItemByPoint(&CPoint(m_prevColumnRect.left, m_prevColumnRect.top), &pColumnsInfoItem));
			if (pColumnsInfoItem)
			{
				BOOL bIsUrl = FALSE;
				ASSERT_IF_FAILED(pColumnsInfoItem->GetRectBoolProp(Twitter::Metadata::Item::VAR_IS_URL, &bIsUrl));

				if (bIsUrl)
				{
					RECT itemRect = { 0 };
					GetItemRect(m_PrevHoveredItemIndex, &itemRect);
					ASSERT_IF_FAILED(pColumnsInfoItem->SetVariantValue(Layout::Metadata::Element::Selected, &CComVar(false)));
					CRect rect;
					ASSERT_IF_FAILED(pColumnsInfoItem->GetRect(&rect));
					rect.top += itemRect.top;
					rect.bottom += itemRect.top;
					InvalidateRect(rect);
				}
			}
		}
	}

	m_prevX = x;
	m_prevY = y;
	m_bstrPrevHoveredColumnName = m_bstrHoveredColumnName;
	m_prevColumnRect = rectPrevious;
	m_PrevHoveredItemIndex = m_HoveredItemIndex;
	return 0;
}

void CCustomListBox::PreTranslateMessage(HWND hWnd, MSG *pMsg, BOOL *pbResult)
{
	if (pMsg->hwnd == m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		BOOL bHandled = FALSE;
		OnKeyDown(pMsg->message, pMsg->wParam, pMsg->lParam, bHandled);
		*pbResult = bHandled;
	}
}

LRESULT CCustomListBox::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if (wParam == VK_RETURN)
	{
		auto curSel = GetCurSel();
		if (curSel >= 0)
		{
			auto keyControlState = GetKeyState(VK_CONTROL) & 0x8000;
			if (keyControlState == 0)
			{
				CComPtr<IColumnsInfo> pColumnsInfo = m_columnsInfo[curSel];
				CComPtr<IColumnsInfoItem> pColumnsInfoItem;
				ASSERT_IF_FAILED(pColumnsInfo->FindItemByName(Twitter::Connection::Metadata::TweetObject::Image, &pColumnsInfoItem));
				if (pColumnsInfoItem)
				{
					CComBSTR bstrMediaUrl;
					ASSERT_IF_FAILED(pColumnsInfoItem->GetRectStringProp(Twitter::Connection::Metadata::MediaObject::MediaUrl, &bstrMediaUrl));
					if (bstrMediaUrl != CComBSTR(L""))
					{
						CRect itemRect;
						GetItemRect(curSel, &itemRect);
						CRect columnRect;
						ASSERT_IF_FAILED(pColumnsInfoItem->GetRect(columnRect));
						HandleClick(MAKELONG(itemRect.left + columnRect.left + 1, itemRect.top + columnRect.top + 1), NM_LISTBOX_LCLICK);
						bHandled = TRUE;
					}
				}
			}
			else if (keyControlState > 0)
			{
				CRect itemRect;
				GetItemRect(curSel, &itemRect);
				HandleClick(MAKELONG(itemRect.left + 1, itemRect.top + 1), NM_LISTBOX_LDOUBLECLICK);
				bHandled = TRUE;
			}
		}
	}
	return 0;
}

LRESULT CCustomListBox::HandleClick(LPARAM lParam, UINT uiCode)
{
	if (!m_columnsInfo.size())
		return 0;
	auto x = GET_X_LPARAM(lParam);
	auto y = GET_Y_LPARAM(lParam);

	CPoint pt = { x, y };

	BOOL bOutside = FALSE;
	auto uiItem = ItemFromPoint(pt, bOutside);
	RECT itemRect = { 0 };
	GetItemRect(uiItem, &itemRect);
	if (bOutside || uiItem == (UINT)INVALID_ITEM_INDEX)
		return 0;

	if (uiCode == NM_LISTBOX_RCLICK)
	{
		SendMessage(WM_SETLISTBOX_SCROLL_MODE, SCROLL_OPTION_KEEP_SCROLLPOS);
		SetCurSel(uiItem);
		SendMessage(WM_SETLISTBOX_SCROLL_MODE, SCROLL_OPTION_NONE);
	}

	CComPtr<IColumnsInfo> pColumnsInfo = m_columnsInfo[uiItem];

	auto nID = GetDlgCtrlID();

	CComPtr<IVariantObject> pVariantObject;
	ASSERT_IF_FAILED(m_pItems->GetAt(uiItem, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));

	CComPtr<IColumnsInfoItem> pItem;
	{
		CPoint ptLocal(x, y);
		ptLocal.Offset(0, -itemRect.top);
		ASSERT_IF_FAILED(pColumnsInfo->FindItemByPoint(&ptLocal, &pItem));
	}

	ClientToScreen(&pt);

	NMCOLUMNCLICK nm = { 0 };
	nm.nmhdr.hwndFrom = m_hWnd;
	nm.nmhdr.idFrom = nID;
	nm.nmhdr.code = uiCode;
	nm.dwCurrentItem = uiItem;
	nm.pColumnsInfoItem = pItem.p;
	nm.pVariantObject = pVariantObject.p;
	nm.x = pt.x;
	nm.y = pt.y;

	::SendMessage(GetParent(), WM_NOTIFY, (WPARAM)nID, (LPARAM)&nm);
	return 0;
}

LRESULT CCustomListBox::OnLMouseDoubleClick(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return HandleClick(lParam, NM_LISTBOX_LDOUBLECLICK);
}

LRESULT CCustomListBox::OnLMouseButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return HandleClick(lParam, NM_LISTBOX_LCLICK);
}

LRESULT CCustomListBox::OnRMouseButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return HandleClick(lParam, NM_LISTBOX_RCLICK);
}

void CCustomListBox::BeginUpdate()
{
	if (!m_updateRefCount)
		SetRedraw(FALSE);
	++m_updateRefCount;
}

void CALLBACK TimerCallback3(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	::SendMessage(((CCustomListBox*)dwUser)->m_hWnd, WM_TIMER, 1, 0);
}

void CCustomListBox::EndUpdate()
{
	--m_updateRefCount;

	if (m_updateRefCount)
		return;

	SetRedraw();

	if (m_bAnimationNeeded)
	{
		StartFadeAnimation();
	}
	m_bAnimationNeeded = FALSE;
}

void CCustomListBox::StartFadeAnimation()
{
	m_bAnimating = TRUE;
	UINT uiInterval = 0;
	ASSERT_IF_FAILED(m_pSkinTimeline->AnimationGetParams(&uiInterval));
	m_animationTimerFade.StartAnimationTimer(uiInterval);
}

LRESULT CCustomListBox::OnAnimationTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (wParam == (WPARAM)&m_animationTimerFade  && IsWindow())
	{
		BOOL bContinueAnimation = FALSE;
		vector<IColumnsInfo*> v(m_columnsInfo.begin(), m_columnsInfo.end());
		if (v.size())
		{
			ASSERT_IF_FAILED(m_pSkinTimeline->AnimationNextFrame(&v[0], v.size(), &bContinueAnimation));
		}

		if (bContinueAnimation)
		{
			SetRedraw(FALSE);

			UINT uiCount = 0;
			ASSERT_IF_FAILED(m_pSkinTimeline->AnimationGetIndexes(NULL, &uiCount));
			vector<UINT> vIndexes(uiCount);
			ASSERT_IF_FAILED(m_pSkinTimeline->AnimationGetIndexes(&vIndexes[0], &uiCount));

			StartFadeAnimation();
			SetRedraw();

			if (vIndexes.size() && m_bEnableAnimation)
				Invalidate();

			return 0;
		}
		else if (m_bAnimating)
		{
			Invalidate();
		}
		m_bAnimating = FALSE;
	}
	return 0;
}

void CCustomListBox::RefreshItems(IVariantObject** pItemArray, UINT uiCountArray)
{
	for (size_t i = 0; i < uiCountArray; i++)
	{
		UINT uiIndex = 0;
		HRESULT hr = m_pItems->IndexOf(pItemArray[i], &uiIndex);
		if (SUCCEEDED(hr))
			RefreshItem(uiIndex);
		if (hr == E_NOT_SET)
			continue;
		ASSERT_IF_FAILED(hr);
	}
}

void CCustomListBox::InvalidateItems(IVariantObject** pItemArray, UINT uiCountArray)
{
	CRect rect;
	GetClientRect(&rect);

	RedrawWindow(FALSE);
	BOOL bNeedInvalidate = FALSE;
	for (size_t i = 0; i < uiCountArray; i++)
	{
		UINT uiIndex = 0;
		HRESULT hr = m_pItems->IndexOf(pItemArray[i], &uiIndex);

		if (SUCCEEDED(hr))
			RefreshItem(uiIndex);

		if (hr == E_NOT_SET)
			continue;

		ASSERT_IF_FAILED(hr);

		IVariantObject* pVariantObject = pItemArray[i];

		CRect rectItem;
		GetItemRect(uiIndex, &rectItem);
		CRect rectIntersect;
		BOOL bIntersects = rectIntersect.IntersectRect(rectItem, rect);
		UpdateAnimatedColumns(m_columnsInfo[uiIndex], uiIndex, pVariantObject, bIntersects);

		if (bIntersects)
			bNeedInvalidate = TRUE;
	}
	RedrawWindow();

	if (bNeedInvalidate && !m_bAnimating)
	{
		StartFadeAnimation();
	}
}

void CCustomListBox::SetSettings(ISettings* pSettings)
{
	ATLASSERT(pSettings);
	m_pSettings = pSettings;
	CComPtr<ISettings> pTimelineSettings;
	ASSERT_IF_FAILED(m_pSettings->OpenSubSettings(Twitter::Metadata::Settings::PathTimeline, &pTimelineSettings));
	CComVar vDisableAnimation;
	ASSERT_IF_FAILED(pTimelineSettings->GetVariantValue(Twitter::Metadata::Settings::Timeline::DisableAnimation, &vDisableAnimation));
	m_bEnableAnimation = (vDisableAnimation.vt != VT_I4 || (!vDisableAnimation.intVal));
}