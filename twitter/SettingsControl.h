// SettingsControl.h : Declaration of the CSettingsControl

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "asyncsvc_contract_i.h"
#include "twitconn_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;

// CSettingsControl

class ATL_NO_VTABLE CSettingsControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSettingsControl, &CLSID_SettingsControl>,
	public CAxDialogImpl<CSettingsControl>,
	public CDialogResize<CSettingsControl>,
	public ISettingsControl,
	public IPersistSettings,
	public IMsgHandler,
	public IPluginSupportNotifications,
	public IThreadServiceEventSink,
	public IInitializeWithControlImpl,
	public IThemeSupport
{
public:
	CSettingsControl()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CSettingsControl)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IControl2)
		COM_INTERFACE_ENTRY(ISettingsControl)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IPersistSettings)
		COM_INTERFACE_ENTRY(IMsgHandler)
		COM_INTERFACE_ENTRY(IThemeSupport)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
	END_COM_MAP()

	BEGIN_DLGRESIZE_MAP(CSettingsControl)
		
		DLGRESIZE_CONTROL(IDC_GROUP1, DLSZ_CENTER_X)
		DLGRESIZE_CONTROL(IDC_LABEL_LOGGED_USER, DLSZ_CENTER_X)

		BEGIN_DLGRESIZE_GROUP()
		DLGRESIZE_CONTROL(IDC_LABEL_LOGIN, DLSZ_CENTER_X)
		DLGRESIZE_CONTROL(IDC_EDITUSER, DLSZ_CENTER_X)
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
		DLGRESIZE_CONTROL(IDC_LABEL_PASS, DLSZ_CENTER_X)
		DLGRESIZE_CONTROL(IDC_EDITPASSWORD, DLSZ_CENTER_X)
		END_DLGRESIZE_GROUP()

		DLGRESIZE_CONTROL(IDC_BUTTON_LOGIN, DLSZ_CENTER_X)
		DLGRESIZE_CONTROL(IDC_BUTTON_LOGOUT, DLSZ_CENTER_X)

		DLGRESIZE_CONTROL(IDC_GROUP2, DLSZ_CENTER_X)
		
		BEGIN_DLGRESIZE_GROUP()
		DLGRESIZE_CONTROL(IDC_LABEL_MINITWI, DLSZ_CENTER_X)
		DLGRESIZE_CONTROL(IDC_LABEL_VERSION, DLSZ_CENTER_X)
		
		END_DLGRESIZE_GROUP()
		DLGRESIZE_CONTROL(IDC_LABEL_HOMEPAGE, DLSZ_CENTER_X)

	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CSettingsControl)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_BUTTON_LOGIN, BN_CLICKED, OnClickedLogin)
		COMMAND_HANDLER(IDC_BUTTON_LOGOUT, BN_CLICKED, OnClickedLogout)
		CHAIN_MSG_MAP(CDialogResize<CSettingsControl>)
	END_MSG_MAP()

private:
	HRESULT LoadEditBoxText(int id, BSTR bstrKey, ISettings* pSettings);
	HRESULT SaveEditBoxText(int id, BSTR bstrKey, ISettings* pSettings);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickedLogin(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedLogout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCtlColorStatic(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CComPtr<ISettings> m_pSettings;
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IFormManager> m_pFormManager;
	CComQIPtr<IHomeTimeLineControl> m_pHomeTimeLineControl;
	CComQIPtr<ITimelineControl> m_pTimelineControl;
	CComPtr<ITheme> m_pTheme;
	CComQIPtr<ICustomTabControl> m_pCustomTabControl;
	CComPtr<IViewControllerService> m_pViewControllerService;

	DWORD m_dwAdvice = 0;
	CEdit m_editUser;
	CEdit m_editPass;

	CStatic m_labelLoggedUser;
	CStatic m_labelVersion;
	CHyperLink m_labelHomePage;

	void SwitchToLoginMode();
	void SwitchToLogoutMode();
	void EnableLoginControls(BOOL bEnale);

public:
	enum { IDD = IDD_SETTINGSCONTROL };

	bool DlgResize_PositionControl(int in_nWidth, int in_nHeight, RECT& in_sGroupRect, _AtlDlgResizeData&  in_sData, bool in_bGroup, _AtlDlgResizeData* in_pDataPrev = NULL);

	STDMETHOD(GetHWND)(HWND *hWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult);

	STDMETHOD(ProcessWindowMessage)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult, BOOL *bResult);

	STDMETHOD(Load)(ISettings *pSettings);
	STDMETHOD(Save)(ISettings *pSettings);
	STDMETHOD(Reset)(ISettings *pSettings);

	METHOD_EMPTY(STDMETHOD(CreateEx2)(HWND hWndParent, RECT rect, HWND* hWnd));
	STDMETHOD(GetText)(BSTR* pbstr);
	METHOD_EMPTY(STDMETHOD(OnActivate)());
	METHOD_EMPTY(STDMETHOD(OnDeactivate)());
	METHOD_EMPTY(STDMETHOD(OnClose)());

	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(SetTheme)(ITheme* pTheme);
};

OBJECT_ENTRY_AUTO(__uuidof(SettingsControl), CSettingsControl)
