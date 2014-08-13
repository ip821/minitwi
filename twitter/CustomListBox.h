#pragma once

#include "atlctrls.h"
#include "twitter_i.h"

class CCustomListBox : 
	public CWindowImpl<CCustomListBox, CListBox>,
	public COwnerDraw<CCustomListBox>
{
public:
	DECLARE_WND_CLASS(_T("WTL_CCustomListBox"))

	BEGIN_MSG_MAP(CCustomListBox)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		CHAIN_MSG_MAP_ALT(COwnerDraw<CCustomListBox>, 1)
	END_MSG_MAP()

private:
	std::vector<CAdapt<CComPtr<IVariantObject> > > m_items;
	CComPtr<ISkinTimeline> m_pSkinTimeline;
public:
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);

	void DrawItem(LPDRAWITEMSTRUCT lpdi);
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void AddItem(IVariantObject* pItemObject);
	void SetSkinTimeline(ISkinTimeline* pSkin);
};

