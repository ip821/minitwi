#pragma once 

#include "twitter_i.h"
#include "asyncsvc_contract_i.h"

template <class T>
class ATL_NO_VTABLE CBaseTimeLineControl : 
	public IInitializeWithControlImpl,
	public IControl2,
	public IThemeSupport,
	public IPluginSupportNotifications,
	public IInitializeWithSettings,
	public IServiceProviderSupport,
	public ITimelineControlSupport
{
protected:
	CComPtr<IPluginSupport> m_pPluginSupport;
	CComQIPtr<IServiceProvider> m_pServiceProviderParent;
	CComQIPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<ITheme> m_pTheme;
	CComPtr<ITimelineControl> m_pTimelineControl;
	CComPtr<ITimelineService> m_pTimelineService;
	CComPtr<ISettings> m_pSettings;
	CComPtr<ITimerService> m_pTimerService;

	HWND m_hWndTimelineControl = 0;

	virtual HRESULT Initializing()
	{
		return S_OK;
	}

	virtual HRESULT Initialized()
	{
		return S_OK;
	}

	virtual HRESULT ShuttingDown()
	{
		return S_OK;
	}

public:
	METHOD_EMPTY(STDMETHOD(CreateEx2)(HWND hWndParent, RECT rect, HWND* hWnd));
	METHOD_EMPTY(STDMETHOD(OnClose)());
	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider)
	{
		T* pT = static_cast<T*>(this);

		CHECK_E_POINTER(pServiceProvider);
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_PluginSupport, &m_pPluginSupport));
		RETURN_IF_FAILED(Initializing());

		m_pServiceProviderParent = pServiceProvider;
		RETURN_IF_FAILED(m_pPluginSupport->SetParentServiceProvider(m_pServiceProviderParent));
		m_pServiceProvider = m_pPluginSupport;
		ATLASSERT(m_pServiceProvider);

		CComPtr<IControl> pControl;
		RETURN_IF_FAILED(pT->QueryInterface(__uuidof(IControl), (LPVOID*)&pControl));

		CComPtr<IUnknown> pUnk;
		RETURN_IF_FAILED(pT->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

		RETURN_IF_FAILED(HrInitializeWithControl(m_pTimelineControl, pControl));
		RETURN_IF_FAILED(HrInitializeWithControl(m_pPluginSupport, pControl));

		RETURN_IF_FAILED(HrNotifyOnInitialized(m_pTimelineControl, m_pPluginSupport));

		CComPtr<ITimelineCleanupService> pTimelineCleanupService;
		if (pTimelineCleanupService)
		{
			RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_TimelineCleanupService, &pTimelineCleanupService));
			RETURN_IF_FAILED(pTimelineCleanupService->SetTimelineControl(m_pTimelineControl));
		}

		CComPtr<ITimelineImageService> pTimelineImageService;
		RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_TimelineImageService, &pTimelineImageService));
		RETURN_IF_FAILED(pTimelineImageService->SetTimelineControl(m_pTimelineControl));

		RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_TIMER, &m_pTimerService));
		RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE, &m_pTimelineService));
		RETURN_IF_FAILED(m_pTimelineService->SetTimelineControl(m_pTimelineControl));
		RETURN_IF_FAILED(HrInitializeWithSettings(m_pTimelineService, m_pSettings));

		RETURN_IF_FAILED(m_pPluginSupport->OnInitialized());

		CComPtr<IThreadService> pThreadServiceUpdateTimeline;
		RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &pThreadServiceUpdateTimeline));
		RETURN_IF_FAILED(pThreadServiceUpdateTimeline->SetTimerService(SERVICE_TIMELINE_TIMER));

		CComPtr<IThreadPoolService> pThreadPoolService;
		RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ThreadPoolService, &pThreadPoolService));
		RETURN_IF_FAILED(pThreadPoolService->SetThreadCount(6));
		RETURN_IF_FAILED(pThreadPoolService->Start());

		RETURN_IF_FAILED(Initialized());

		return S_OK;
	}

	STDMETHOD(OnShutdown)()
	{
		RETURN_IF_FAILED(ShuttingDown());
		{
			CComQIPtr <IPluginSupportNotifications> pPluginSupportNotifications = m_pTimelineControl;
			if (pPluginSupportNotifications)
			{
				RETURN_IF_FAILED(pPluginSupportNotifications->OnShutdown());
			}
		}

		RETURN_IF_FAILED(m_pPluginSupport->OnShutdown());
		RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());

		m_pTimerService.Release();
		m_pSettings.Release();
		m_pTimelineService.Release();
		m_pTimelineControl.Release();
		m_pTheme.Release();
		m_pServiceProvider.Release();
		m_pServiceProviderParent.Release();
		m_pPluginSupport.Release();

		return S_OK;
	}

	STDMETHOD(GetTimelineControl)(ITimelineControl** ppTimelineControl)
	{
		CHECK_E_POINTER(ppTimelineControl);
		RETURN_IF_FAILED(m_pTimelineControl->QueryInterface(ppTimelineControl));
		return S_OK;
	}

	STDMETHOD(GetHWND)(HWND *hWnd)
	{
		T* pT = static_cast<T*>(this);
		CHECK_E_POINTER(hWnd);
		*hWnd = pT->m_hWnd;
		return S_OK;
	}

	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd)
	{
		T* pT = static_cast<T*>(this);
		CHECK_E_POINTER(hWnd);
		*hWnd = pT->Create(hWndParent, 0, 0, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_COMPOSITED | WS_EX_CONTROLPARENT);
		return S_OK;
	}

	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult)
	{
		RETURN_IF_FAILED(m_pTimelineControl->PreTranslateMessage(pMsg, pbResult));
		return S_OK;
	}

	STDMETHOD(OnActivate)()
	{
		RETURN_IF_FAILED(m_pTimelineControl->OnActivate());
		::SetFocus(m_hWndTimelineControl);
		return S_OK;
	}

	STDMETHOD(OnDeactivate)()
	{
		RETURN_IF_FAILED(m_pTimelineControl->OnDeactivate());
		return S_OK;
	}

	STDMETHOD(Load)(ISettings* pSettings)
	{
		CHECK_E_POINTER(pSettings);
		m_pSettings = pSettings;
		CComQIPtr<IInitializeWithSettings> p = m_pTimelineControl;
		if (p)
		{
			RETURN_IF_FAILED(p->Load(m_pSettings));
		}
		return S_OK;
	}

	STDMETHOD(SetTheme)(ITheme* pTheme)
	{
		CHECK_E_POINTER(pTheme);
		m_pTheme = pTheme;

		CComPtr<IImageManagerService> pImageManagerService;
		RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ImageManagerService, &pImageManagerService));
		CComPtr<ISkinTimeline> pSkinTimeline;
		RETURN_IF_FAILED(m_pTheme->GetTimelineSkin(&pSkinTimeline));
		RETURN_IF_FAILED(pSkinTimeline->SetImageManagerService(pImageManagerService));
		RETURN_IF_FAILED(m_pTimelineControl->SetSkinTimeline(pSkinTimeline));
		return S_OK;
	}

	STDMETHOD(OnCreate)(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TimelineControl, &m_pTimelineControl));
		RETURN_IF_FAILED(m_pTimelineControl->CreateEx(pT->m_hWnd, &m_hWndTimelineControl));
		AdjustSizes();
		return 0;
	}

	STDMETHOD(OnSize)(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		AdjustSizes();
		return 0;
	}

	void AdjustSizes()
	{
		T* pT = static_cast<T*>(this);
		CRect clientRect;
		pT->GetClientRect(&clientRect);
		::SetWindowPos(m_hWndTimelineControl, 0, 0, 0, clientRect.Width(), clientRect.Height(), 0);
	}

	LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (!m_pPluginSupport)
			return 0;

		T* pT = static_cast<T*>(this);
		CComPtr<IObjArray> pObjectArray;
		RETURN_IF_FAILED(m_pPluginSupport->GetPlugins(&pObjectArray));
		UINT cb = 0;
		RETURN_IF_FAILED(pObjectArray->GetCount(&cb));
		for (UINT i = 0; i < cb; i++)
		{
			CComPtr<IMsgHandler> pMsgHandler;
			HRESULT hr = pObjectArray->GetAt(i, IID_IMsgHandler, (LPVOID*)&pMsgHandler);
			if (hr == E_NOINTERFACE)
				continue;

			if (pMsgHandler)
			{
				LRESULT lResult = 0;
				BOOL bResult = FALSE;
				pMsgHandler->ProcessWindowMessage(pT->m_hWnd, uMsg, wParam, lParam, &lResult, &bResult);
				if (bResult)
					return bResult;
			}
		}
		return 0;
	}

	STDMETHOD(GetServiceProvider)(IServiceProvider** ppServiceProvider)
	{
		CHECK_E_POINTER(ppServiceProvider);
		RETURN_IF_FAILED(m_pServiceProvider->QueryInterface(ppServiceProvider));
		return S_OK;
	}
};

