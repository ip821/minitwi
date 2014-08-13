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

	m_pSkinTimeline->DrawItem(m_hWnd, m_items[lpdi->itemID].m_T, (TDRAWITEMSTRUCT*)lpdi);
}

void CCustomListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	m_pSkinTimeline->MeasureItem(m_hWnd, m_items[lpMeasureItemStruct->itemID].m_T, (TMEASUREITEMSTRUCT*)lpMeasureItemStruct);
}

void CCustomListBox::AddItem(IVariantObject* pItemObject)
{
	m_items.push_back(CAdapt<CComPtr<IVariantObject> >(pItemObject));
	auto itemId = SendMessage(LB_ADDSTRING, 0, (LPARAM)1);
}

