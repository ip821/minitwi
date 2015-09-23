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

	RETURN_IF_FAILED(HrNotifyOnInitialized(m_pUserAccountControl, m_pPluginSupport));
	RETURN_IF_FAILED(HrNotifyOnInitialized(m_pTimelineControl, m_pPluginSupport));
	
	CComPtr<IThreadPoolService> pThreadPoolService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ThreadPoolService, &pThreadPoolService));
	RETURN_IF_FAILED(pThreadPoolService->SetThreadCount(6));
	RETURN_IF_FAILED(pThreadPoolService->Start());

	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE, &m_pTimelineService));
	RETURN_IF_FAILED(HrInitializeWithSettings(m_pPluginSupport, m_pSettings));

	RETURN_IF_FAILED(m_pPluginSupport->OnInitialized());
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_UPDATE_THREAD, &m_pThreadServiceUpdateTimeline));
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_GETUSER_THREAD, &m_pThreadServiceGetUser));

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	RETURN_IF_FAILED(AtlAdvise(m_pThreadServiceGetUser, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceGetUser));

	return S_OK;
}

STDMETHODIMP CUserInfoControl::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadServiceGetUser, __uuidof(IThreadServiceEventSink), m_dwAdviceGetUser));

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
	m_pThreadServiceGetUser.Release();
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

STDMETHODIMP CUserInfoControl::GetVariantObject(IVariantObject** ppVariantObject)
{
	CHECK_E_POINTER(ppVariantObject);
	RETURN_IF_FAILED(m_pVariantObject->QueryInterface(ppVariantObject));
	return S_OK;
}

STDMETHODIMP CUserInfoControl::SetVariantObject(IVariantObject *pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);

	CComVar vObjectType;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(ObjectModel::Metadata::Object::Type, &vObjectType));

	if (vObjectType.vt == VT_BSTR && CComBSTR(vObjectType.bstrVal) == Twitter::Connection::Metadata::UserObject::TypeId)
	{
		m_pVariantObject = pVariantObject;
		RETURN_IF_FAILED(LoadChildControls());
	}
	else
	{
		RETURN_IF_FAILED(m_pThreadServiceGetUser->Run());
	}

	return S_OK;
}

STDMETHODIMP CUserInfoControl::OnFinish(IVariantObject *pResult)
{
	CComVar vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));

	if (FAILED(vHr.intVal))
	{
		return S_OK;
	}

	CComVar vUserObject;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Connection::Metadata::TweetObject::UserObject, &vUserObject));
	ATLASSERT(vUserObject.vt == VT_UNKNOWN);
	CComQIPtr<IVariantObject> pUserObject = vUserObject.punkVal;
	m_pVariantObject = pUserObject;
	RETURN_IF_FAILED(LoadChildControls());
	return S_OK;
}

STDMETHODIMP CUserInfoControl::LoadChildControls()
{
	RETURN_IF_FAILED(HrInitializeWithVariantObject(m_pPluginSupport, m_pVariantObject));
	RETURN_IF_FAILED(HrInitializeWithVariantObject(m_pUserAccountControl, m_pVariantObject));
	RETURN_IF_FAILED(HrInitializeWithVariantObject(m_pTimelineControl, m_pVariantObject));
	RETURN_IF_FAILED(m_pThreadServiceUpdateTimeline->Run());
	return S_OK;
}

STDMETHODIMP CUserInfoControl::SetTheme(ITheme* pTheme)
{
	CHECK_E_POINTER(pTheme);
	m_pTheme = pTheme;

	CComQIPtr<IThemeSupport> pThemeSupport = m_pUserAccountControl;
	ATLASSERT(pThemeSupport);
	RETURN_IF_FAILED(pThemeSupport->SetTheme(m_pTheme));

	CComPtr<ISkinTimeline> pSkinTimeline;
	RETURN_IF_FAILED(m_pTheme->GetTimelineSkin(&pSkinTimeline));
	RETURN_IF_FAILED(pSkinTimeline->SetImageManagerService(m_pImageManagerService));
	RETURN_IF_FAILED(m_pTimelineControl->SetSkinTimeline(pSkinTimeline));
	return S_OK;
}

LRESULT CUserInfoControl::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	::SetFocus(m_hWndTimelineControl);
	return 0;
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

STDMETHODIMP CUserInfoControl::GetServiceProvider(IServiceProvider** ppServiceProvider)
{
	CHECK_E_POINTER(ppServiceProvider);
	RETURN_IF_FAILED(m_pServiceProvider->QueryInterface(ppServiceProvider));
	return S_OK;
}