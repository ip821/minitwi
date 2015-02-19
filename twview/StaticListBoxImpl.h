#pragma once

#include <vector>
#include "atlctrls.h"

using namespace std;

#define SCROLL_OPTION_NONE 0
#define SCROLL_OPTION_KEEP_SCROLLPOS 1

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

	int m_scrollMode = SCROLL_OPTION_NONE;

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
		MESSAGE_HANDLER(WM_SETLISTBOX_SCROLL_MODE, OnSetScrollMode)
		CHAIN_MSG_MAP(CScrollImpl<T>)
		CHAIN_MSG_MAP_ALT(CScrollImpl<T>, 1)
	END_MSG_MAP()

	CStaticListBoxImpl()
	{
		m_nWheelLines = 1;
	}

	HWND GetHWND()
	{
		T* pT = static_cast<T*>(this);
		return pT->m_hWnd;
	}

	void GetRealRect(CRect& rectVirtual)
	{
		rectVirtual.top -= m_ptOffset.y;
		rectVirtual.bottom -= m_ptOffset.y;
	}

	void GetVirtualRect(int index, CRect& rectItem)
	{
		CRect rectClient;
		GetClientRect(GetHWND(), &rectClient);

		UINT uiCount = m_items.size();
		LONG lTop = rectClient.top;

		for (size_t i = 0; i < uiCount; i++)
		{
			rectItem.left = rectClient.left;
			rectItem.right = rectClient.right;
			rectItem.top = lTop;
			rectItem.bottom = rectItem.top + m_items[i].height - 1;

			if (index == (int)i)
				break;

			lTop += m_items[i].height;
		}
	}

	UINT ItemFromPoint(POINT pt, BOOL& bOutside)
	{
		ATLASSERT(::IsWindow(GetHWND()));
		DWORD dw = (DWORD)::SendMessage(GetHWND(), LB_ITEMFROMPOINT, 0, MAKELPARAM(pt.x, pt.y));
		bOutside = (BOOL)HIWORD(dw);
		return (UINT)LOWORD(dw);
	}

	void InvalidateItem(int index)
	{
		CRect rectItem;
		GetVirtualRect(index, rectItem);
		GetRealRect(rectItem);

		CRect rectClient;
		GetClientRect(GetHWND(), &rectClient);

		CRect rectIntersection;
		if (!rectIntersection.IntersectRect(&rectClient, &rectItem))
			return;

		InvalidateRect(GetHWND(), rectItem, TRUE);
	}

	LRESULT OnSetScrollMode(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		m_scrollMode = wParam;
		return 0;
	}

	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		UINT curSelIndex = SendMessage(GetHWND(), LB_GETCURSEL, 0, 0);

		switch (wParam)
		{
		case VK_DOWN:
			if (curSelIndex < m_items.size() - 1)
				SendMessage(GetHWND(), LB_SETCURSEL, curSelIndex + 1, SCROLL_OPTION_NONE);
			if (curSelIndex == LB_ERR)
			{
				auto topIndex = SendMessage(GetHWND(), LB_GETTOPINDEX, 0, 0);
				SendMessage(GetHWND(), LB_SETCURSEL, topIndex, SCROLL_OPTION_NONE);
			}
			break;
		case VK_UP:
			if (curSelIndex == LB_ERR)
				SendMessage(GetHWND(), LB_SETCURSEL, 0, SCROLL_OPTION_NONE);
			else if (curSelIndex > 0)
				SendMessage(GetHWND(), LB_SETCURSEL, curSelIndex - 1, SCROLL_OPTION_NONE);
			break;
		case VK_HOME:
			SendMessage(GetHWND(), LB_SETCURSEL, 0, SCROLL_OPTION_NONE);
			break;
		case VK_END:
			SendMessage(GetHWND(), LB_SETCURSEL, m_items.size() - 1, SCROLL_OPTION_NONE);
			break;
		case VK_PRIOR:
			SendMessage(GetHWND(), WM_COMMAND, ID_SCROLL_PAGE_UP, 0);
			break;
		case VK_NEXT:
			SendMessage(GetHWND(), WM_COMMAND, ID_SCROLL_PAGE_DOWN, 0);
			break;
		}
		return 0;
	}

	LRESULT OnLMouseButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);

		BOOL bOutside = FALSE;
		auto uiItem = ItemFromPoint(CPoint(x, y), bOutside);
		if (bOutside || uiItem == (UINT)INVALID_ITEM_INDEX)
			return 0;

		SendMessage(GetHWND(), LB_SETCURSEL, uiItem, SCROLL_OPTION_KEEP_SCROLLPOS);
		return 0;
	}

	LRESULT OnItemFromPoint(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		CRect rect;
		GetClientRect(GetHWND(), &rect);

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
		return SendMessage(GetHWND(), LB_ITEMFROMPOINT, 0, 0);
	}

	LRESULT OnSetTopIndex(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		CRect rectItem;
		GetVirtualRect(wParam, rectItem);
		SetScrollOffset(0, rectItem.top - m_ptOffset.y);
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
		auto curSelIndex = SendMessage(GetHWND(), LB_GETCURSEL, 0, 0);

		if ((WPARAM)curSelIndex == wParam)
			return 0;

		for_each(m_items.begin(), m_items.end(), [&](Item& item){item.focused = false; });

		if (wParam == (WPARAM)LB_ERR)
		{
			if (curSelIndex != LB_ERR)
				InvalidateItem(curSelIndex);
			return 0;
		}

		m_items[wParam].focused = true;

		CRect rectItem;
		GetVirtualRect(wParam, rectItem);
		GetRealRect(rectItem);

		CRect rectClient;
		GetClientRect(GetHWND(), &rectClient);

		CRect rectIntersect;
		rectIntersect.IntersectRect(&rectClient, &rectItem);

		if (lParam == SCROLL_OPTION_NONE && m_scrollMode == SCROLL_OPTION_NONE)
		{
			if (rectIntersect.Height() < rectItem.Height() && !rectIntersect.IsRectEmpty())
			{
				if (curSelIndex != (WPARAM)LB_ERR && wParam < (WPARAM)curSelIndex)
				{
					CPoint ptOffset;
					GetScrollOffset(ptOffset);
					GetVirtualRect(wParam, rectItem);
					SetScrollOffset(ptOffset.x, rectItem.top);
					return 0;
				}

				if (curSelIndex != (WPARAM)LB_ERR && wParam > (WPARAM)curSelIndex)
				{
					CPoint ptOffset;
					GetScrollOffset(ptOffset);
					GetVirtualRect(wParam, rectItem);
					SetScrollOffset(ptOffset.x, ptOffset.y + (rectItem.Height() - rectIntersect.Height()) + 2);
					return 0;
				}
			}

			if (curSelIndex != (WPARAM)LB_ERR && wParam > (WPARAM)curSelIndex && rectIntersect.Height() < rectItem.Height() && wParam - curSelIndex == 1)
			{
				CRect rectCurSel;
				GetVirtualRect(curSelIndex, rectCurSel);
				GetRealRect(rectCurSel);

				CRect rectCurSelIntersect;
				rectCurSelIntersect.IntersectRect(&rectCurSel, &rectClient);

				if (rectCurSelIntersect.Height() < m_items[curSelIndex].height && !rectCurSelIntersect.IsRectEmpty())
				{
					GetVirtualRect(curSelIndex, rectCurSel);
					CPoint ptOffset;
					GetScrollOffset(ptOffset);
					GetVirtualRect(wParam, rectItem);
					SetScrollOffset(ptOffset.x, ptOffset.y + rectItem.Height() + (rectCurSel.Height() - rectCurSelIntersect.Height()) + 2);
					return 0;
				}
			}

			if (rectIntersect.IsRectEmpty())
			{
				CPoint ptOffset;
				GetScrollOffset(ptOffset);
				GetVirtualRect(wParam, rectItem);
				SetScrollOffset(ptOffset.x, rectItem.top);
				return 0;
			}
		}

		if (curSelIndex != LB_ERR)
			InvalidateItem(curSelIndex);

		InvalidateItem(wParam);

		return 0;
	}

	LRESULT OnGetItemRect(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LPRECT lpResultRect = (LPRECT)lParam;
		CRect rect = *lpResultRect;
		GetVirtualRect(wParam, rect);
		GetRealRect(rect);
		*lpResultRect = rect;
		return 0;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(GetHWND()));
		pT->DoSize(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	LRESULT OnPrintClient(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		CRect rect;
		GetClientRect(GetHWND(), &rect);
		OnPaintInternal((HDC)wParam, rect);
		return 0;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		PAINTSTRUCT ps = { 0 };
		BeginPaint(GetHWND(), &ps);
		OnPaintInternal(ps.hdc, CRect(ps.rcPaint));
		EndPaint(GetHWND(), &ps);
		return 0;
	}

	void OnPaintInternal(HDC hdc, CRect& rectToPaint)
	{
		T* pT = static_cast<T*>(this);

		CRect rect;
		GetClientRect(GetHWND(), &rect);

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
		GetClientRect(GetHWND(), &rect);

		MEASUREITEMSTRUCT mi = { 0 };
		mi.itemID = wParam;
		mi.itemWidth = rect.Width();
		pT->MeasureItem(&mi);

		m_items[wParam].height = mi.itemHeight;

		CPoint ptOffset;
		GetScrollOffset(ptOffset);

		CRect rectItemVirtual;
		GetVirtualRect(mi.itemID, rectItemVirtual);

		int yOffset = ptOffset.y;

		if (rectItemVirtual.top < ptOffset.y || m_scrollMode == SCROLL_OPTION_KEEP_SCROLLPOS)
			yOffset = ptOffset.y + m_items[wParam].height;

		UpdateScroll(yOffset);
		return 0;
	}

	void UpdateScroll(int yOffset = 0)
	{
		CRect rect;
		GetClientRect(GetHWND(), &rect);

		auto height = 0;
		for (size_t i = 0; i < m_items.size(); i++)
		{
			height += m_items[i].height;
		}

		SetScrollExtendedStyle(SCRL_SMOOTHSCROLL, SCRL_SMOOTHSCROLL);
		SetScrollSize(1, height + 50, true, yOffset);
		SetScrollLine(1, 40);
	}

	LRESULT OnSetItemHeight(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if (wParam >= m_items.size())
			return 0;

		CRect rectClient;
		GetClientRect(GetHWND(), &rectClient);

		CRect rectItemVirtual;
		GetVirtualRect(wParam, rectItemVirtual);

		CPoint ptOffset;
		GetScrollOffset(ptOffset);

		int yOffset = ptOffset.y;

		if (rectItemVirtual.top < ptOffset.y || m_scrollMode == SCROLL_OPTION_KEEP_SCROLLPOS)
			yOffset = ptOffset.y + (lParam - m_items[wParam].height);

		m_items[wParam].height = lParam;
		UpdateScroll(yOffset);
		return 0;
	}
};
