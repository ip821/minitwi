#include "stdafx.h"
#include "CustomListBox.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"

#define TARGET_RESOLUTION 1

CCustomListBox::CCustomListBox()
{
	m_HoveredItemIndex = INVALID_ITEM_INDEX;
	m_HoveredColumnIndex = INVALID_COLUMN_INDEX;
	m_handCursor.LoadSysCursor(IDC_HAND);
	m_arrowCursor.LoadSysCursor(IDC_ARROW);
	HrCoCreateInstance(CLSID_ObjectCollection, &m_pItems);
}

LRESULT CCustomListBox::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

void CCustomListBox::Clear()
{
	auto nID = GetDlgCtrlID();
	UINT uiCount = 0;
	m_pItems->GetCount(&uiCount);
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pVariantObject;
		m_pItems->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pVariantObject);

		NMITEMREMOVED nm = { 0 };
		nm.nmhdr.hwndFrom = m_hWnd;
		nm.nmhdr.idFrom = nID;
		nm.nmhdr.code = NM_ITEM_REMOVED;
		nm.pVariantObject = pVariantObject.p;
		nm.pColumnRects = m_columnRects[i].m_T;
		::SendMessage(GetParent(), WM_NOTIFY, (WPARAM)nID, (LPARAM)&nm);
	}
	ResetContent();
	m_pItems->Clear();
	m_itemsToIndex.clear();
	m_columnRects.clear();
	m_animatedColumns.clear();
}

void CCustomListBox::RemoveItemByIndex(UINT uiIndex)
{
	auto nID = GetDlgCtrlID();

	DeleteString(uiIndex);

	CComPtr<IVariantObject> pVariantObject;
	m_pItems->GetAt(uiIndex, __uuidof(IVariantObject), (LPVOID*)&pVariantObject);

	NMITEMREMOVED nm = { 0 };
	nm.nmhdr.hwndFrom = m_hWnd;
	nm.nmhdr.idFrom = nID;
	nm.nmhdr.code = NM_ITEM_REMOVED;
	nm.pVariantObject = pVariantObject.p;
	nm.pColumnRects = m_columnRects[uiIndex].m_T;
	::SendMessage(GetParent(), WM_NOTIFY, (WPARAM)nID, (LPARAM)&nm);

	m_pItems->RemoveObjectAt(uiIndex);
	m_columnRects.erase(m_columnRects.begin() + uiIndex);
	m_itemsToIndex.erase(pVariantObject.p);

	CComVariant vId;
	pVariantObject->GetVariantValue(VAR_ID, &vId);
	m_animatedColumns.erase(pVariantObject.p);
}

void CCustomListBox::RefreshItem(UINT uiIndex)
{
	MEASUREITEMSTRUCT s = { 0 };
	s.itemID = uiIndex;
	MeasureItem(&s);
}

LRESULT CCustomListBox::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (!m_hWnd)
		return 0;

	auto itemCount = GetCount();
	for (int i = 0; i < itemCount; i++)
	{
		MEASUREITEMSTRUCT s = { 0 };
		s.itemID = i;
		MeasureItem(&s);
		SetItemHeight(i, s.itemHeight);
	}
	return 0;
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
	m_pItems->GetAt(lpdi->itemID, __uuidof(IVariantObject), (LPVOID*)&pVariantObject);
	auto& columns = m_animatedColumns[pVariantObject.p];
	std::vector<UINT> vIndexes(columns.begin(), columns.end());
	UINT* pui = nullptr;
	if (vIndexes.size())
		pui = &vIndexes[0];
	TDRAWITEMSTRUCTTIMELINE distl = { 0 };
	distl.lpdi = reinterpret_cast<TDRAWITEMSTRUCT*>(lpdi);
	distl.iHoveredItem = m_HoveredItemIndex;
	distl.iHoveredColumn = m_HoveredColumnIndex;
	distl.puiNotAnimatedColumnIndexes = pui;
	distl.uiNotAnimatedColumnIndexesCount = vIndexes.size();
	m_pSkinTimeline->DrawItem(m_hWnd, m_columnRects[lpdi->itemID].m_T, &distl);
}

void CCustomListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CComPtr<IVariantObject> pVariantObject;
	m_pItems->GetAt(lpMeasureItemStruct->itemID, __uuidof(IVariantObject), (LPVOID*)&pVariantObject);

	m_pSkinTimeline->MeasureItem(m_hWnd, pVariantObject.p, (TMEASUREITEMSTRUCT*)lpMeasureItemStruct, m_columnRects[lpMeasureItemStruct->itemID].m_T);
}

void CCustomListBox::IsEmpty(BOOL* pbEmpty)
{
	UINT uiCount = 0;
	m_pItems->GetCount(&uiCount);
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
	m_pItems->GetCount(&uiCount);
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pVariantObject;
		m_pItems->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pVariantObject);

		CComVariant v;
		pVariantObject->GetVariantValue(VAR_TWITTER_RELATIVE_TIME, &v);
		if (v.vt == VT_BSTR)
		{
			UINT uiColumnCount = 0;
			m_columnRects[i]->GetCount(&uiColumnCount);
			for (size_t j = 0; j < uiColumnCount; j++)
			{
				CComBSTR bstrColumnName;
				m_columnRects[i]->GetRectStringProp(j, VAR_COLUMN_NAME, &bstrColumnName);
				if (bstrColumnName == CComBSTR(VAR_TWITTER_RELATIVE_TIME))
				{
					m_columnRects[i]->SetRectStringProp(j, VAR_TEXT, v.bstrVal);
					m_columnRects[i]->SetRectStringProp(j, VAR_VALUE, v.bstrVal);
				}
			}
		}
	}
}

void CCustomListBox::InsertItem(IVariantObject* pItemObject, int index)
{
	CComVariant vId;
	pItemObject->GetVariantValue(VAR_ID, &vId);

	m_pItems->InsertObject(pItemObject, index);
	m_itemsToIndex[pItemObject] = index;
	CComPtr<IColumnRects> pColumnRects;
	HrCoCreateInstance(CLSID_ColumnRects, &pColumnRects);
	m_columnRects.insert(m_columnRects.begin() + index, pColumnRects);
	SendMessage(LB_INSERTSTRING, index, (LPARAM)1);
	
	if (m_bEnableAnimation)
		m_pSkinTimeline->AnimationRegisterItemIndex(index, pColumnRects, INVALID_COLUMN_INDEX);
	UpdateAnimatedColumns(pColumnRects, index, pItemObject, TRUE);
	m_bAnimationNeeded = TRUE;
}

void CCustomListBox::UpdateAnimatedColumns(IColumnRects* pColumnRects, int itemIndex, IVariantObject* pVariantObject, BOOL bRegisterForAnimation)
{
	CComPtr<IImageManagerService> pImageManagerService;
	m_pSkinTimeline->GetImageManagerService(&pImageManagerService);

	UINT uiCount = 0;
	pColumnRects->GetCount(&uiCount);
	for (size_t i = 0; i < uiCount; i++)
	{
		BOOL bImage = FALSE;
		pColumnRects->GetRectBoolProp(i, VAR_IS_IMAGE, &bImage);
		if (!bImage)
			continue;

		CComBSTR bstrValue;
		pColumnRects->GetRectStringProp(i, VAR_VALUE, &bstrValue);

		BOOL bContains = FALSE;
		pImageManagerService->ContainsImageKey(bstrValue, &bContains);

		if (!bContains)
			continue;

		if (m_animatedColumns[pVariantObject].find(i) != m_animatedColumns[pVariantObject].end())
			continue;

		m_animatedColumns[pVariantObject].insert(i);
		if (bRegisterForAnimation && m_bEnableAnimation)
			m_pSkinTimeline->AnimationRegisterItemIndex(itemIndex, pColumnRects, i);
	}
}

LRESULT CCustomListBox::OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	auto x = GET_X_LPARAM(lParam);
	auto y = GET_Y_LPARAM(lParam);

	auto prevItemIndex = m_HoveredItemIndex;
	auto prevColumnIndex = m_HoveredColumnIndex;

	SetCursor(m_arrowCursor);

	m_HoveredItemIndex = INVALID_ITEM_INDEX;
	m_HoveredColumnIndex = INVALID_COLUMN_INDEX;

	if (m_columnRects.empty())
		goto Exit;

	{ //Current point
		BOOL bOutside = FALSE;
		int uiItem = ItemFromPoint(CPoint(x, y), bOutside);
		RECT itemRect = { 0 };
		GetItemRect(uiItem, &itemRect);
		if (bOutside || uiItem == INVALID_ITEM_INDEX)
			goto Exit;

		CComPtr<IColumnRects> pColumnRects = m_columnRects[uiItem];
		UINT uiCount = 0;
		pColumnRects->GetCount(&uiCount);
		for (int i = 0; i < (int)uiCount; i++)
		{
			CRect rect;
			pColumnRects->GetRect(i, &rect);
			rect.top += itemRect.top;
			rect.bottom += itemRect.top;

			if (rect.PtInRect(CPoint(x, y)))
			{
				BOOL bIsUrl = FALSE;
				pColumnRects->GetRectBoolProp(i, VAR_IS_URL, &bIsUrl);

				if (bIsUrl)
				{
					SetCursor(m_handCursor);
					m_HoveredItemIndex = uiItem;
					m_HoveredColumnIndex = i;
					if (prevColumnIndex != uiItem || prevItemIndex != i)
					{
						InvalidateRect(rect);
					}
				}
				break;
			}
		}
		if (m_HoveredItemIndex == INVALID_ITEM_INDEX)
		{
			m_HoveredItemIndex = uiItem;
			m_HoveredColumnIndex = INVALID_COLUMN_INDEX;
		}
	}

	{ //Previous point
		BOOL bOutside = FALSE;
		CPoint point(m_prevX, m_prevY);
		int uiItem = ItemFromPoint(point, bOutside);
		RECT itemRect = { 0 };
		GetItemRect(uiItem, &itemRect);
		if (bOutside || uiItem == INVALID_ITEM_INDEX)
			goto Exit;

		CComPtr<IColumnRects> pColumnRects = m_columnRects[uiItem];
		UINT uiCount = 0;
		pColumnRects->GetCount(&uiCount);
		for (int i = 0; i < (int)uiCount; i++)
		{
			CRect rect;
			pColumnRects->GetRect(i, &rect);
			rect.top += itemRect.top;
			rect.bottom += itemRect.top;

			if (rect.PtInRect(point) && (i != m_HoveredColumnIndex || uiItem != m_HoveredItemIndex))
			{
				BOOL bIsUrl = FALSE;
				pColumnRects->GetRectBoolProp(i, VAR_IS_URL, &bIsUrl);

				if (bIsUrl)
				{
					InvalidateRect(rect);
				}
				break;
			}
		}
	}
Exit:
	m_prevX = x;
	m_prevY = y;
	return 0;
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
				CComPtr<IColumnRects> pColumnRects = m_columnRects[curSel];
				UINT uiCount = 0;
				pColumnRects->GetCount(&uiCount);
				for (size_t i = 0; i < uiCount; i++)
				{
					CComBSTR bstrColumnName;
					pColumnRects->GetRectStringProp(i, VAR_COLUMN_NAME, &bstrColumnName);
					CComBSTR bstrMediaUrl;
					pColumnRects->GetRectStringProp(i, VAR_TWITTER_MEDIAURL, &bstrMediaUrl);
					if (bstrColumnName == CComBSTR(VAR_TWITTER_IMAGE) && bstrMediaUrl != CComBSTR(L""))
					{
						CRect itemRect;
						GetItemRect(curSel, &itemRect);
						CRect columnRect;
						pColumnRects->GetRect(i, columnRect);
						HandleCLick(MAKELONG(itemRect.left + columnRect.left + 1, itemRect.top + columnRect.top + 1), NM_LISTBOX_LCLICK);
						bHandled = TRUE;
						break;
					}
				}
			}
			else if (keyControlState > 0)
			{
				CRect itemRect;
				GetItemRect(curSel, &itemRect);
				HandleCLick(MAKELONG(itemRect.left + 1, itemRect.top + 1), NM_LISTBOX_LDOUBLECLICK);
				bHandled = TRUE;
			}
		}
	}
	return 0;
}

LRESULT CCustomListBox::HandleCLick(LPARAM lParam, UINT uiCode)
{
	if (!m_columnRects.size())
		return 0;
	auto x = GET_X_LPARAM(lParam);
	auto y = GET_Y_LPARAM(lParam);

	BOOL bOutside = FALSE;
	auto uiItem = ItemFromPoint(CPoint(x, y), bOutside);
	RECT itemRect = { 0 };
	GetItemRect(uiItem, &itemRect);
	if (bOutside || uiItem == (UINT)INVALID_ITEM_INDEX)
		return 0;

	if (uiCode == NM_LISTBOX_RCLICK)
		SetCurSel(uiItem);

	CComPtr<IColumnRects> pColumnRects = m_columnRects[uiItem];

	auto nID = GetDlgCtrlID();

	CPoint pt = { x, y };
	ClientToScreen(&pt);

	CComPtr<IVariantObject> pVariantObject;
	m_pItems->GetAt(uiItem, __uuidof(IVariantObject), (LPVOID*)&pVariantObject);

	NMCOLUMNCLICK nm = { 0 };
	nm.nmhdr.hwndFrom = m_hWnd;
	nm.nmhdr.idFrom = nID;
	nm.nmhdr.code = uiCode;
	nm.dwCurrentItem = uiItem;
	nm.pColumnRects = pColumnRects;
	nm.pVariantObject = pVariantObject.p;
	nm.dwCurrentColumn = INVALID_COLUMN_INDEX;
	nm.x = pt.x;
	nm.y = pt.y;

	UINT uiCount = 0;
	pColumnRects->GetCount(&uiCount);

	for (size_t i = 0; i < uiCount; i++)
	{
		CRect rect;
		pColumnRects->GetRect(i, &rect);
		rect.top += itemRect.top;
		rect.bottom += itemRect.top;

		if (rect.PtInRect(CPoint(x, y)))
		{
			nm.dwCurrentColumn = i;
			break;
		}
	}
	::SendMessage(GetParent(), WM_NOTIFY, (WPARAM)nID, (LPARAM)&nm);
	return 0;
}

LRESULT CCustomListBox::OnLMouseDoubleClick(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return HandleCLick(lParam, NM_LISTBOX_LDOUBLECLICK);
}

LRESULT CCustomListBox::OnLMouseButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return HandleCLick(lParam, NM_LISTBOX_LCLICK);
}

LRESULT CCustomListBox::OnRMouseButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return HandleCLick(lParam, NM_LISTBOX_RCLICK);
}

void CCustomListBox::BeginUpdate()
{
	SetRedraw(FALSE);
}

void CALLBACK TimerCallback3(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	::SendMessage(((CCustomListBox*)dwUser)->m_hWnd, WM_TIMER, 1, 0);
}

void CCustomListBox::EndUpdate()
{
	if (m_bAnimationNeeded)
	{
		if (m_bEnableAnimation)
			StartAnimation();
		else
			Invalidate(TRUE);
	}
	m_bAnimationNeeded = FALSE;
	SetRedraw();
}

void CCustomListBox::StartAnimation()
{
	m_bAnimating = TRUE;
	UINT uiInterval = 0;
	m_pSkinTimeline->AnimationGetParams(&uiInterval);
	StartAnimationTimer(uiInterval);
}

LRESULT CCustomListBox::OnAnimationTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	BOOL bContinueAnimation = FALSE;
	m_pSkinTimeline->AnimationNextFrame(&bContinueAnimation);

	if (bContinueAnimation)
	{
		SetRedraw(FALSE);

		UINT uiCount = 0;
		m_pSkinTimeline->AnimationGetIndexes(NULL, &uiCount);
		std::vector<UINT> vIndexes(uiCount);
		m_pSkinTimeline->AnimationGetIndexes(&vIndexes[0], &uiCount);

		if (vIndexes.size())
			Invalidate();

		StartAnimation();
		SetRedraw();
		return 0;
	}
	m_bAnimating = FALSE;
	return 0;
}

void CCustomListBox::RefreshItems(IVariantObject** pItemArray, UINT uiCountArray)
{
	for (size_t i = 0; i < uiCountArray; i++)
	{
		auto it = m_itemsToIndex.find(pItemArray[i]);
		if (it == m_itemsToIndex.end())
			continue;
		RefreshItem(it->second);
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
		auto it = m_itemsToIndex.find(pItemArray[i]);
		if (it == m_itemsToIndex.end())
			continue;

		IVariantObject* pVariantObject = pItemArray[i];

		CRect rectItem;
		GetItemRect(it->second, &rectItem);
		CRect rectIntersect;
		BOOL bIntersects = rectIntersect.IntersectRect(rectItem, rect);
		UpdateAnimatedColumns(m_columnRects[it->second].m_T, it->second, pVariantObject, bIntersects);
		if (bIntersects)
			OutputDebugString(CString(L"Schedule update due to item index: ") + boost::lexical_cast<std::wstring>(i).c_str() + CString(L"\n"));
	
		if (bIntersects)
			bNeedInvalidate = TRUE;
	}
	RedrawWindow();

	if (bNeedInvalidate && !m_bAnimating)
	{
		if (m_bEnableAnimation)
			StartAnimation();
		else
			Invalidate(TRUE);
	}
}

void CCustomListBox::EnableAnimation(BOOL bEnable)
{
	m_bEnableAnimation = bEnable;
}
