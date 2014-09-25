#pragma once

#include "twitter_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;
using namespace Gdiplus;

class CUserAccountControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUserAccountControl, &CLSID_UserAccountControl>,
	public CWindowImpl<CUserAccountControl>,
	public CDialogResize<CUserAccountControl>,
	public IUserAccountControl,
	public IInitializeWithControlImpl,
	public IInitializeWithVariantObject,
	public IPluginSupportNotifications
{
public:
	DECLARE_WND_CLASS(L"UserAccountControl")
	CUserAccountControl();

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CUserAccountControl)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IUserAccountControl)
	END_COM_MAP()

	BEGIN_MSG_MAP(CUserAccountControl)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPrintClient)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

private:
	CComPtr<ISkinUserAccountControl> m_pSkinUserAccountControl;
	CComPtr<IVariantObject> m_pVariantObject;
	CComPtr<IImageManagerService> m_pImageManagerService;
	CComPtr<IThemeColorMap> m_pThemeColorMap;
	CComPtr<IThemeFontMap> m_pThemeFontMap;

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPrintClient(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	STDMETHOD(GetHWND)(HWND *hWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult);

	STDMETHOD(SetVariantObject)(IVariantObject *pVariantObject);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(SetSkinUserAccountControl)(ISkinUserAccountControl* pSkinUserAccountControl);
};

OBJECT_ENTRY_AUTO(__uuidof(UserAccountControl), CUserAccountControl)