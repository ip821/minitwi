#pragma once

#include "twitter_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;

class CUserInfoControl : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUserInfoControl, &CLSID_UserInfoControl>,
	public CWindowImpl<CUserInfoControl>,
	public IControl,
	public IThemeSupport,
	public IInitializeWithControlImpl,
	public IInitializeWithVariantObject,
	public IPluginSupportNotifications
{
public:
	DECLARE_WND_CLASS(L"UserInfoControl")
	CUserInfoControl();

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CUserInfoControl)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IThemeSupport)
		COM_INTERFACE_ENTRY(IControl)
	END_COM_MAP()

	BEGIN_MSG_MAP(CUserInfoControl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
	END_MSG_MAP()

private:

	CComPtr<ITheme> m_pTheme;
	CComPtr<IVariantObject> m_pVariantObject;
	CComPtr<IControl> m_pUserAccountControl;
	CComPtr<ITimelineControl> m_pTimelineControl;
	HWND m_hWndUserAccountControl = 0;
	HWND m_hWndTimelineControl = 0;

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void AdjustSizes();

public:
	STDMETHOD(GetHWND)(HWND *hWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult);

	STDMETHOD(SetVariantObject)(IVariantObject *pVariantObject);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(SetTheme)(ITheme* pTheme);
};

OBJECT_ENTRY_AUTO(__uuidof(UserInfoControl), CUserInfoControl)
