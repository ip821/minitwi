#pragma once

#include "twitter_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"
#include "asyncsvc_contract_i.h"

using namespace ATL;
using namespace std;

class ATL_NO_VTABLE CHomeTimeLineControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CHomeTimeLineControl, &CLSID_HomeTimeLineControl>,
	public CWindowImpl<CHomeTimeLineControl>,
	public IHomeTimeLineControl,
	public IThemeSupport,
	public IInitializeWithControlImpl,
	public IPluginSupportNotifications,
	public IInitializeWithSettings
{
public:
	CHomeTimeLineControl()
	{
	}

	DECLARE_NO_REGISTRY()
	BEGIN_COM_MAP(CHomeTimeLineControl)
		COM_INTERFACE_ENTRY(IHomeTimeLineControl)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IControl2)
		COM_INTERFACE_ENTRY(IThemeSupport)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
	END_COM_MAP()

	BEGIN_MSG_MAP(CUserInfoControl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		bHandled = TRUE;
		lResult = OnMessage(uMsg, wParam, lParam, bHandled);
		if (bHandled)
			return TRUE;
	END_MSG_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

private:
	CComPtr<IPluginSupport> m_pPluginSupport;
	CComQIPtr<IServiceProvider> m_pServiceProviderParent;
	CComQIPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<ITheme> m_pTheme;
	CComPtr<ITimelineControl> m_pTimelineControl;
	CComPtr<ITimelineService> m_pTimelineService;
	CComPtr<ISettings> m_pSettings;
	CComPtr<IThreadService> m_pThreadServiceShowMoreTimeline;
	CComPtr<IThreadService> m_pThreadServiceUpdateTimeline;
	CComPtr<ITimerService> m_pTimerService;

	DWORD m_dwAdviceUpdateTimeline = 0;
	DWORD m_dwAdviceShowMoreTimeline = 0;
	HWND m_hWndTimelineControl = 0;

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void AdjustSizes();

public:
	STDMETHOD(GetTimelineControl)(ITimelineControl** ppTimelineControl);
	STDMETHOD(StartTimers)();
	STDMETHOD(StopTimers)();

	STDMETHOD(GetHWND)(HWND *hWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult);

	METHOD_EMPTY(STDMETHOD(CreateEx2)(HWND hWndParent, RECT rect, HWND* hWnd));
	METHOD_EMPTY(STDMETHOD(GetText)(BSTR* pbstr));
	STDMETHOD(OnActivate)();
	STDMETHOD(OnDeactivate)();
	METHOD_EMPTY(STDMETHOD(OnClose)());

	STDMETHOD(Load)(ISettings* pSettings);

	STDMETHOD(SetTheme)(ITheme* pTheme);

	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();
};

OBJECT_ENTRY_AUTO(__uuidof(HomeTimeLineControl), CHomeTimeLineControl)