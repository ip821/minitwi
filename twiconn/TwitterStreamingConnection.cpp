#include "stdafx.h"
#include "TwitterStreamingConnection.h"
#include "..\model-libs\objmdl\StringUtils.h"
#include "oauthlib.h"

size_t CTwitterStreamingConnection::WriteCallback(char *ptr, size_t size, size_t nmemb, CTwitterStreamingConnection* pObj)
{
	if (pObj && ptr)
	{
		return pObj->SaveLastWebResponse(ptr, (size*nmemb));
	}
	return 0;
}

int CTwitterStreamingConnection::SaveLastWebResponse(char*& data, size_t size)
{
	if (data && size)
	{
		m_callbackData.append(data, size);
		return (int)size;
	}
	return 0;
}

STDMETHODIMP CTwitterStreamingConnection::StartStream(BSTR bstrKey, BSTR bstrSecret)
{
	CURL* curl = curl_easy_init();
	if (!curl)
		return E_FAIL;

	const string strQuery("https://userstream.twitter.com/1.1/user.json");

	char errorBuffer[1024] = { 0 };

	USES_CONVERSION;

	curl_easy_setopt(curl, CURLOPT_URL, strQuery.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1);
	curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, NULL);
	curl_easy_setopt(curl, CURLOPT_PROXYAUTH, (long)CURLAUTH_ANY);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL);
	curl_easy_setopt(curl, CURLOPT_ENCODING, "");
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

	string dataStrDummy;
	string oAuthHttpHeader;
	oAuth oAuth;

	oAuth.setConsumerKey(string(APP_KEY));
	oAuth.setConsumerSecret(string(APP_SECRET));

	oAuth.setOAuthTokenKey(string(CW2A(bstrKey)));
	oAuth.setOAuthTokenSecret(string(CW2A(bstrSecret)));

	struct curl_slist* pHeaderList = NULL;
	pHeaderList = curl_slist_append(pHeaderList, "User-Agent: minitwi");

	oAuth.getOAuthHeader(eOAuthHttpGet, strQuery, dataStrDummy, oAuthHttpHeader);
	if (oAuthHttpHeader.length())
	{
		curl_slist_append(pHeaderList, oAuthHttpHeader.c_str());
	}

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, pHeaderList);

	{
		WINHTTP_CURRENT_USER_IE_PROXY_CONFIG proxyConfig = { 0 };
		if (WinHttpGetIEProxyConfigForCurrentUser(&proxyConfig))
		{
			vector<CString> proxies;
			StrSplit(proxyConfig.lpszProxy, L";", proxies);
			for (auto it = proxies.begin(); it != proxies.end(); it++)
			{
				vector<CString> values;
				StrSplit(*it, L"=", values);
				if (values.size() == 2 && values[0] == L"http")
				{
					string strValue = (CW2A(values[1]));
					curl_easy_setopt(curl, CURLOPT_PROXY, strValue.c_str());
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

	auto res = curl_easy_perform(curl);
	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

	HRESULT curlHr = S_OK;

	if (curlHr == S_OK)
	{
		switch (res)
		{
		case CURLE_COULDNT_CONNECT:
			curlHr = HRESULT_FROM_WIN32(ERROR_NETWORK_UNREACHABLE);
		}
	}

	if (curlHr == S_OK)
	{
		switch (http_code)
		{
		case 401:
			curlHr = COMADMIN_E_USERPASSWDNOTVALID;
		}
	}

	if (pHeaderList)
	{
		curl_slist_free_all(pHeaderList);
	}

	curl_easy_cleanup(curl);
	m_callbackData.clear();
	return curlHr;
}

HRESULT CTwitterStreamingConnection::Fire_OnMessages(IObjCollection* pObjCollection)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	HRESULT hr = S_OK;
	CTwitterStreamingConnection* pThis = static_cast<CTwitterStreamingConnection*>(this);
	int cConnections = m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
		pThis->Unlock();

		ITwitterStreamingConnectionEventSink * pConnection = static_cast<ITwitterStreamingConnectionEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnMessages(pObjCollection);
		}
	}
	return hr;
}
