#include <vector>
#include "atlctrls.h"

using namespace std;

template <class T>
class CStaticListBox : public CScrollImpl<T>
{
private:
	struct Item
	{
		int id;
		int height;
	};

	vector<Item> m_items;
public:
	BEGIN_MSG_MAP(CStaticListBox<T>)
		MESSAGE_HANDLER(WM_PAINT, OnPaint);
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPrintClient);
		MESSAGE_HANDLER(LB_GETCOUNT, OnGetCount);
		MESSAGE_HANDLER(LB_INSERTSTRING, OnInsertString);
		MESSAGE_HANDLER(LB_DELETESTRING, OnDeleteString);
		MESSAGE_HANDLER(LB_RESETCONTENT, OnResetContent);
		MESSAGE_HANDLER(LB_SETITEMHEIGHT, OnSetItemHeight);
		CHAIN_MSG_MAP(CScrollImpl<T>)
	END_MSG_MAP()

	LRESULT OnPrintClient(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		CRect rect;
		pT->GetClientRect(&rect);
		OnPaintInternal((HDC)wParam, rect);
		return 1;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);

		CRect rect;
		pT->GetClientRect(&rect);

		PAINTSTRUCT ps = { 0 };
		pT->BeginPaint(&ps);
		OnPaintInternal(ps.hdc, CRect(ps.rcPaint));
		pT->EndPaint(&ps);
		return 1;
	}

	void OnPaintInternal(HDC hdc, CRect& rect)
	{
		T* pT = static_cast<T*>(this);

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

			DRAWITEMSTRUCT di = { 0 };
			di.itemID = i;
			di.rcItem = rectItem;
			di.hDC = hdc;
			pT->DrawItem(&di);

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
		UpdateScroll();
		return 0;
	}

	void UpdateScroll()
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
		SetScrollSize(rect.Width(), height);
	}

	LRESULT OnSetItemHeight(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if (wParam >= m_items.size())
			return 0;
		m_items[wParam].height = lParam;
		UpdateScroll();
		return 0;
	}
};
