#pragma once

#include "twitter_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;

class CUserAccountControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUserAccountControl, &CLSID_UserAccountControl>,
	public CWindowImpl<CUserAccountControl>,
	public IControl,
	public IThemeSupport,
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
		COM_INTERFACE_ENTRY(IThemeSupport)
		COM_INTERFACE_ENTRY(IControl)
	END_COM_MAP()

	BEGIN_MSG_MAP(CUserAccountControl)
	END_MSG_MAP()

private:
	CComPtr<ITheme> m_pTheme;
	CComPtr<IVariantObject> m_pVariantObject;

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