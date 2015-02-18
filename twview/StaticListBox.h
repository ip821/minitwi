#pragma once

#include "atlctrls.h"

using namespace std;

template <class TBase>
class CStaticListBoxT : public TBase
{
public:
	int GetCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_GETCOUNT, 0, 0L);
	}

	int GetTopIndex() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_GETTOPINDEX, 0, 0L);
	}

	int SetTopIndex(int nIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_SETTOPINDEX, nIndex, 0L);
	}

	int DeleteString(UINT nIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_DELETESTRING, nIndex, 0L);
	}

	int InsertString(int nIndex, LPCTSTR lpszItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_INSERTSTRING, nIndex, (LPARAM)lpszItem);
	}

	void ResetContent()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, LB_RESETCONTENT, 0, 0L);
	}

	int GetItemRect(int nIndex, LPRECT lpRect) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LB_GETITEMRECT, nIndex, (LPARAM)lpRect);
	}

	int GetCurSel() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT((GetStyle() & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)) == 0);
		return (int)::SendMessage(m_hWnd, LB_GETCURSEL, 0, 0L);
	}

	int SetCurSel(int nSelect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT((GetStyle() & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)) == 0);
		return (int)::SendMessage(m_hWnd, LB_SETCURSEL, nSelect, 0L);
	}
};

typedef CStaticListBoxT<ATL::CWindow> CStaticListBox;
