// SettingsControl.h : Declaration of the CSettingsControl

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"

using namespace ATL;

// CSettingsControl

class ATL_NO_VTABLE CSettingsControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSettingsControl, &CLSID_SettingsControl>,
	public IControl2,
	public IPersistSettings,
	public CAxDialogImpl<CSettingsControl>,
	public IMsgHandler
{
public:
	CSettingsControl()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CSettingsControl)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IControl2)
		COM_INTERFACE_ENTRY(IPersistSettings)
		COM_INTERFACE_ENTRY(IMsgHandler)
	END_COM_MAP()

	BEGIN_MSG_MAP(CSettingsControl)
	END_MSG_MAP()

private:
	HRESULT LoadEditBoxText(int id, BSTR bstrKey, ISettings* pSettings);
	HRESULT SaveEditBoxText(int id, BSTR bstrKey, ISettings* pSettings);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
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

};

OBJECT_ENTRY_AUTO(__uuidof(SettingsControl), CSettingsControl)
