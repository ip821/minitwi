#include <vector>
#include "atlctrls.h"

using namespace std;

template <class T>
class CStaticListBox
{
private:
	struct Item
	{
		int id;
	};

	vector<Item> m_items;
public:
	BEGIN_MSG_MAP(CStaticListBox<T>)
		ALT_MSG_MAP(1)
		MESSAGE_HANDLER(WM_PAINT, OnPaint);
		MESSAGE_HANDLER(LB_GETCOUNT, OnGetCount);
		MESSAGE_HANDLER(LB_INSERTSTRING, OnInsertString);
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		//T* pT = static_cast<T*>(this);
		//PAINTSTRUCT ps = { 0 };
		//BeginPaint(&ps);
		//UINT uiCount = 0;
		//ASSERT_IF_FAILED(GetCount(&uiCount));
		//CRect rect;
		//GetClientRect(&rect);
		//LONG lTop = 0; rect.top;
		//for (size_t i = 0; i < uiCount; i++)
		//{
		//	MEASUREITEMSTRUCT mi = { 0 };
		//	mi.itemID = i;
		//	mi.itemWidth = rect.Width();
		//	MeasureItem(&mi);

		//	CRect rectItem;
		//	rectItem.left = rect.left;
		//	rectItem.right = rect.right;
		//	rectItem.top = lTop;
		//	rectItem.bottom = rectItem.top + mi.itemHeight;
		//	lTop += rectItem.Height();

		//	DRAWITEMSTRUCT di = { 0 };
		//	di.itemID = i;
		//	di.rcItem = rectItem;
		//	di.hDC = ps.hdc;
		//	DrawItem(&di);
		//}
		//EndPaint(&ps);
		return 1;
	}

	LRESULT OnGetCount(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		return m_items.size();
	}

	LRESULT OnInsertString(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		Item item = { 0 };
		m_items.insert(m_items.begin() + wParam, item);
		return 1;
	}
};

//typedef CStaticListBoxT<CListBox> CStaticListBox;