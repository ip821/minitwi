// TimelineControl.h : Declaration of the CTimelineControl

#pragma once
#include "resource.h"       // main symbols
#include "twview_i.h"
#include "CustomListBox.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"
#include "Plugins.h"

using namespace ATL;

// CTimelineControl

class ATL_NO_VTABLE CTimelineControl :
	public CWindowImpl<CTimelineControl>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineControl, &CLSID_TimelineControl>,
	public ITimelineControl,
	public IContainerControl,
	public IMsgHandler,
	public IInitializeWithControlImpl,
	public IPluginSupportNotifications,
	public ICommandSupportEventSink,
	public IInitializeWithSettings,
	public IConnectionPointContainerImpl<CTimelineControl>,
	public IConnectionPointImpl<CTimelineControl, &__uuidof(ITimelineControlEventSink)>
{
public:
	CTimelineControl()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CTimelineControl)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IControl2)
		COM_INTERFACE_ENTRY(ITimelineControl)
		COM_INTERFACE_ENTRY(IContainerControl)
		COM_INTERFACE_ENTRY(IMsgHandler)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(ICommandSupportEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CTimelineControl)
		CONNECTION_POINT_ENTRY(__uuidof(ITimelineControlEventSink))
	END_CONNECTION_POINT_MAP()
	
	BEGIN_MSG_MAP(CTimelineControl)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		NOTIFY_CODE_HANDLER(NM_LISTBOX_LCLICK, OnColumnClick)
		NOTIFY_CODE_HANDLER(NM_LISTBOX_LDOUBLECLICK, OnItemDoubleClick)
		NOTIFY_CODE_HANDLER(NM_LISTBOX_RCLICK, OnColumnRClick)
		NOTIFY_CODE_HANDLER(NM_ITEM_REMOVED, OnItemRemove)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();
	void AdjustSizes();

private:
	CCustomListBox m_listBox;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IPluginSupport> m_pPluginSupport;
	CComPtr<ICommandSupport> m_pCommandSupport;
	CComPtr<ISettings> m_pSettings;
	DWORD m_dwAdviceCommandSupport = 0;

	CMenu m_popupMenu;

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnColumnClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnColumnRClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnItemDoubleClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnItemRemove(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	HRESULT Fire_OnItemRemoved(IVariantObject *pItemObject);
	HRESULT Fire_OnColumnClick(IColumnsInfoItem* pColumnsInfoItem, IVariantObject* pVariantObject);
	HRESULT Fire_OnItemDoubleClick(IVariantObject* pVariantObject);
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
	STDMETHOD(InsertItem)(IVariantObject* pVariantObject, UINT uiIndex);
	STDMETHOD(InsertItems)(IObjArray* pObjectArray, UINT uiStartIndex);
	STDMETHOD(IsEmpty)(BOOL* pbIsEmpty);
	STDMETHOD(InvalidateItems)(IVariantObject** pItemArray, UINT uiCountArray);
	STDMETHOD(RefreshItems)(IVariantObject** pItemArray, UINT uiCountArray);
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
	STDMETHOD(ScrollToItem)(UINT uiIndex);

	STDMETHOD(OnBeforeCommandInvoke)(REFGUID guidCommand, int iCommandSource, ICommand* pCommand);
	METHOD_EMPTY(STDMETHOD(OnCommandInvoke)(REFGUID guidCommand));
	STDMETHOD(Load)(ISettings *pSettings);
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineControl), CTimelineControl)
