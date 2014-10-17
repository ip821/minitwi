#include "stdafx.h"
#include "UserInfoControl.h"
#include "Plugins.h"

#define USER_CONTROL_HEIGHT 240

CUserInfoControl::CUserInfoControl()
{
}

HRESULT CUserInfoControl::FinalConstruct()
{
	return S_OK;
}

void CUserInfoControl::FinalRelease()
{
	if (m_hWnd)
		DestroyWindow();
}

STDMETHODIMP CUserInfoControl::OnInitialized(IServiceProvider* pServiceProvider)
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_PluginSupport, &m_pPluginSupport));
	RETURN_IF_FAILED(m_pPluginSupport->InitializePlugins(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE));

	m_pServiceProviderParent = pServiceProvider;
	RETURN_IF_FAILED(m_pPluginSupport->SetParentServiceProvider(m_pServiceProviderParent));
	m_pServiceProvider = m_pPluginSupport;
	ATLASSERT(m_pServiceProvider);

	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ImageManagerService, &m_pImageManagerService));
	
	CComPtr<IControl> pControl;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IControl), (LPVOID*)&pControl));

	RETURN_IF_FAILED(HrInitializeWithControl(m_pUserAccountControl, pControl));
	RETURN_IF_FAILED(HrInitializeWithControl(m_pTimelineControl, pControl));
	RETURN_IF_FAILED(HrInitializeWithControl(m_pPluginSupport, pControl));

	CComPtr<IInitializeWithControl> pTabbedControlStatusService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_TabbedControlStatusService, &pTabbedControlStatusService));
	RETURN_IF_FAILED(pTabbedControlStatusService->SetControl(m_pControl));

	RETURN_IF_FAILED(HrNotifyOnInitialized(m_pUserAccountControl, m_pPluginSupport));
	RETURN_IF_FAILED(HrNotifyOnInitialized(m_pTimelineControl, m_pPluginSupport));
	
	CComPtr<IThreadPoolService> pThreadPoolService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ThreadPoolService, &pThreadPoolService));
	RETURN_IF_FAILED(pThreadPoolService->SetThreadCount(6));
	RETURN_IF_FAILED(pThreadPoolService->Start());

	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_TimelineService, &m_pTimelineService));
	RETURN_IF_FAILED(m_pTimelineService->SetTimelineControl(m_pTimelineControl));
	RETURN_IF_FAILED(HrInitializeWithSettings(m_pTimelineService, m_pSettings));

	CComPtr<ITimelineImageService> pTimelineImageService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_TimelineImageService, &pTimelineImageService));
	RETURN_IF_FAILED(pTimelineImageService->SetTimelineControl(m_pTimelineControl));

	RETURN_IF_FAILED(m_pPluginSupport->OnInitialized());
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &m_pThreadServiceUpdateTimeline));

	return S_OK;
}

STDMETHODIMP CUserInfoControl::OnShutdown()
{
	{
		CComQIPtr <IPluginSupportNotifications> pPluginSupportNotifications = m_pTimelineControl;
		if (pPluginSupportNotifications)
		{
			RETURN_IF_FAILED(pPluginSupportNotifications->OnShutdown());
		}
	}

	{
		CComQIPtr <IPluginSupportNotifications> pPluginSupportNotifications = m_pUserAccountControl;
		if (pPluginSupportNotifications)
		{
			RETURN_IF_FAILED(pPluginSupportNotifications->OnShutdown());
		}
	}


	RETURN_IF_FAILED(m_pPluginSupport->OnShutdown());
	m_pPluginSupport.Release();
	m_pServiceProviderParent.Release();
	m_pServiceProvider.Release();
	m_pTheme.Release();
	m_pVariantObject.Release();
	m_pUserAccountControl.Release();
	m_pTimelineControl.Release();
	m_pImageManagerService.Release();
	m_pSettings.Release();
	m_pTimelineService.Release();
	m_pThreadServiceUpdateTimeline.Release();
	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());

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
	RETURN_IF_FAILED(m_pTimelineControl->PreTranslateMessage(pMsg, pbResult));
	return S_OK;
}

STDMETHODIMP CUserInfoControl::SetVariantObject(IVariantObject *pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	m_pVariantObject = pVariantObject;

	CComPtr<IImageManagerService> pParentImageService;
	RETURN_IF_FAILED(m_pServiceProviderParent->QueryService(CLSID_ImageManagerService, &pParentImageService));
	if (pParentImageService)
	{
		CComVariant vUserImage;
		RETURN_IF_FAILED(m_pVariantObject->GetVariantValue(VAR_TWITTER_USER_IMAGE, &vUserImage));
		if (vUserImage.vt == VT_BSTR)
		{
			BOOL bContains = FALSE;
			RETURN_IF_FAILED(pParentImageService->ContainsImageKey(vUserImage.bstrVal, &bContains));
			if (bContains)
			{
				RETURN_IF_FAILED(pParentImageService->CopyImageTo(vUserImage.bstrVal, m_pImageManagerService));
			}
		}
	}

	RETURN_IF_FAILED(HrInitializeWithVariantObject(m_pUserAccountControl, m_pVariantObject));
	RETURN_IF_FAILED(HrInitializeWithVariantObject(m_pTimelineControl, m_pVariantObject));

	CComVariant vUserScreenName;
	RETURN_IF_FAILED(m_pVariantObject->GetVariantValue(VAR_TWITTER_USER_NAME, &vUserScreenName));
	ATLASSERT(vUserScreenName.vt == VT_BSTR);

	RETURN_IF_FAILED(m_pTimelineService->SetUserId(vUserScreenName.bstrVal));
	RETURN_IF_FAILED(m_pThreadServiceUpdateTimeline->Run());

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
	::SetFocus(m_hWndTimelineControl);
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

STDMETHODIMP CUserInfoControl::GetTimelineControl(ITimelineControl** ppTimelineControl)
{
	CHECK_E_POINTER(ppTimelineControl);
	RETURN_IF_FAILED(m_pTimelineControl->QueryInterface(ppTimelineControl));
	return S_OK;
}
