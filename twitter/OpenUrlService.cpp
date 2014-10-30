#include "stdafx.h"
#include "OpenUrlService.h"
#include "Plugins.h"

STDMETHODIMP COpenUrlService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_WindowService, &m_pWindowService));

	CComQIPtr<IMainWindow> pMainWindow = m_pControl;
	CComPtr<IContainerControl> pContainerControl;
	RETURN_IF_FAILED(pMainWindow->GetContainerControl(&pContainerControl));
	m_pTabbedControl = pContainerControl;
	CComPtr<IControl> pControl;
	RETURN_IF_FAILED(m_pTabbedControl->GetPage(0, &pControl));
	CComQIPtr<IHomeTimeLineControl> pHomeTimeLineControl = pControl;
	CComQIPtr<ITimelineControlSupport> pTimelineControlSupport = pHomeTimeLineControl;
	ATLASSERT(pTimelineControlSupport);
	RETURN_IF_FAILED(pTimelineControlSupport->GetTimelineControl(&m_pTimelineControl));

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(AtlAdvise(m_pTimelineControl, pUnk, __uuidof(ITimelineControlEventSink), &m_dwAdviceTimelineControl));
	RETURN_IF_FAILED(AtlAdvise(m_pTabbedControl, pUnk, __uuidof(ITabbedControlEventSink), &m_dwAdviceTabbedControl));

	return S_OK;
}

STDMETHODIMP COpenUrlService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pTimelineControl, __uuidof(ITimelineControlEventSink), m_dwAdviceTimelineControl));
	RETURN_IF_FAILED(AtlUnadvise(m_pTabbedControl, __uuidof(ITabbedControlEventSink), m_dwAdviceTabbedControl));
	m_pWindowService.Release();
	m_pServiceProvider.Release();
	m_pTimelineControl.Release();
	m_pTabbedControl.Release();
	return S_OK;
}

STDMETHODIMP COpenUrlService::OnActivate(IControl *pControl)
{
	{
		CComQIPtr<ITwitViewControl> pTwitViewControl = pControl;
		CComQIPtr<ITimelineControlSupport> p = pTwitViewControl;
		if (pTwitViewControl && p)
		{
			ATLASSERT(!m_dwAdviceTimelineControlInTwitViewControl);
			CComPtr<ITimelineControl> pTimelineControl;
			RETURN_IF_FAILED(p->GetTimelineControl(&pTimelineControl));
			CComPtr<IUnknown> pUnk;
			RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
			RETURN_IF_FAILED(AtlAdvise(pTimelineControl, pUnk, __uuidof(ITimelineControlEventSink), &m_dwAdviceTimelineControlInTwitViewControl));
		}
	}
	{
		CComQIPtr<IUserInfoControl> pUserInfoControl = pControl;
		if (pUserInfoControl)
		{
			ATLASSERT(!m_dwAdviceTimelineControlInUserInfoControl);
			CComPtr<ITimelineControl> pTimelineControl;
			RETURN_IF_FAILED(pUserInfoControl->GetTimelineControl(&pTimelineControl));
			CComPtr<IUnknown> pUnk;
			RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
			RETURN_IF_FAILED(AtlAdvise(pTimelineControl, pUnk, __uuidof(ITimelineControlEventSink), &m_dwAdviceTimelineControlInUserInfoControl));
		}
	}
	return S_OK;
}

STDMETHODIMP COpenUrlService::OnDeactivate(IControl *pControl)
{
	CComQIPtr<ITwitViewControl> pTwitViewControl = pControl;
	CComQIPtr<ITimelineControlSupport> p = pTwitViewControl;
	if (pTwitViewControl && p)
	{
		ATLASSERT(m_dwAdviceTimelineControlInTwitViewControl);
		CComPtr<ITimelineControl> pTimelineControl;
		RETURN_IF_FAILED(p->GetTimelineControl(&pTimelineControl));
		RETURN_IF_FAILED(AtlUnadvise(pTimelineControl, __uuidof(ITimelineControlEventSink), m_dwAdviceTimelineControlInTwitViewControl));
		m_dwAdviceTimelineControlInTwitViewControl = 0;

		CComQIPtr<IPluginSupportNotifications> pPluginSupportNotifications = pTwitViewControl;
		if (pPluginSupportNotifications)
		{
			RETURN_IF_FAILED(pPluginSupportNotifications->OnShutdown());
			RETURN_IF_FAILED(m_pTabbedControl->RemovePage(pControl));
		}
		return S_OK;
	}

	CComQIPtr<IUserInfoControl> pUserInfoControl = pControl;
	if (pUserInfoControl)
	{
		ATLASSERT(m_dwAdviceTimelineControlInUserInfoControl);
		CComPtr<ITimelineControl> pTimelineControl;
		RETURN_IF_FAILED(pUserInfoControl->GetTimelineControl(&pTimelineControl));
		RETURN_IF_FAILED(AtlUnadvise(pTimelineControl, __uuidof(ITimelineControlEventSink), m_dwAdviceTimelineControlInUserInfoControl));
		m_dwAdviceTimelineControlInUserInfoControl = 0;

		CComQIPtr<IPluginSupportNotifications> pPluginSupportNotifications = pUserInfoControl;
		if (pPluginSupportNotifications)
		{
			RETURN_IF_FAILED(pPluginSupportNotifications->OnShutdown());
			RETURN_IF_FAILED(m_pTabbedControl->RemovePage(pControl));
		}
		return S_OK;
	}
	return S_OK;
}

STDMETHODIMP COpenUrlService::OnClose(IControl *pControl)
{
	return S_OK;
}

STDMETHODIMP COpenUrlService::OpenTwitView(IVariantObject* pVariantObject)
{
	CComPtr<IControl> pControl;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TwitViewControl, &pControl));
	RETURN_IF_FAILED(m_pTabbedControl->AddPage(pControl));

	CComPtr<IThemeService> pThemeService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ThemeService, &pThemeService));
	CComPtr<ITheme> pTheme;
	RETURN_IF_FAILED(pThemeService->GetCurrentTheme(&pTheme));
	CComQIPtr<IThemeSupport> pThemeSupport = pControl;
	if (pThemeSupport)
	{
		RETURN_IF_FAILED(pThemeSupport->SetTheme(pTheme));
	}

	RETURN_IF_FAILED(HrInitializeWithControl(pControl, m_pControl));
	RETURN_IF_FAILED(CopyImages(pControl));
	RETURN_IF_FAILED(HrInitializeWithVariantObject(pControl, pVariantObject));

	RETURN_IF_FAILED(m_pTabbedControl->ActivatePage(pControl));

	return S_OK;
}

STDMETHODIMP COpenUrlService::CopyImages(IControl* pControl)
{
	CComPtr<IControl> pCurrentControl;
	RETURN_IF_FAILED(m_pTabbedControl->GetCurrentPage(&pCurrentControl));
	ATLASSERT(pCurrentControl);
	CComQIPtr<IServiceProviderSupport> pCurrentControlServiceProviderSupport = pCurrentControl;
	ATLASSERT(pCurrentControlServiceProviderSupport);
	CComPtr<IServiceProvider> pCurrentControlServiceProvider;
	RETURN_IF_FAILED(pCurrentControlServiceProviderSupport->GetServiceProvider(&pCurrentControlServiceProvider));
	CComPtr<IImageManagerService> pCurrentControlImageService;
	RETURN_IF_FAILED(pCurrentControlServiceProvider->QueryService(CLSID_ImageManagerService, &pCurrentControlImageService));

	CComQIPtr<IServiceProviderSupport> pUserInfoServiceProviderSupport = pControl;
	ATLASSERT(pUserInfoServiceProviderSupport);
	CComPtr<IServiceProvider> pUserInfoServiceProvider;
	RETURN_IF_FAILED(pUserInfoServiceProviderSupport->GetServiceProvider(&pUserInfoServiceProvider));
	CComPtr<IImageManagerService> pUserInfoImageManagerService;
	RETURN_IF_FAILED(pUserInfoServiceProvider->QueryService(CLSID_ImageManagerService, &pUserInfoImageManagerService));

	RETURN_IF_FAILED(pCurrentControlImageService->CopyTo(pUserInfoImageManagerService));

	return S_OK;
}

STDMETHODIMP COpenUrlService::OpenUserInfo(IVariantObject* pVariantObject)
{
	CComPtr<IControl> pControl;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_UserInfoControl, &pControl));
	RETURN_IF_FAILED(m_pTabbedControl->AddPage(pControl));

	CComPtr<IThemeService> pThemeService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ThemeService, &pThemeService));
	CComPtr<ITheme> pTheme;
	RETURN_IF_FAILED(pThemeService->GetCurrentTheme(&pTheme));
	CComQIPtr<IThemeSupport> pThemeSupport = pControl;
	if (pThemeSupport)
	{
		RETURN_IF_FAILED(pThemeSupport->SetTheme(pTheme));
	}

	RETURN_IF_FAILED(HrInitializeWithControl(pControl, m_pControl));
	RETURN_IF_FAILED(CopyImages(pControl));

	CComVariant vUserObject;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_USER_OBJECT, &vUserObject));
	if (vUserObject.vt == VT_UNKNOWN)
	{
		CComQIPtr<IVariantObject> pObj = vUserObject.punkVal;
		ATLASSERT(pObj);
		RETURN_IF_FAILED(HrInitializeWithVariantObject(pControl, pObj));
	}

	RETURN_IF_FAILED(m_pTabbedControl->ActivatePage(pControl));

	return S_OK;
}

STDMETHODIMP COpenUrlService::OnItemDoubleClick(IVariantObject* pVariantObject)
{
	RETURN_IF_FAILED(OpenTwitView(pVariantObject));
	return S_OK;
}

STDMETHODIMP COpenUrlService::OnColumnClick(BSTR bstrColumnName, DWORD dwColumnIndex, IColumnRects* pColumnRects, IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(bstrColumnName);
	CHECK_E_POINTER(pVariantObject);

	CString strUrl;
	if (CComBSTR(bstrColumnName) == CComBSTR(VAR_TWITTER_USER_DISPLAY_NAME) ||
		CComBSTR(bstrColumnName) == CComBSTR(VAR_TWITTER_USER_NAME))
	{
		RETURN_IF_FAILED(OpenUserInfo(pVariantObject));
	}

	if (CComBSTR(bstrColumnName) == CComBSTR(VAR_TWITTER_RETWEETED_USER_DISPLAY_NAME))
	{
		CComVariant v;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_RETWEETED_USER_NAME, &v));
		if (v.vt == VT_BSTR)
		{
			strUrl = L"https://twitter.com/" + CString(v.bstrVal);
		}
	}

	if (CComBSTR(bstrColumnName) == CComBSTR(VAR_TWITTER_RELATIVE_TIME))
	{
		RETURN_IF_FAILED(OpenTwitView(pVariantObject));
	}

	if (CComBSTR(bstrColumnName) == CComBSTR(VAR_TWITTER_URL))
	{
		CComBSTR bstr;
		RETURN_IF_FAILED(pColumnRects->GetRectStringProp(dwColumnIndex, VAR_VALUE, &bstr));
		strUrl = bstr;
	}

	if (CComBSTR(bstrColumnName) == CComBSTR(VAR_TWITTER_USER_IMAGE))
	{
		CComBSTR bstr;
		RETURN_IF_FAILED(pColumnRects->GetRectStringProp(dwColumnIndex, VAR_VALUE, &bstr));
		CString strUrl(bstr);
		strUrl.Replace(L"_normal", L"");

		CComPtr<IVariantObject> pUrlObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pUrlObject));
		RETURN_IF_FAILED(pUrlObject->SetVariantValue(VAR_TWITTER_MEDIAURL, &CComVariant(strUrl)));

		RETURN_IF_FAILED(m_pWindowService->OpenWindow(m_hControlWnd, CLSID_PictureWindow, pUrlObject));
		return S_OK;
	}

	if (CComBSTR(bstrColumnName) == CComBSTR(VAR_TWITTER_IMAGE))
	{
		CComBSTR bstr;
		RETURN_IF_FAILED(pColumnRects->GetRectStringProp(dwColumnIndex, VAR_TWITTER_MEDIAURL, &bstr));

		CComPtr<IVariantObject> pUrlObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pUrlObject));
		RETURN_IF_FAILED(pUrlObject->SetVariantValue(VAR_TWITTER_MEDIAURL, &CComVariant(bstr)));

		CComVariant vMediaUrls;
		if (SUCCEEDED(pVariantObject->GetVariantValue(VAR_TWITTER_MEDIAURLS, &vMediaUrls)))
		{
			RETURN_IF_FAILED(pUrlObject->SetVariantValue(VAR_TWITTER_MEDIAURLS, &vMediaUrls));
		}

		RETURN_IF_FAILED(m_pWindowService->OpenWindow(m_hControlWnd, CLSID_PictureWindow, pUrlObject));
		return S_OK;
	}

	if (!strUrl.IsEmpty())
		ShellExecute(NULL, L"open", strUrl, NULL, NULL, SW_SHOW);

	return S_OK;
}
