#include "stdafx.h"
#include "CustomListBox.h"

LRESULT CCustomListBox::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
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
	if (lpdi->itemID == -1)
		return;

	m_pSkinTimeline->DrawItem(m_hWnd, m_items[lpdi->itemID].m_T, (TDRAWITEMSTRUCT*)lpdi, m_HoveredItemIndex, m_HoveredColumnIndex);
}

void CCustomListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	m_pSkinTimeline->MeasureItem(m_hWnd, m_items[lpMeasureItemStruct->itemID].m_T, (TMEASUREITEMSTRUCT*)lpMeasureItemStruct, m_columnRects[lpMeasureItemStruct->itemID].m_T);
}

void CCustomListBox::AddItem(IVariantObject* pItemObject)
{
	m_items.push_back(CAdapt<CComPtr<IVariantObject> >(pItemObject));
	CComPtr<IColumnRects> pColumnRects;
	HrCoCreateInstance(CLSID_ColumnRects, &pColumnRects);
	m_columnRects.push_back(pColumnRects);
	auto itemId = SendMessage(LB_ADDSTRING, 0, (LPARAM)1);
}

LRESULT CCustomListBox::OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	auto x = GET_X_LPARAM(lParam);
	auto y = GET_Y_LPARAM(lParam);

	auto prevItemIndex = m_HoveredItemIndex;
	auto prevColumnIndex = m_HoveredColumnIndex;

	m_HoveredItemIndex = 0xFFFF;
	m_HoveredColumnIndex = -1;

	{ //Current point
		BOOL bOutside = FALSE;
		auto uiItem = ItemFromPoint(CPoint(x, y), bOutside);
		RECT itemRect = { 0 };
		GetItemRect(uiItem, &itemRect);
		if (bOutside || uiItem == 0xFFFF)
			goto Exit;

		CComPtr<IColumnRects> pColumnRects = m_columnRects[uiItem];
		UINT uiCount = 0;
		pColumnRects->GetCount(&uiCount);
		for (size_t i = 0; i < uiCount; i++)
		{
			CRect rect;
			pColumnRects->GetRect(i, &rect);
			rect.top += itemRect.top;
			rect.bottom += itemRect.top;
			if (rect.PtInRect(CPoint(x, y)) && (prevColumnIndex != uiItem || prevItemIndex != i))
			{
				InvalidateRect(rect);
				m_HoveredItemIndex = uiItem;
				m_HoveredColumnIndex = i;
				break;
			}
		}
		if (m_HoveredItemIndex == 0xFFFF)
		{
			m_HoveredItemIndex = uiItem;
			m_HoveredColumnIndex = -1;
		}
	}

	{ //Previous point
		BOOL bOutside = FALSE;
		CPoint point(m_prevX, m_prevY);
		auto uiItem = ItemFromPoint(point, bOutside);
		RECT itemRect = { 0 };
		GetItemRect(uiItem, &itemRect);
		if (bOutside || uiItem == 0xFFFF)
			goto Exit;

		CComPtr<IColumnRects> pColumnRects = m_columnRects[uiItem];
		UINT uiCount = 0;
		pColumnRects->GetCount(&uiCount);
		for (size_t i = 0; i < uiCount; i++)
		{
			CRect rect;
			pColumnRects->GetRect(i, &rect);
			rect.top += itemRect.top;
			rect.bottom += itemRect.top;
			if (rect.PtInRect(point) && (i != m_HoveredColumnIndex || uiItem != m_HoveredItemIndex))
			{
				InvalidateRect(rect);
				break;
			}
		}
	}
Exit:
	m_prevX = x;
	m_prevY = y;
	return 0;
}
