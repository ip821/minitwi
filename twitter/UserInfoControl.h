#pragma once

#include "twitter_i.h"
#include "asyncsvc_contract_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;

class CUserInfoControl : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUserInfoControl, &CLSID_UserInfoControl>,
	public CWindowImpl<CUserInfoControl>,
	public IControl2,
	public IThemeSupport,
	public IThreadServiceEventSink,
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
		COM_INTERFACE_ENTRY(IThemeSupport)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IControl2)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
	END_COM_MAP()

	BEGIN_MSG_MAP(CUserInfoControl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
	END_MSG_MAP()

private:

	CComPtr<IPluginSupport> m_pPluginSupport;
	CComQIPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<ITheme> m_pTheme;
	CComPtr<IVariantObject> m_pVariantObject;
	CComPtr<IUserAccountControl> m_pUserAccountControl;
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

	STDMETHOD(SetTheme)(ITheme* pTheme);

	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();

	METHOD_EMPTY(STDMETHOD(CreateEx2)(HWND hWndParent, RECT rect, HWND* hWnd));
	METHOD_EMPTY(STDMETHOD(GetText)(BSTR* pbstr));
	STDMETHOD(OnActivate)();
	METHOD_EMPTY(STDMETHOD(OnDeactivate)());
	METHOD_EMPTY(STDMETHOD(OnClose)());

	METHOD_EMPTY(STDMETHOD(OnStart)(IVariantObject *pResult));
	METHOD_EMPTY(STDMETHOD(OnRun)(IVariantObject *pResult));
	METHOD_EMPTY(STDMETHOD(OnFinish)(IVariantObject *pResult));
};

OBJECT_ENTRY_AUTO(__uuidof(UserInfoControl), CUserInfoControl)
