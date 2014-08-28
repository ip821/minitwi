#pragma once

#include "atlctrls.h"
#include "twitter_i.h"

struct NMCOLUMNCLICK
{
	NMHDR nmhdr;
	DWORD dwCurrentItem;
	DWORD dwCurrentColumn;
	IColumnRects* pColumnRects;
	IVariantObject* pVariantObject;
	int x;
	int y;
};

struct NMITEMREMOVED
{
	NMHDR nmhdr;
	IColumnRects* pColumnRects;
	IVariantObject* pVariantObject;
};

class CCustomListBox :
	public CWindowImpl<CCustomListBox, CListBox>,
	public COwnerDraw < CCustomListBox >
{
public:
	DECLARE_WND_CLASS(_T("WTL_CCustomListBox"))

	BEGIN_MSG_MAP(CCustomListBox)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLMouseButtonUp);
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRMouseButtonUp);
		CHAIN_MSG_MAP_ALT(COwnerDraw<CCustomListBox>, 1)
	END_MSG_MAP()

	CCustomListBox();
private:
	std::vector<CAdapt<CComPtr<IVariantObject> > > m_items;
	std::vector<CAdapt<CComPtr<IColumnRects> > > m_columnRects;
	CComPtr<ISkinTimeline> m_pSkinTimeline;
	int m_prevX = 0;
	int m_prevY = 0;
	int m_HoveredItemIndex = -1;
	int m_HoveredColumnIndex = -1;
	CCursor m_handCursor;
	CCursor m_arrowCursor;

	LRESULT HandleCLick(LPARAM lParam, UINT uiCode);

public:
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnLMouseButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnRMouseButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);

	void DrawItem(LPDRAWITEMSTRUCT lpdi);
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void AddItem(IVariantObject* pItemObject);
	void SetSkinTimeline(ISkinTimeline* pSkin);
	void Clear();
	HRESULT GetItems(IObjArray** ppObjectArray);
	void IsEmpty(BOOL* pbEmpty);
	void OnItemsUpdated();
};

