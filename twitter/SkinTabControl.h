#pragma once

#include "twitter_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;

class CSkinTabControl :
	public CWindowImpl<CSkinTabControl>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSkinTabControl, &CLSID_SkinTabControl>,
	public ITabbedControl,
	public IControl,
	public IInitializeWithControlImpl,
	public IPersistSettings,
	public IPluginSupportNotifications,
	public IConnectionPointContainerImpl<CSkinTabControl>,
	public IConnectionPointImpl<CSkinTabControl, &__uuidof(ITabbedControlEventSink)>
{

public:
	DECLARE_WND_CLASS(L"SkinTabControl")

	CSkinTabControl()
	{

	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CSkinTabControl)
		COM_INTERFACE_ENTRY(ITabbedControl)
		COM_INTERFACE_ENTRY(IContainerControl)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IPersistSettings)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
	END_COM_MAP()

	BEGIN_MSG_MAP(CSkinTabControl)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
	END_MSG_MAP()

	BEGIN_CONNECTION_POINT_MAP(CSkinTabControl)
		CONNECTION_POINT_ENTRY(__uuidof(ITabbedControlEventSink))
	END_CONNECTION_POINT_MAP()

private:
	vector<CAdapt<CComPtr<IControl>>> m_pControls;
	int m_selectedPageIndex = -1;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<ISettings> m_pSettings;

	void SelectPage(DWORD dwIndex);

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
public:

	STDMETHOD(GetHWND)(HWND *hWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult);

	STDMETHOD(UpdateControl)(IControl *pControl);

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

OBJECT_ENTRY_AUTO(__uuidof(SkinTabControl), CSkinTabControl)