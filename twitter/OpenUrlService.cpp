#include "stdafx.h"
#include "OpenUrlService.h"
#include "Plugins.h"

STDMETHODIMP COpenUrlService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_WindowService, &m_pWindowService));
	return S_OK;
}

STDMETHODIMP COpenUrlService::OnShutdown()
{
	m_pWindowService.Release();
	m_pServiceProvider.Release();
	return S_OK;
}

STDMETHODIMP COpenUrlService::OpenColumnAsUrl(BSTR bstrColumnName, DWORD dwColumnIndex, IColumnRects* pColumnRects, IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(bstrColumnName);
	CHECK_E_POINTER(pVariantObject);

	CString strUrl;
	if (CComBSTR(bstrColumnName) == CComBSTR(VAR_TWITTER_USER_DISPLAY_NAME) ||
		CComBSTR(bstrColumnName) == CComBSTR(VAR_TWITTER_USER_NAME))
	{
		CComVariant v;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_USER_NAME, &v));
		if (v.vt == VT_BSTR)
		{
			strUrl = L"https://twitter.com/" + CString(v.bstrVal);
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
		return S_OK;
		CComVariant vUserName;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_USER_NAME, &vUserName));
		CComVariant v;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_ID, &v));
		if (vUserName.vt == VT_BSTR && v.vt == VT_BSTR)
		{
			strUrl = L"https://twitter.com/" + CString(vUserName.bstrVal) + L"/status/" + CString(v.bstrVal);
		}
	}

	if (CComBSTR(bstrColumnName) == CComBSTR(VAR_TWITTER_URL))
	{
		CComBSTR bstr;
		RETURN_IF_FAILED(pColumnRects->GetRectProp(dwColumnIndex, VAR_VALUE, &bstr));
		strUrl = bstr;
	}

	if (CComBSTR(bstrColumnName) == CComBSTR(VAR_TWITTER_IMAGE))
	{
		{
			CComBSTR bstr;
			RETURN_IF_FAILED(pColumnRects->GetRectProp(dwColumnIndex, VAR_TWITTER_MEDIAURL, &bstr));

			CComPtr<IVariantObject> pUrlObject;
			RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pUrlObject));
			RETURN_IF_FAILED(pUrlObject->SetVariantValue(VAR_TWITTER_MEDIAURL, &CComVariant(bstr)));

			RETURN_IF_FAILED(m_pWindowService->OpenWindow(m_hControlWnd, CLSID_PictureWindow, pUrlObject));
		}
		return S_OK;
	}

	if (!strUrl.IsEmpty())
		ShellExecute(NULL, L"open", strUrl, NULL, NULL, SW_SHOW);

	return S_OK;
}