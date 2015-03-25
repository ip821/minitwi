#pragma once

#include <vector>
#include "atlctrls.h"
#include "../twtheme/GdilPlusUtils.h"

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
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground);
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
			MESSAGE_HANDLER(WM_GETDLGCODE, OnGetDlgCode)
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

	LRESULT OnGetDlgCode(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
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
		}
		
		return 0;
	}

	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		switch (wParam)
		{
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
		SetScrollOffset(0, rectItem.top);
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

		//if ((WPARAM)curSelIndex == wParam)
		//	return 0;

		for_each(m_items.begin(), m_items.end(), [&](Item& item){item.focused = false; });

		if (wParam == (WPARAM)LB_ERR)
		{
			if (curSelIndex != LB_ERR)
				InvalidateItem(curSelIndex);
			return 0;
		}

		m_items[wParam].focused = true;

		if (lParam == SCROLL_OPTION_NONE && m_scrollMode == SCROLL_OPTION_NONE)
		{
			if (curSelIndex == LB_ERR)
			{
				::SendMessage(GetHWND(), LB_SETTOPINDEX, wParam, 0);
				return 0;
			}

			CPoint ptOffset;
			GetScrollOffset(ptOffset);

			CRect rectItemNext;
			GetVirtualRect(wParam, rectItemNext);
			CRect rectItemNextReal = rectItemNext;
			GetRealRect(rectItemNextReal);

			CRect rectItemPrev;
			GetVirtualRect(curSelIndex, rectItemPrev);
			CRect rectItemPrevReal = rectItemPrev;
			GetRealRect(rectItemPrevReal);

			CRect rectClient;
			GetClientRect(GetHWND(), &rectClient);

			CRect rectIntersectNext;
			rectIntersectNext.IntersectRect(&rectClient, &rectItemNextReal);

			CRect rectIntersectPrev;
			rectIntersectPrev.IntersectRect(&rectClient, &rectItemPrevReal);

			auto offsetY = 0;

			if (rectIntersectPrev.IsRectEmpty() && rectIntersectNext.IsRectEmpty())
			{
				::SendMessage(GetHWND(), LB_SETTOPINDEX, wParam, 0);
				return 0;
			}

			if (wParam < (WPARAM)curSelIndex && curSelIndex - wParam == 1)
			{
				if (rectIntersectNext.Height() != rectItemNext.Height())
				{
					if (rectIntersectPrev.IsRectEmpty())
					{
						if (rectIntersectNext.Height() < rectItemNext.Height())
							offsetY += rectItemNext.Height() - rectIntersectNext.Height();
					}
					else
					{
						if (rectIntersectPrev.Height() < rectItemPrev.Height())
							offsetY -= rectItemPrev.Height() - rectIntersectPrev.Height();
						if (rectIntersectNext.Height() < rectItemNext.Height())
							offsetY -= rectItemNext.Height() - rectIntersectNext.Height();
					}
				}
			}
			else if (wParam > (WPARAM)curSelIndex && wParam - curSelIndex == 1)
			{
				if (rectIntersectNext.Height() != rectItemNext.Height())
				{
					if (rectIntersectPrev.IsRectEmpty())
					{
						if (rectIntersectNext.Height() < rectItemNext.Height())
							offsetY -= rectItemNext.Height() - rectIntersectNext.Height();
					}
					else
					{
						if (rectIntersectPrev.Height() < rectItemPrev.Height())
							offsetY += rectItemPrev.Height() - rectIntersectPrev.Height();

						if (rectIntersectNext.Height() < rectItemNext.Height())
							offsetY += rectItemNext.Height() - rectIntersectNext.Height();
					}
				}
			}
			else
				::SendMessage(GetHWND(), LB_SETTOPINDEX, wParam, 0);

			if (offsetY)
				SetScrollOffset(ptOffset.x, ptOffset.y + offsetY);
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

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		return 0;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		OutputDebugString(L"CStaticListBox::OnPaint\n");
		PAINTSTRUCT ps = { 0 };
		BeginPaint(GetHWND(), &ps);
		{
			CRect rect;
			GetClientRect(GetHWND(), &rect);

			CDoubleBufferScope doubleBuffer(ps.hdc, rect);
			OnPaintInternal(doubleBuffer.GetHDC(), rect);
		}
		EndPaint(GetHWND(), &ps);
		return 0;
	}

	void OnPaintInternal(HDC hdc, CRect& rectToPaint)
	{
		T* pT = static_cast<T*>(this);

		CRect rect;
		GetClientRect(GetHWND(), &rect);

		CDCHandle dc(hdc);

		pT->EraseBackground(dc, rectToPaint);

		CPoint ptViewportOrg;
		dc.SetViewportOrg(-m_ptOffset.x, -m_ptOffset.y, &ptViewportOrg);

		UINT uiCount = m_items.size();
		LONG lTop = rect.top;
		LONG lLastBotom = lTop;
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
				lLastBotom += rectItem.Height();
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
		CRect rectItem;
		GetVirtualRect(wParam, rectItem);

		CPoint ptOffset;
		GetScrollOffset(ptOffset);

		m_items.erase(m_items.begin() + wParam);

		CRect rectClient;
		GetClientRect(GetHWND(), &rectClient);

		auto fullHeight = GetFullHeight();
		if (ptOffset.y > fullHeight || ptOffset.y + rectClient.Height() > fullHeight)
		{
			fullHeight -= rectClient.Height();
			if (fullHeight < 0)
				fullHeight = 0;
			UpdateScroll(fullHeight);
		}

		InvalidateRect(GetHWND(), NULL, TRUE);
		return 0;
	}

	LRESULT OnResetContent(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		m_items.clear();
		UpdateScroll();
		InvalidateRect(GetHWND(), NULL, TRUE);
		return 0;
	}

	LRESULT OnInsertString(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		Item item = { 0 };
		if (wParam == -1)
		{
			m_items.push_back(item);
			wParam = m_items.size() - 1;
		}
		else
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

	int GetFullHeight()
	{
		int height = 0;
		for (size_t i = 0; i < m_items.size(); i++)
		{
			height += m_items[i].height;
		}
		return height;
	}

	void UpdateScroll(int yOffset = 0)
	{
		CRect rect;
		GetClientRect(GetHWND(), &rect);

		int height = GetFullHeight();

		SetScrollExtendedStyle(SCRL_SMOOTHSCROLL, SCRL_SMOOTHSCROLL);
		SetScrollSize(1, height ? height : 1, true, yOffset);
		SetScrollLine(1, 200);
		SetScrollPage(1, 500);
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
