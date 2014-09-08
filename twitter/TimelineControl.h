// TimelineControl.h : Declaration of the CTimelineControl

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "CustomListBox.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"
#include "Plugins.h"

using namespace ATL;

// CTimelineControl

class ATL_NO_VTABLE CTimelineControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineControl, &CLSID_TimelineControl>,
	public ITimelineControl,
	public IContainerControl,
	public IMsgHandler,
	public CAxDialogImpl<CTimelineControl>,
	public CDialogResize<CTimelineControl>,
	public IInitializeWithControlImpl,
	public IPluginSupportNotifications,
	public ICommandSupportEventSink,
	public IConnectionPointContainerImpl<CTimelineControl>,
	public IConnectionPointImpl<CTimelineControl, &__uuidof(ITimelineControlEventSink)>
{
public:
	enum { IDD = IDD_TIMELINECONTROL };

	CTimelineControl()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_TIMELINECONTROL)

	BEGIN_COM_MAP(CTimelineControl)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IControl2)
		COM_INTERFACE_ENTRY(ITimelineControl)
		COM_INTERFACE_ENTRY(IContainerControl)
		COM_INTERFACE_ENTRY(IMsgHandler)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(ICommandSupportEventSink)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CTimelineControl)
		CONNECTION_POINT_ENTRY(__uuidof(ITimelineControlEventSink))
	END_CONNECTION_POINT_MAP()
	
	BEGIN_DLGRESIZE_MAP(CTimelineControl)
		DLGRESIZE_CONTROL(IDC_LIST1, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CTimelineControl)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		NOTIFY_HANDLER(IDC_LIST1, NM_LISTBOX_LCLICK, OnColumnClick)
		NOTIFY_HANDLER(IDC_LIST1, NM_LISTBOX_RCLICK, OnColumnRClick)
		NOTIFY_HANDLER(IDC_LIST1, NM_ITEM_REMOVED, OnItemRemove)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<CTimelineControl>)
		CHAIN_MSG_MAP(CAxDialogImpl<CTimelineControl>)
	END_MSG_MAP()

private:
	CCustomListBox m_listBox;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IPluginSupport> m_pPluginSupport;
	CComPtr<ICommandSupport> m_pCommandSupport;
	DWORD m_dwAdviceCommandSupport = 0;

	CMenu m_popupMenu;

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnColumnClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnColumnRClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnItemRemove(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	HRESULT Fire_OnItemRemoved(IVariantObject *pItemObject);
	HRESULT Fire_OnColumnClick(BSTR bstrColumnName, DWORD dwColumnIndex, IColumnRects* pColumnRects, IVariantObject* pVariantObject);
public:

	STDMETHOD(GetHWND)(HWND *hWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult);

	STDMETHOD(UpdateControl)(IControl *pControl);

	STDMETHOD(ProcessWindowMessage)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plResult, BOOL* bResult);

	METHOD_EMPTY(STDMETHOD(CreateEx2)(HWND hWndParent, RECT rect, HWND* hWnd));
	STDMETHOD(GetText)(BSTR* pbstr);
	METHOD_EMPTY(STDMETHOD(OnActivate)());
	METHOD_EMPTY(STDMETHOD(OnDeactivate)());
	METHOD_EMPTY(STDMETHOD(OnClose)());

	STDMETHOD(GetItems)(IObjArray** ppObjectArray);
	STDMETHOD(InsertItems)(IObjArray* pObjectArray, UINT uiStartIndex);
	STDMETHOD(IsEmpty)(BOOL* pbIsEmpty);
	STDMETHOD(Invalidate)();
	STDMETHOD(Clear)();
	STDMETHOD(SetSkinTimeline)(ISkinTimeline* pSkinTimeline);
	STDMETHOD(BeginUpdate)();
	STDMETHOD(EndUpdate)();
	STDMETHOD(OnItemsUpdated)();

	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();
	STDMETHOD(GetTopVisibleItemIndex)(UINT* puiIndex);
	STDMETHOD(GetItemsCount)(UINT* puiCount);
	STDMETHOD(RemoveItemByIndex)(UINT uiIndex);
	STDMETHOD(RefreshItem)(UINT uiIndex);

	STDMETHOD(OnBeforeCommandInvoke)(REFGUID guidCommand, ICommand* pCommand);
	METHOD_EMPTY(STDMETHOD(OnCommandInvoke)(REFGUID guidCommand));
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineControl), CTimelineControl)
