// DownloadService.cpp : Implementation of CDownloadService

#include "stdafx.h"
#include "DownloadService.h"
#include "Plugins.h"
#include "..\ObjMdl\StringUtils.h"

#include <curl/curl.h>
#include <curl/easy.h>
#include <boost/filesystem.hpp>
#include <Winhttp.h>

// CDownloadService

STDMETHODIMP CDownloadService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ThreadPoolService, &m_pThreadPoolService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadPoolService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));
	return S_OK;
}

STDMETHODIMP CDownloadService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadPoolService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	m_pThreadPoolService.Release();
	return S_OK;
}

STDMETHODIMP CDownloadService::OnStart(IVariantObject *pResult)
{
	return S_OK;
}

STDMETHODIMP CDownloadService::OnRun(IVariantObject *pResult)
{
	CoInitialize(NULL);
	CURL* curl = curl_easy_init();
	if (!curl)
		return E_FAIL;

	CComVariant vUrl;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_URL, &vUrl));
	if (vUrl.vt != VT_BSTR)
		return E_INVALIDARG;

	CString strTempFolder;
	StrGetTempPath(strTempFolder);
	GUID guid = { 0 };
	CoCreateGuid(&guid);
	CString strGuid;
	StrGuidToString(guid, strGuid);
	StrPathAppend(strTempFolder, strGuid);
	strTempFolder += L".img";

	USES_CONVERSION;

	auto file = fopen(W2A(strTempFolder), "wb");

	char errorBuffer[1024] = { 0 };

	auto res = curl_easy_setopt(curl, CURLOPT_URL, W2A(vUrl.bstrVal));
	res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	res = curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1);
	res = curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, NULL);
	res = curl_easy_setopt(curl, CURLOPT_PROXYAUTH, (long)CURLAUTH_ANY);
	res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
	res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
	res = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	res = curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);

	WINHTTP_CURRENT_USER_IE_PROXY_CONFIG proxyConfig = { 0 };
	if (WinHttpGetIEProxyConfigForCurrentUser(&proxyConfig))
	{
		std::vector<CString> proxies;
		StrSplit(proxyConfig.lpszProxy, L";", proxies);
		for (auto it = proxies.begin(); it != proxies.end(); it++)
		{
			std::vector<CString> values;
			StrSplit(*it, L"=", values);
			if (values.size() == 2 && values[0] == L"http")
			{
				res = curl_easy_setopt(curl, CURLOPT_PROXY, W2A(values[1]));
				break;
			}
		}
	}

	res = curl_easy_perform(curl);
	
	if (res != CURLcode::CURLE_OK)
	{
		curl_easy_cleanup(curl);
		fclose(file);
		return E_FAIL;
	}

	curl_easy_cleanup(curl);
	fclose(file);

	pResult->SetVariantValue(VAR_FILEPATH, &CComVariant(strTempFolder));

	return S_OK;
}

STDMETHODIMP CDownloadService::OnFinish(IVariantObject *pResult)
{
	CComVariant vUrl;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_URL, &vUrl));
	if (vUrl.vt != VT_BSTR)
		return S_OK;

	CComVariant vFilePath;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_FILEPATH, &vFilePath));
	if (vFilePath.vt != VT_BSTR)
		return S_OK;

	RETURN_IF_FAILED(Fire_OnDownloadComplete(pResult));
	DeleteFile(vFilePath.bstrVal);
	return S_OK;
}

STDMETHODIMP CDownloadService::AddDownload(IVariantObject* pVariantObject)
{
	RETURN_IF_FAILED(m_pThreadPoolService->AddTask(pVariantObject));
	return S_OK;
}

HRESULT CDownloadService::Fire_OnDownloadComplete(IVariantObject *pResult)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	HRESULT hr = S_OK;
	CDownloadService* pThis = static_cast<CDownloadService*>(this);
	int cConnections = m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
		pThis->Unlock();

		IDownloadServiceEventSink * pConnection = static_cast<IDownloadServiceEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnDownloadComplete(pResult);
		}
	}
	return hr;
}