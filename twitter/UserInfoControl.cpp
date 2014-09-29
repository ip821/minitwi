#include "stdafx.h"
#include "UserInfoControl.h"
#include "Plugins.h"

#define USER_CONTROL_HEIGHT 240

CUserInfoControl::CUserInfoControl()
{
}

STDMETHODIMP CUserInfoControl::OnInitialized(IServiceProvider* pServiceProvider)
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_PluginSupport, &m_pPluginSupport));
	RETURN_IF_FAILED(m_pPluginSupport->InitializePlugins(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE));
	//RETURN_IF_FAILED(m_pPluginSupport->SetParentServiceProvider(pServiceProvider));
	m_pServiceProvider = m_pPluginSupport;
	ATLASSERT(m_pServiceProvider);

	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ImageManagerService, &m_pImageManagerService));

	CComPtr<IControl> pControl;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IControl), (LPVOID*)&pControl));

	RETURN_IF_FAILED(HrInitializeWithControl(m_pUserAccountControl, pControl));
	RETURN_IF_FAILED(HrInitializeWithControl(m_pTimelineControl, pControl));
	RETURN_IF_FAILED(HrInitializeWithControl(m_pPluginSupport, pControl));

	RETURN_IF_FAILED(HrNotifyOnInitialized(m_pUserAccountControl, m_pPluginSupport));
	RETURN_IF_FAILED(HrNotifyOnInitialized(m_pTimelineControl, m_pPluginSupport));
	
	CComPtr<IThreadPoolService> pThreadPoolService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ThreadPoolService, &pThreadPoolService));
	RETURN_IF_FAILED(pThreadPoolService->SetThreadCount(6));
	RETURN_IF_FAILED(pThreadPoolService->Start());

	CComPtr<ITimelineService> pTimelineService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_TimelineService, &pTimelineService));
	RETURN_IF_FAILED(pTimelineService->SetTimelineControl(m_pTimelineControl));
	RETURN_IF_FAILED(HrInitializeWithSettings(pTimelineService, m_pSettings));

	CComPtr<ITimelineImageService> pTimelineImageService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_TimelineImageService, &pTimelineImageService));
	RETURN_IF_FAILED(pTimelineImageService->SetTimelineControl(m_pTimelineControl));

	RETURN_IF_FAILED(m_pPluginSupport->OnInitialized());

	CComPtr<IThreadService> pThreadServiceUpdateTimeline;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &pThreadServiceUpdateTimeline));
	RETURN_IF_FAILED(pThreadServiceUpdateTimeline->SetTimerService(SERVICE_TIMELINE_TIMER));

	CComPtr<ITimerService> pTimerService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_TIMER, &pTimerService));

	RETURN_IF_FAILED(pTimerService->StartTimer(60 * 1000));

	return S_OK;
}

STDMETHODIMP CUserInfoControl::OnShutdown()
{
	return S_OK;
}

LRESULT CUserInfoControl::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if (!m_pPluginSupport)
		return 0;

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
			pMsgHandler->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, &lResult, &bResult);
			if (bResult)
				return bResult;
		}
	}
	return 0;
}

LRESULT CUserInfoControl::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_UserAccountControl, &m_pUserAccountControl));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TimelineControl, &m_pTimelineControl));
	RETURN_IF_FAILED(m_pUserAccountControl->CreateEx(m_hWnd, &m_hWndUserAccountControl));
	RETURN_IF_FAILED(m_pTimelineControl->CreateEx(m_hWnd, &m_hWndTimelineControl));

	AdjustSizes();
	return 0;
}

LRESULT CUserInfoControl::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	AdjustSizes();
	return 0;
}

void CUserInfoControl::AdjustSizes()
{
	CRect clientRect;
	GetClientRect(&clientRect);
	if (clientRect.Height() < USER_CONTROL_HEIGHT + 1)
		return;

	::SetWindowPos(m_hWndUserAccountControl, 0, 0, 0, clientRect.Width(), USER_CONTROL_HEIGHT, 0);
	::SetWindowPos(m_hWndTimelineControl, 0, 0, USER_CONTROL_HEIGHT, clientRect.Width(), clientRect.Height() - USER_CONTROL_HEIGHT, 0);
}

STDMETHODIMP CUserInfoControl::GetHWND(HWND *hWnd)
{
	CHECK_E_POINTER(hWnd);
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CUserInfoControl::CreateEx(HWND hWndParent, HWND *hWnd)
{
	CHECK_E_POINTER(hWnd);
	*hWnd = Create(hWndParent, 0, 0, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_COMPOSITED | WS_EX_CONTROLPARENT);
	return S_OK;
}

STDMETHODIMP CUserInfoControl::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
	return S_OK;
}

STDMETHODIMP CUserInfoControl::SetVariantObject(IVariantObject *pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	m_pVariantObject = pVariantObject;
	RETURN_IF_FAILED(HrInitializeWithVariantObject(m_pUserAccountControl, m_pVariantObject));
	RETURN_IF_FAILED(HrInitializeWithVariantObject(m_pTimelineControl, m_pVariantObject));
	return S_OK;
}

STDMETHODIMP CUserInfoControl::SetTheme(ITheme* pTheme)
{
	CHECK_E_POINTER(pTheme);
	m_pTheme = pTheme;

	CComPtr<ISkinUserAccountControl> pSkinUserAccountControl;
	RETURN_IF_FAILED(pTheme->GetSkinUserAccountControl(&pSkinUserAccountControl));
	RETURN_IF_FAILED(pSkinUserAccountControl->SetImageManagerService(m_pImageManagerService));
	RETURN_IF_FAILED(m_pUserAccountControl->SetSkinUserAccountControl(pSkinUserAccountControl));

	CComPtr<ISkinTimeline> pSkinTimeline;
	RETURN_IF_FAILED(m_pTheme->GetTimelineSkin(&pSkinTimeline));
	RETURN_IF_FAILED(pSkinTimeline->SetImageManagerService(m_pImageManagerService));
	RETURN_IF_FAILED(m_pTimelineControl->SetSkinTimeline(pSkinTimeline));
	return S_OK;
}

STDMETHODIMP CUserInfoControl::OnActivate()
{
	RETURN_IF_FAILED(m_pTimelineControl->OnActivate());
	RETURN_IF_FAILED(m_pUserAccountControl->OnActivate());
	return S_OK;
}

STDMETHODIMP CUserInfoControl::OnDeactivate()
{
	RETURN_IF_FAILED(m_pTimelineControl->OnDeactivate());
	RETURN_IF_FAILED(m_pUserAccountControl->OnDeactivate());
	return S_OK;
}

STDMETHODIMP CUserInfoControl::Load(ISettings* pSettings)
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