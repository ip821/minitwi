#include "stdafx.h"
#include "OpenUrlService.h"
#include "Plugins.h"

STDMETHODIMP COpenUrlService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_WindowService, &m_pWindowService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_FORMS_SERVICE, &m_pFormsService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_FORM_MANAGER, &m_pFormManager));

	m_pMainWindow = m_pControl;
	ATLASSERT(m_pMainWindow);
	RETURN_IF_FAILED(m_pMainWindow->GetMessageLoop(&m_pMessageLoop));
	RETURN_IF_FAILED(m_pMessageLoop->AddMessageFilter(this));

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	RETURN_IF_FAILED(AtlAdvise(m_pFormManager, pUnk, __uuidof(IFormManagerEventSink), &m_dwAdviceFormManager));

	{
		CComPtr<IControl> pControl;
		RETURN_IF_FAILED(m_pFormManager->FindForm(CLSID_HomeTimeLineControl, &pControl));

		CComQIPtr<ITimelineControlSupport> pTimelineControlSupport = pControl;
		ATLASSERT(pTimelineControlSupport);
		RETURN_IF_FAILED(pTimelineControlSupport->GetTimelineControl(&m_pHomeTimelineControl));

		RETURN_IF_FAILED(AtlAdvise(m_pHomeTimelineControl, pUnk, __uuidof(ITimelineControlEventSink), &m_dwAdviceHomeTimelineControl));
	}

	{
		{
			CComPtr<IControl> pControl;
			RETURN_IF_FAILED(m_pFormManager->FindForm(CLSID_SearchControl, &pControl));

			CComQIPtr<ITimelineControlSupport> pTimelineControlSupport = pControl;
			ATLASSERT(pTimelineControlSupport);
			RETURN_IF_FAILED(pTimelineControlSupport->GetTimelineControl(&m_pSearchTimelineControl));

			RETURN_IF_FAILED(AtlAdvise(m_pSearchTimelineControl, pUnk, __uuidof(ITimelineControlEventSink), &m_dwAdviceSearchTimelineControl));
		}
	}
	return S_OK;
}

STDMETHODIMP COpenUrlService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pSearchTimelineControl, __uuidof(ITimelineControlEventSink), m_dwAdviceSearchTimelineControl));
	RETURN_IF_FAILED(AtlUnadvise(m_pHomeTimelineControl, __uuidof(ITimelineControlEventSink), m_dwAdviceHomeTimelineControl));
	RETURN_IF_FAILED(AtlUnadvise(m_pFormManager, __uuidof(IFormManagerEventSink), m_dwAdviceFormManager));
	RETURN_IF_FAILED(m_pMessageLoop->RemoveMessageFilter(this));
	m_pWindowService.Release();
	m_pServiceProvider.Release();
	m_pHomeTimelineControl.Release();
	m_pFormsService.Release();
	m_pFormManager.Release();
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
		RETURN_IF_FAILED(m_pFormsService->Close(pControl));
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
		RETURN_IF_FAILED(m_pFormsService->Close(pControl));
		return S_OK;
	}
	return S_OK;
}

STDMETHODIMP COpenUrlService::OpenUserInfoForm(IVariantObject* pVariantObject)
{
	CComPtr<IControl> pControl;
	HRESULT hr = m_pFormManager->FindForm(CLSID_UserInfoControl, &pControl);
	if (SUCCEEDED(hr))
	{
		CComQIPtr<IUserInfoControl> pUserInfoControl = pControl;
		ATLASSERT(pUserInfoControl);
		CComPtr<IVariantObject> pVariantObjectOpened;
		RETURN_IF_FAILED(pUserInfoControl->GetVariantObject(&pVariantObjectOpened));
		CComVariant vUserNameOpened;
		RETURN_IF_FAILED(pVariantObjectOpened->GetVariantValue(VAR_TWITTER_USER_NAME, &vUserNameOpened));
		CComVariant vUserName;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_USER_NAME, &vUserName));
		if (vUserName.vt == VT_BSTR && vUserNameOpened.vt == VT_BSTR && CComBSTR(vUserName.bstrVal) == CComBSTR(vUserNameOpened.bstrVal))
		{
			return S_OK;
		}
	}

	RETURN_IF_FAILED(m_pFormsService->OpenForm(CLSID_UserInfoControl, pVariantObject));
	return S_OK;
}

STDMETHODIMP COpenUrlService::OpenTwitViewForm(IVariantObject* pVariantObject)
{
	CComPtr<IControl> pControl;
	HRESULT hr = m_pFormManager->FindForm(CLSID_TwitViewControl, &pControl);
	if (SUCCEEDED(hr))
	{
		CComQIPtr<ITwitViewControl> pTwitViewControl = pControl;
		ATLASSERT(pTwitViewControl);
		CComPtr<IVariantObject> pVariantObjectOpened;
		RETURN_IF_FAILED(pTwitViewControl->GetVariantObject(&pVariantObjectOpened));
		CComVariant vIdOpened;
		RETURN_IF_FAILED(pVariantObjectOpened->GetVariantValue(VAR_ID, &vIdOpened));
		CComVariant vId;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_ID, &vId));
		if (vId.vt == VT_BSTR && vIdOpened.vt == VT_BSTR && CComBSTR(vId.bstrVal) == CComBSTR(vIdOpened.bstrVal))
		{
			return S_OK;
		}
	}
	RETURN_IF_FAILED(m_pFormsService->OpenForm(CLSID_TwitViewControl, pVariantObject));
	return S_OK;
}

STDMETHODIMP COpenUrlService::OnItemDoubleClick(IVariantObject* pVariantObject)
{
	RETURN_IF_FAILED(OpenTwitViewForm(pVariantObject));
	return S_OK;
}

STDMETHODIMP COpenUrlService::OnColumnClick(IColumnsInfoItem* pColumnsInfoItem, IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	CHECK_E_POINTER(pColumnsInfoItem);

	CComBSTR bstrColumnName;
	RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(VAR_COLUMN_NAME, &bstrColumnName));

	CString strUrl;
	if (CComBSTR(bstrColumnName) == CComBSTR(VAR_TWITTER_USER_DISPLAY_NAME) ||
		CComBSTR(bstrColumnName) == CComBSTR(VAR_TWITTER_USER_NAME))
	{
		CComVariant vUserObject;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_USER_OBJECT, &vUserObject));
		if (vUserObject.vt == VT_UNKNOWN)
		{
			CComQIPtr<IVariantObject> pObj = vUserObject.punkVal;
			ATLASSERT(pObj);
			RETURN_IF_FAILED(OpenUserInfoForm(pObj));
		}
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
		RETURN_IF_FAILED(OpenTwitViewForm(pVariantObject));
	}

	if (CComBSTR(bstrColumnName) == CComBSTR(VAR_TWITTER_URL))
	{
		CComBSTR bstr;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(VAR_VALUE, &bstr));
		strUrl = bstr;
	}

	if (CComBSTR(bstrColumnName) == CComBSTR(VAR_TWITTER_USER_IMAGE))
	{
		CComBSTR bstr;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(VAR_VALUE, &bstr));
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
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(VAR_TWITTER_MEDIAURL, &bstr));

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

STDMETHODIMP COpenUrlService::PreTranslateMessage(MSG *pMsg, BOOL *bResult)
{
	HWND hWnd = 0;
	RETURN_IF_FAILED(m_pControl->GetHWND(&hWnd));
	if (pMsg->message == WM_KEYDOWN && GetActiveWindow() == hWnd)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			CComPtr<IControl> pControlHome;
			CComPtr<IFormManager> pFormManager;
			RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_FORM_MANAGER, &pFormManager));
			RETURN_IF_FAILED(pFormManager->ActivateForm(CLSID_HomeTimeLineControl));
		}
	}
	return 0;
}