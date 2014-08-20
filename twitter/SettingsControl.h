// SettingsControl.h : Declaration of the CSettingsControl

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "asyncsvc_contract_i.h"

using namespace ATL;

// CSettingsControl

class ATL_NO_VTABLE CSettingsControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSettingsControl, &CLSID_SettingsControl>,
	public CAxDialogImpl<CSettingsControl>,
	public CDialogResize<CSettingsControl>,
	public IControl2,
	public IPersistSettings,
	public IMsgHandler,
	public IPluginSupportNotifications,
	public IThreadServiceEventSink
{
public:
	CSettingsControl()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CSettingsControl)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IControl2)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IPersistSettings)
		COM_INTERFACE_ENTRY(IMsgHandler)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
	END_COM_MAP()

	BEGIN_DLGRESIZE_MAP(CSettingsControl)
		DLGRESIZE_CONTROL(IDC_EDITUSER, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_EDITPASSWORD, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_BUTTON1, DLSZ_CENTER_X)
	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CSettingsControl)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_BUTTON1, BN_CLICKED, OnClickedOK)
		CHAIN_MSG_MAP(CDialogResize<CSettingsControl>)
	END_MSG_MAP()

private:
	HRESULT LoadEditBoxText(int id, BSTR bstrKey, ISettings* pSettings);
	HRESULT SaveEditBoxText(int id, BSTR bstrKey, ISettings* pSettings);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	CComPtr<ISettings> m_pSettings;
	CComPtr<IThreadService> m_pThreadService;
	DWORD m_dwAdvice = 0;

public:
	enum { IDD = IDD_SETTINGSCONTROL };

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
};

OBJECT_ENTRY_AUTO(__uuidof(SettingsControl), CSettingsControl)
