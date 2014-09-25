#pragma once

#include "twitter_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;
using namespace Gdiplus;

class CUserAccountControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUserAccountControl, &CLSID_UserAccountControl>,
	public CAxDialogImpl<CUserAccountControl>,
	public CDialogResize<CUserAccountControl>,
	public IControl,
	public IThemeSupport,
	public IInitializeWithControlImpl,
	public IInitializeWithVariantObject,
	public IPluginSupportNotifications
{
public:
	enum{ IDD = IDD_USER_ACCOUNT_CONTROL };
	//DECLARE_WND_CLASS(L"UserAccountControl")
	CUserAccountControl();

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CUserAccountControl)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IThemeSupport)
		COM_INTERFACE_ENTRY(IControl)
	END_COM_MAP()

	BEGIN_MSG_MAP(CUserAccountControl)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		//MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CUserAccountControl)
		DLGRESIZE_CONTROL(IDC_LABEL_DISPLAY_NAME, DLSZ_CENTER_X)
		DLGRESIZE_CONTROL(IDC_LABEL_SCREEN_NAME, DLSZ_CENTER_X)
		DLGRESIZE_CONTROL(IDC_LABEL_DESCRIPTION, DLSZ_CENTER_X)
	END_DLGRESIZE_MAP()
private:
	CComPtr<ITheme> m_pTheme;
	CComPtr<IVariantObject> m_pVariantObject;
	CBitmap m_bitmap;

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	STDMETHOD(GetHWND)(HWND *hWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult);

	STDMETHOD(SetVariantObject)(IVariantObject *pVariantObject);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(SetTheme)(ITheme* pTheme);
};

OBJECT_ENTRY_AUTO(__uuidof(UserAccountControl), CUserAccountControl)