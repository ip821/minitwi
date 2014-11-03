// SettingsControl.h : Declaration of the CSearchControl

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "asyncsvc_contract_i.h"
#include "twitconn_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;

// CSearchControl

class ATL_NO_VTABLE CSearchControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSearchControl, &CLSID_SettingsControl>,
	public CAxDialogImpl<CSearchControl>,
	public CDialogResize<CSearchControl>,
	public ISearchControl,
	public IInitializeWithSettings,
	public IPluginSupportNotifications,
	public IThreadServiceEventSink,
	public IInitializeWithControlImpl,
	public IThemeSupport
{
public:
	CSearchControl()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CSearchControl)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IControl2)
		COM_INTERFACE_ENTRY(ISearchControl)
		COM_INTERFACE_ENTRY(IThemeSupport)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
	END_COM_MAP()

	BEGIN_DLGRESIZE_MAP(CSearchControl)
		DLGRESIZE_CONTROL(IDC_EDITTEXT, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_BUTTONGO, DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CSearchControl)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_BUTTONGO, BN_CLICKED, OnClickedGo)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		CHAIN_MSG_MAP(CDialogResize<CSearchControl>)
	END_MSG_MAP()

private:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickedGo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CComPtr<ISettings> m_pSettings;
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComQIPtr<ISearchTimelineControl> m_pTimelineControl;
	CComPtr<ITheme> m_pTheme;
	CComPtr<IViewControllerService> m_pViewControllerService;

	DWORD m_dwAdvice = 0;
	HWND m_hWndTimelineControl = 0;
	CEdit m_editText;
	CButton m_buttonGo;

	void EnableControls(BOOL bEnable);
	void AdjustSizes();
	STDMETHOD(DoSearch)();

public:
	enum { IDD = IDD_SEARCHCONTROL };

	STDMETHOD(GetHWND)(HWND *hWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult);

	STDMETHOD(Load)(ISettings *pSettings);

	METHOD_EMPTY(STDMETHOD(CreateEx2)(HWND hWndParent, RECT rect, HWND* hWnd));
	STDMETHOD(GetText)(BSTR* pbstr);
	STDMETHOD(OnActivate)();
	STDMETHOD(OnDeactivate)();
	METHOD_EMPTY(STDMETHOD(OnClose)());

	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	METHOD_EMPTY(STDMETHOD(OnRun)(IVariantObject *pResult));
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(SetTheme)(ITheme* pTheme);
};

OBJECT_ENTRY_AUTO(__uuidof(SearchControl), CSearchControl)
