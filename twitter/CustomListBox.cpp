#include "stdafx.h"
#include "CustomListBox.h"
#include "Plugins.h"

CCustomListBox::CCustomListBox()
{
	m_HoveredItemIndex = -1;
	m_HoveredColumnIndex = -1;
	m_handCursor.LoadSysCursor(IDC_HAND);
	m_arrowCursor.LoadSysCursor(IDC_ARROW);
}

LRESULT CCustomListBox::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

void CCustomListBox::Clear()
{
	auto nID = GetDlgCtrlID();

	for (size_t i = 0; i < m_items.size(); i++)
	{
		NMITEMREMOVED nm = { 0 };
		nm.nmhdr.hwndFrom = m_hWnd;
		nm.nmhdr.idFrom = nID;
		nm.nmhdr.code = NM_ITEM_REMOVED;
		nm.pVariantObject = m_items[i].m_T;
		nm.pColumnRects = m_columnRects[i].m_T;
		::SendMessage(GetParent(), WM_NOTIFY, (WPARAM)nID, (LPARAM)&nm);
	}
	ResetContent();
	m_items.clear();
	m_columnRects.clear();
}

void CCustomListBox::RemoveItemByIndex(UINT uiIndex)
{
	auto nID = GetDlgCtrlID();

	DeleteString(uiIndex);

	NMITEMREMOVED nm = { 0 };
	nm.nmhdr.hwndFrom = m_hWnd;
	nm.nmhdr.idFrom = nID;
	nm.nmhdr.code = NM_ITEM_REMOVED;
	nm.pVariantObject = m_items[uiIndex].m_T;
	nm.pColumnRects = m_columnRects[uiIndex].m_T;
	::SendMessage(GetParent(), WM_NOTIFY, (WPARAM)nID, (LPARAM)&nm);

	m_items.erase(m_items.begin() + uiIndex);
	m_columnRects.erase(m_columnRects.begin() + uiIndex);
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

	m_pSkinTimeline->DrawItem(m_hWnd, m_columnRects[lpdi->itemID].m_T, (TDRAWITEMSTRUCT*)lpdi, m_HoveredItemIndex, m_HoveredColumnIndex);
}

void CCustomListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	m_pSkinTimeline->MeasureItem(m_hWnd, m_items[lpMeasureItemStruct->itemID].m_T, (TMEASUREITEMSTRUCT*)lpMeasureItemStruct, m_columnRects[lpMeasureItemStruct->itemID].m_T);
}

void CCustomListBox::IsEmpty(BOOL* pbEmpty)
{
	*pbEmpty = m_items.empty();
}

HRESULT CCustomListBox::GetItems(IObjArray** ppObjectArray)
{
	CComPtr<IObjCollection> pObjCollection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pObjCollection));
	for (size_t i = 0; i < m_items.size(); i++)
	{
		RETURN_IF_FAILED(pObjCollection->AddObject(m_items[i].m_T));
	}
	RETURN_IF_FAILED(pObjCollection->QueryInterface(ppObjectArray));
	return S_OK;
}

void CCustomListBox::OnItemsUpdated()
{
	for (size_t i = 0; i < m_items.size(); i++)
	{
		CComVariant v;
		m_items[i].m_T->GetVariantValue(VAR_TWITTER_RELATIVE_TIME, &v);
		if (v.vt == VT_BSTR)
		{
			UINT uiColumnCount = 0;
			m_columnRects[i]->GetCount(&uiColumnCount);
			for (size_t j = 0; j < uiColumnCount; j++)
			{
				CComBSTR bstrColumnName;
				m_columnRects[i]->GetRectProp(j, VAR_COLUMN_NAME, &bstrColumnName);
				if (bstrColumnName == CComBSTR(VAR_TWITTER_RELATIVE_TIME))
				{
					m_columnRects[i]->SetRectProp(j, VAR_TEXT, v.bstrVal);
					m_columnRects[i]->SetRectProp(j, VAR_VALUE, v.bstrVal);
				}
			}
		}
	}
}

void CCustomListBox::InsertItem(IVariantObject* pItemObject, int index)
{
	CComVariant vId;
	pItemObject->GetVariantValue(VAR_ID, &vId);

	if (vId.vt == VT_BSTR)
	{
		auto it = std::find_if(
			m_items.cbegin(),
			m_items.cend(),
			[&](CAdapt<CComPtr<IVariantObject> > p)
		{
			CComVariant v;
			p->GetVariantValue(VAR_ID, &v);
			if (v.vt == VT_BSTR && CComBSTR(v.bstrVal) == CComBSTR(vId.bstrVal))
				return true;
			return false;
		}
		);

		if (it != m_items.cend())
			return;
	}

	m_items.insert(m_items.begin() + index, CAdapt<CComPtr<IVariantObject> >(pItemObject));
	CComPtr<IColumnRects> pColumnRects;
	HrCoCreateInstance(CLSID_ColumnRects, &pColumnRects);
	m_columnRects.insert(m_columnRects.begin() + index, pColumnRects);
	auto itemId = SendMessage(LB_INSERTSTRING, index, (LPARAM)1);
}

LRESULT CCustomListBox::OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	auto x = GET_X_LPARAM(lParam);
	auto y = GET_Y_LPARAM(lParam);

	auto prevItemIndex = m_HoveredItemIndex;
	auto prevColumnIndex = m_HoveredColumnIndex;

	SetCursor(m_arrowCursor);

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

			if (rect.PtInRect(CPoint(x, y)))
			{
				CComBSTR bstrIsUrl;
				pColumnRects->GetRectProp(i, VAR_IS_URL, &bstrIsUrl);

				if (bstrIsUrl == L"1")
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
				CComBSTR bstrIsUrl;
				pColumnRects->GetRectProp(i, VAR_IS_URL, &bstrIsUrl);

				if (bstrIsUrl == L"1")
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

LRESULT CCustomListBox::HandleCLick(LPARAM lParam, UINT uiCode)
{
	auto x = GET_X_LPARAM(lParam);
	auto y = GET_Y_LPARAM(lParam);

	BOOL bOutside = FALSE;
	auto uiItem = ItemFromPoint(CPoint(x, y), bOutside);
	RECT itemRect = { 0 };
	GetItemRect(uiItem, &itemRect);
	if (bOutside || uiItem == 0xFFFF)
		return 0;

	if (uiCode == NM_LISTBOX_RCLICK)
		SetCurSel(uiItem);

	CComPtr<IColumnRects> pColumnRects = m_columnRects[uiItem];

	auto nID = GetDlgCtrlID();

	CPoint pt = { x, y };
	ClientToScreen(&pt);

	NMCOLUMNCLICK nm = { 0 };
	nm.nmhdr.hwndFrom = m_hWnd;
	nm.nmhdr.idFrom = nID;
	nm.nmhdr.code = uiCode;
	nm.dwCurrentItem = uiItem;
	nm.pColumnRects = pColumnRects;
	nm.pVariantObject = m_items[uiItem].m_T;
	nm.dwCurrentColumn = -1;
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