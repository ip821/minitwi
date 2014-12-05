#pragma once

#include "atlctrls.h"
#include "twitter_i.h"
#include "AnimationTimerSupport.h"
#include <atomic>

using namespace std;

struct NMCOLUMNCLICK
{
	NMHDR nmhdr;
	int dwCurrentItem;
	int dwCurrentColumn;
	IColumnsInfo* pColumnsInfo;
	IVariantObject* pVariantObject;
	int x;
	int y;
};

struct NMITEMREMOVED
{
	NMHDR nmhdr;
	IColumnsInfo* pColumnsInfo;
	IVariantObject* pVariantObject;
};

class CCustomListBox :
	public CWindowImpl<CCustomListBox, CListBox>,
	public COwnerDraw <CCustomListBox>,
	public CAnimationTimerSupport<CCustomListBox>
{
public:
	DECLARE_WND_CLASS(_T("WTL_CCustomListBox"))

	BEGIN_MSG_MAP(CCustomListBox)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_ANIMATION_TIMER, OnAnimationTimer)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLMouseDoubleClick);
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLMouseButtonUp);
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRMouseButtonUp);
		MESSAGE_HANDLER(WM_GETDLGCODE, OnKeyDown);
		CHAIN_MSG_MAP_ALT(COwnerDraw<CCustomListBox>, 1)
	END_MSG_MAP()

	CCustomListBox();

	static const int INVALID_COLUMN_INDEX = -1;
	static const int INVALID_ITEM_INDEX = -1;

private:
	CComPtr<IObjCollection> m_pItems;
	vector<CAdapt<CComPtr<IColumnsInfo>>> m_columnsInfo;
	CComPtr<ISkinTimeline> m_pSkinTimeline;
	int m_prevX = 0;
	int m_prevY = 0;
	int m_HoveredItemIndex = INVALID_ITEM_INDEX;
	int m_HoveredColumnIndex = INVALID_COLUMN_INDEX;
	CCursor m_handCursor;
	CCursor m_arrowCursor;
	BOOL m_bAnimationNeeded = FALSE;
	BOOL m_bAnimating = FALSE;
	BOOL m_bEnableAnimation;
	atomic<int> m_updateTefCount = 0;

	map<IVariantObject*, hash_set<int>> m_animatedColumns;

	LRESULT HandleCLick(LPARAM lParam, UINT uiCode);
	void UpdateAnimatedColumns(IColumnsInfo* pColumnsInfo, int itemIndex, IVariantObject* pVariantObject, BOOL bRegisterForAnimation);
	void StartAnimation();

public:
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnLMouseDoubleClick(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnLMouseButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnRMouseButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnAnimationTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);

	void DrawItem(LPDRAWITEMSTRUCT lpdi);
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void InsertItem(IVariantObject* pItemObject, int index);
	void SetSkinTimeline(ISkinTimeline* pSkin);
	void Clear();
	HRESULT GetItems(IObjArray** ppObjectArray);
	void IsEmpty(BOOL* pbEmpty);
	void OnItemsUpdated();
	void RemoveItemByIndex(UINT uiIndex);
	void RefreshItem(UINT uiIndex);
	void BeginUpdate();
	void EndUpdate();
	void InvalidateItems(IVariantObject** pItemArray, UINT uiCountArray);
	void RefreshItems(IVariantObject** pItemArray, UINT uiCountArray);
	void EnableAnimation(BOOL bEnable);
};

