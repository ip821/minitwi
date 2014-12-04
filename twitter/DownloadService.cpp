// DownloadService.cpp : Implementation of CDownloadService

#include "stdafx.h"
#include "DownloadService.h"
#include "Plugins.h"
#include "..\ObjMdl\StringUtils.h"

#include <curl/curl.h>
#include <curl/easy.h>
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
	CURL* curl = curl_easy_init();
	if (!curl)
		return E_FAIL;

	HRESULT hr = S_OK;
	CComVariant vUrl;
	if (FAILED(hr = pResult->GetVariantValue(VAR_URL, &vUrl)) || vUrl.vt != VT_BSTR)
	{
		curl_easy_cleanup(curl);
		return E_INVALIDARG;
	}

	CString strTempFolder;
	StrGetTempPath(strTempFolder);
	GUID guid = { 0 };
	if (FAILED(hr = CoCreateGuid(&guid)))
	{
		curl_easy_cleanup(curl);
		return hr;
	}

	CString strGuid;
	StrGuidToString(guid, strGuid);
	StrPathAppend(strTempFolder, strGuid);

	CComVariant vExt;
	pResult->GetVariantValue(VAR_FILE_EXT, &vExt);
	if (vExt.vt == VT_BSTR)
	{
		strTempFolder += vExt.bstrVal;
	}
	else
	{
		strTempFolder += L".img";
	}

	USES_CONVERSION;

	string strTempFolderA = CW2A(strTempFolder);
	auto file = fopen(strTempFolderA.c_str(), "wb");

	char errorBuffer[1024] = { 0 };

	string strUrl(CW2A(vUrl.bstrVal));
	auto res = curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	res = curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1);
	res = curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, NULL);
	res = curl_easy_setopt(curl, CURLOPT_PROXYAUTH, (long)CURLAUTH_ANY);
	res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
	res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
	res = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	res = curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);

	{
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
					string strValue = (CW2A(values[1]));
					res = curl_easy_setopt(curl, CURLOPT_PROXY, strValue.c_str());
					break;
				}
			}
			if (proxyConfig.lpszAutoConfigUrl)
				GlobalFree(proxyConfig.lpszAutoConfigUrl);
			if (proxyConfig.lpszProxy)
				GlobalFree(proxyConfig.lpszProxy);
			if (proxyConfig.lpszProxyBypass)
				GlobalFree(proxyConfig.lpszProxyBypass);
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
	ATLENSURE(vUrl.vt == VT_BSTR);
	if (vUrl.vt == VT_BSTR)
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		wstring strUrl(vUrl.bstrVal);
		ATLENSURE(m_urls.size());
		m_urls.erase(strUrl);
	}

	CComVariant vFilePath;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_FILEPATH, &vFilePath));
	if (vFilePath.vt == VT_BSTR)
	{
		RETURN_IF_FAILED(Fire_OnDownloadComplete(pResult));
		CComVariant vKeepFile;
		RETURN_IF_FAILED(pResult->GetVariantValue(VAR_KEEP_FILE, &vKeepFile));
		if (vKeepFile.vt != VT_BOOL || !vKeepFile.boolVal)
		{
			DeleteFile(vFilePath.bstrVal);
		}
	}
	return S_OK;
}

STDMETHODIMP CDownloadService::AddDownload(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	CComVariant vUrl;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_URL, &vUrl));
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		ATLENSURE(vUrl.vt == VT_BSTR);
		if (vUrl.vt == VT_BSTR)
		{
			wstring strUrl(vUrl.bstrVal);
			if (m_urls.find(strUrl) != m_urls.end())
				return S_OK;
			auto result = m_urls.insert(strUrl);
			ATLENSURE(result.second);
		}
	}
	RETURN_IF_FAILED(m_pThreadPoolService->AddTask(pVariantObject));
	return S_OK;
}

STDMETHODIMP CDownloadService::SuspendDownloads()
{
	RETURN_IF_FAILED(m_pThreadPoolService->Suspend());
	return S_OK;
}

STDMETHODIMP CDownloadService::ResumeDownloads()
{
	RETURN_IF_FAILED(m_pThreadPoolService->Resume());
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
