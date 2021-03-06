#pragma once

#include <atomic>
#include "atlctrls.h"
#include "twview_i.h"
#include "AnimationTimer.h"
#include "StaticListBoxImpl.h"
#include "StaticListBox.h"

using namespace std;
using namespace IP;

struct NMCOLUMNCLICK
{
	NMHDR nmhdr;
	int dwCurrentItem;
	IColumnsInfoItem* pColumnsInfoItem;
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

#define WINDOW_CLASS L"WTL_CCustomListBox"

class CCustomListBox :
	public CWindowImpl<CCustomListBox, CStaticListBox>,
	public CStaticListBoxImpl<CCustomListBox>,
	public COwnerDraw <CCustomListBox>
{
public:
	DECLARE_WND_CLASS(WINDOW_CLASS)

	BEGIN_MSG_MAP(CCustomListBox)
		MESSAGE_HANDLER(WM_ANIMATION_TIMER, OnAnimationTimer)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLMouseDoubleClick)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLMouseButtonUp)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRMouseButtonUp)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		CHAIN_MSG_MAP(CStaticListBoxImpl<CCustomListBox>)
		CHAIN_MSG_MAP_ALT(COwnerDraw<CCustomListBox>, 1)
	END_MSG_MAP()

	CCustomListBox();
	~CCustomListBox();

private:
	CComPtr<IObjCollection> m_pItems;
	vector<CComPtr<IColumnsInfo>> m_columnsInfo;
	CComPtr<ISkinTimeline> m_pSkinTimeline;
	CComPtr<ISettings> m_pSettings;
	int m_prevX = 0;
	int m_prevY = 0;
	int m_HoveredItemIndex = INVALID_ITEM_INDEX;
	int m_PrevHoveredItemIndex = INVALID_ITEM_INDEX;
	CComBSTR m_bstrHoveredColumnName;
	CComBSTR m_bstrPrevHoveredColumnName;
	CRect m_prevColumnRect;
	CCursor m_handCursor;
	CCursor m_arrowCursor;
	BOOL m_bAnimationNeeded = FALSE;
	BOOL m_bAnimating = FALSE;
	BOOL m_bEnableAnimation;
	atomic<int> m_updateRefCount = 0;
	CAnimationTimer<CCustomListBox> m_animationTimerFade;
	map<IVariantObject*, unordered_set<int>> m_animatedColumns;

	LRESULT HandleClick(LPARAM lParam, UINT uiCode);
	void UpdateAnimatedColumns(IColumnsInfo* pColumnsInfo, int itemIndex, IVariantObject* pVariantObject, BOOL bRegisterForAnimation);
	void StartFadeAnimation();

public:
	HWND Create(HWND hWndParent, ATL::_U_RECT rect = NULL, LPCTSTR szWindowName = NULL, DWORD dwStyle = 0, DWORD dwExStyle = 0, ATL::_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL);

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnLMouseDoubleClick(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnLMouseButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnRMouseButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnAnimationTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	void DoSize(int cx, int cy);

	void PreTranslateMessage(HWND hWnd, MSG *pMsg, BOOL *pbResult);
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
	void SetSettings(ISettings* pSettings);
};

