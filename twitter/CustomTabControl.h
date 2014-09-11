#pragma once

#include "twitter_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;

class CCustomTabControl :
	public CWindowImpl<CCustomTabControl>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCustomTabControl, &CLSID_CustomTabControl>,
	public ICustomTabControl,
	public IControl,
	public IInitializeWithControlImpl,
	public IPersistSettings,
	public IPluginSupportNotifications,
	public IConnectionPointContainerImpl<CCustomTabControl>,
	public IConnectionPointImpl<CCustomTabControl, &__uuidof(ITabbedControlEventSink)>
{

public:
	DECLARE_WND_CLASS(L"CustomTabControl")

	CCustomTabControl()
	{

	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CCustomTabControl)
		COM_INTERFACE_ENTRY(ICustomTabControl)
		COM_INTERFACE_ENTRY(ITabbedControl)
		COM_INTERFACE_ENTRY(IContainerControl)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IPersistSettings)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
	END_COM_MAP()

	BEGIN_MSG_MAP(CCustomTabControl)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
	END_MSG_MAP()

	BEGIN_CONNECTION_POINT_MAP(CCustomTabControl)
		CONNECTION_POINT_ENTRY(__uuidof(ITabbedControlEventSink))
	END_CONNECTION_POINT_MAP()

private:
	vector<CAdapt<CComPtr<IControl>>> m_pControls;
	int m_selectedPageIndex = -1;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<ISettings> m_pSettings;
	CRect m_rectChildControlArea;
	CComPtr<ISkinTabControl> m_pSkinTabControl;

	void SelectPage(DWORD dwIndex);

	CRect& GetChildControlAreaRect();
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
public:

	STDMETHOD(GetHWND)(HWND *hWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult);

	STDMETHOD(UpdateControl)(IControl *pControl);

	STDMETHOD(SetSkinTabControl)(ISkinTabControl* pSkinTabControl);

	STDMETHOD(EnableCommands)(BOOL bEnabled);
	STDMETHOD(AddPage)(IControl *pControl);
	STDMETHOD(GetCurrentPage)(IControl **ppControl);
	STDMETHOD(RemovePage)(IControl *pControl);
	STDMETHOD(ActivatePage)(IControl *pControl);
	STDMETHOD(GetPageCount)(DWORD *pdwCount);
	STDMETHOD(GetPage)(DWORD dwIndex, IControl **ppControl);

	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(Load)(ISettings* pSettings);
	STDMETHOD(Save)(ISettings* pSettings);
	METHOD_EMPTY(STDMETHOD(Reset)(ISettings* /*pSettings*/));
};

OBJECT_ENTRY_AUTO(__uuidof(CustomTabControl), CCustomTabControl)
