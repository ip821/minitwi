#pragma once

#include <vector>
#include "atlctrls.h"

using namespace std;

template <class T>
class CStaticListBoxImpl : public CScrollImpl < T >
{
private:
	struct Item
	{
		int id;
		int height;
		bool focused;
	};

	vector<Item> m_items;
public:
	BEGIN_MSG_MAP(CStaticListBox<T>)
		MESSAGE_HANDLER(WM_PAINT, OnPaint);
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPrintClient);
		MESSAGE_HANDLER(WM_SIZE, OnSize);
		MESSAGE_HANDLER(LB_GETCOUNT, OnGetCount);
		MESSAGE_HANDLER(LB_INSERTSTRING, OnInsertString);
		MESSAGE_HANDLER(LB_DELETESTRING, OnDeleteString);
		MESSAGE_HANDLER(LB_RESETCONTENT, OnResetContent);
		MESSAGE_HANDLER(LB_SETITEMHEIGHT, OnSetItemHeight);
		MESSAGE_HANDLER(LB_ITEMFROMPOINT, OnItemFromPoint);
		MESSAGE_HANDLER(LB_GETTOPINDEX, OnGetTopIndex);
		MESSAGE_HANDLER(LB_SETTOPINDEX, OnSetTopIndex);
		MESSAGE_HANDLER(LB_GETCURSEL, OnGetCurSel);
		MESSAGE_HANDLER(LB_SETCURSEL, OnSetCurSel);
		MESSAGE_HANDLER(LB_GETITEMRECT, OnGetItemRect);
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLMouseButtonDown)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_GETDLGCODE, OnKeyDown)
			CHAIN_MSG_MAP(CScrollImpl<T>)
			CHAIN_MSG_MAP_ALT(CScrollImpl<T>, 1)
	END_MSG_MAP()

	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);

		UINT curSelIndex = pT->SendMessage(LB_GETCURSEL, 0, 0);

		switch (wParam)
		{
		case VK_DOWN:
			if (curSelIndex < m_items.size())
				pT->SendMessage(LB_SETCURSEL, curSelIndex + 1, 0);
			if (curSelIndex == LB_ERR)
				pT->SendMessage(LB_SETCURSEL, 0, 0);
			break;
		case VK_UP:
			if (curSelIndex > 0)
				pT->SendMessage(LB_SETCURSEL, curSelIndex - 1, 0);
			if (curSelIndex == LB_ERR)
				pT->SendMessage(LB_SETCURSEL, 0, 0);
			break;
		case VK_HOME:
			pT->SendMessage(WM_COMMAND, ID_SCROLL_TOP, 0);
			break;
		case VK_END:
			pT->SendMessage(WM_COMMAND, ID_SCROLL_BOTTOM, 0);
			break;
		case VK_PRIOR:
			pT->SendMessage(WM_COMMAND, ID_SCROLL_PAGE_UP, 0);
			break;
		case VK_NEXT:
			pT->SendMessage(WM_COMMAND, ID_SCROLL_PAGE_DOWN, 0);
			break;
		}
		return 0;
	}

	LRESULT OnLMouseButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);

		T* pT = static_cast<T*>(this);

		BOOL bOutside = FALSE;
		auto uiItem = pT->ItemFromPoint(CPoint(x, y), bOutside);
		if (bOutside || uiItem == (UINT)INVALID_ITEM_INDEX)
			return 0;

		pT->SetCurSel(uiItem);
		return 0;
	}

	LRESULT OnItemFromPoint(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);

		CRect rect;
		GetClientRect(pT->m_hWnd, &rect);

		CPoint pt(LOWORD(lParam), HIWORD(lParam));

		auto yTop = -m_ptOffset.y;
		for (size_t i = 0; i < m_items.size(); i++)
		{
			auto yBottom = yTop + m_items[i].height - 1;
			if (pt.x >= 0 && pt.x < rect.Width() && pt.y >= yTop && pt.y <= yBottom)
				return i;
			yTop = yBottom + 1;
		}
		return 0;
	}

	LRESULT OnGetTopIndex(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		BOOL bHandled = FALSE;
		return OnItemFromPoint(LB_ITEMFROMPOINT, 0, 0, bHandled);
	}

	LRESULT OnSetTopIndex(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);

		CRect rectClient;
		pT->GetClientRect(&rectClient);

		UINT uiCount = m_items.size();
		LONG lTop = rectClient.top;

		CRect rect;
		for (size_t i = 0; i < uiCount; i++)
		{
			CRect rectItem;
			rectItem.left = rect.left;
			rectItem.right = rect.right;
			rectItem.top = lTop;
			rectItem.bottom = rectItem.top + m_items[i].height;

			if (wParam == i)
			{
				SetScrollOffset(0, rectItem.top - m_ptOffset.y);
				break;
			}
		}

		return 0;
	}

	LRESULT OnGetCurSel(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		for (size_t i = 0; i < m_items.size(); i++)
		{
			if (m_items[i].focused)
				return i;
		}
		return LB_ERR;
	}

	LRESULT OnSetCurSel(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		for_each(m_items.begin(), m_items.end(), [&](Item& item){item.focused = false; });
		m_items[wParam].focused = true;
		T* pT = static_cast<T*>(this);
		pT->Invalidate();
		return 0;
	}

	LRESULT OnGetItemRect(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);

		CRect rect;
		GetClientRect(pT->m_hWnd, &rect);

		LPRECT lpResultRect = (LPRECT)lParam;

		CPoint pt(LOWORD(lParam), HIWORD(lParam));

		auto yTop = -m_ptOffset.y;
		for (size_t i = 0; i < m_items.size(); i++)
		{
			auto yBottom = yTop + m_items[i].height - 1;
			if (wParam == i)
			{
				rect.top = yTop;
				rect.bottom = yBottom;
				*lpResultRect = rect;
			}
			yTop = yBottom + 1;
		}
		return 0;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(pT->m_hWnd));
		pT->DoSize(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	LRESULT OnPrintClient(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		CRect rect;
		pT->GetClientRect(&rect);
		OnPaintInternal((HDC)wParam, rect);
		return 0;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		PAINTSTRUCT ps = { 0 };
		pT->BeginPaint(&ps);
		OnPaintInternal(ps.hdc, CRect(ps.rcPaint));
		pT->EndPaint(&ps);
		return 0;
	}

	void OnPaintInternal(HDC hdc, CRect& rectToPaint)
	{
		T* pT = static_cast<T*>(this);

		CRect rect;
		pT->GetClientRect(&rect);

		CDCHandle dc(hdc);

		CPoint ptViewportOrg;
		dc.SetViewportOrg(-m_ptOffset.x, -m_ptOffset.y, &ptViewportOrg);

		UINT uiCount = m_items.size();
		LONG lTop = rect.top;
		for (size_t i = 0; i < uiCount; i++)
		{
			CRect rectItem;
			rectItem.left = rect.left;
			rectItem.right = rect.right;
			rectItem.top = lTop;
			rectItem.bottom = rectItem.top + m_items[i].height;

			CRect rectItemReal = rectItem;
			rectItemReal.top -= m_ptOffset.y;
			rectItemReal.bottom -= m_ptOffset.y;

			CRect rectIntersect;
			if (rectIntersect.IntersectRect(&rectItemReal, &rectToPaint))
			{
				DRAWITEMSTRUCT di = { 0 };
				di.itemID = i;
				di.rcItem = rectItem;
				di.hDC = hdc;

				if (m_items[i].focused)
					di.itemState |= ODS_SELECTED;

				pT->DrawItem(&di);
			}
			lTop += rectItem.Height();
		}
		dc.SetViewportOrg(ptViewportOrg);
	}

	LRESULT OnGetCount(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		return m_items.size();
	}

	LRESULT OnDeleteString(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		m_items.erase(m_items.begin() + wParam);
		return 0;
	}

	LRESULT OnResetContent(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		m_items.clear();
		return 0;
	}

	LRESULT OnInsertString(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		Item item = { 0 };
		m_items.insert(m_items.begin() + wParam, item);

		T* pT = static_cast<T*>(this);

		CRect rect;
		pT->GetClientRect(&rect);

		MEASUREITEMSTRUCT mi = { 0 };
		mi.itemID = wParam;
		mi.itemWidth = rect.Width();
		pT->MeasureItem(&mi);

		m_items[wParam].height = mi.itemHeight;

		CPoint ptOffset;
		GetScrollOffset(ptOffset);

		LONG lTop = rect.top;
		for (size_t i = 0; i < m_items.size(); i++)
		{
			if (i == mi.itemID)
				break;

			lTop += m_items[i].height;
		}

		int yOffset = ptOffset.y;

		if (lTop < ptOffset.y)
			yOffset = ptOffset.y + m_items[wParam].height;

		UpdateScroll(yOffset);
		return 0;
	}

	void UpdateScroll(int yOffset = 0)
	{
		T* pT = static_cast<T*>(this);

		CRect rect;
		pT->GetClientRect(&rect);

		auto height = 0;
		for (size_t i = 0; i < m_items.size(); i++)
		{
			height += m_items[i].height;
		}

		SetScrollExtendedStyle(SCRL_SMOOTHSCROLL, SCRL_SMOOTHSCROLL);
		SetScrollSize(rect.Width(), height + 50, true, yOffset);
	}

	LRESULT OnSetItemHeight(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if (wParam >= m_items.size())
			return 0;

		T* pT = static_cast<T*>(this);

		CRect rectClient;
		pT->GetClientRect(&rectClient);

		LONG lTop = rectClient.top;
		for (size_t i = 0; i < m_items.size(); i++)
		{
			if (i == wParam)
				break;
			lTop += m_items[i].height;
		}

		CPoint ptOffset;
		GetScrollOffset(ptOffset);

		int yOffset = ptOffset.y;

		if (lTop < ptOffset.y)
			yOffset = ptOffset.y + (lParam - m_items[wParam].height);

		m_items[wParam].height = lParam;
		UpdateScroll(yOffset);
		return 0;
	}
};
