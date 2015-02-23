#include "stdafx.h"
#include "TwitterStreamingConnection.h"
#include "..\model-libs\objmdl\StringUtils.h"
#include "oauthlib.h"
#include "TwitterConnection.h"

int CTwitterStreamingConnection::ProgressCallback(CTwitterStreamingConnection* pObj, double dltotal, double dlnow, double ultotal, double ulnow)
{
	if (!pObj->m_running)
		return 1;
	return 0;
}

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
	if (!m_running)
		return 0;

	if (data && size)
	{
		USES_CONVERSION;
		string buffer(data, size);
		m_callbackData.Append(CA2W(buffer.c_str()));
		vector<CString> messages;
		StrSplit(m_callbackData, L"\r\n", messages);
		if (messages.size() > 1 || m_callbackData[m_callbackData.GetLength() - 1] == L'\n')
		{
			if (m_callbackData[m_callbackData.GetLength() - 1] != L'\n')
			{
				m_callbackData = *(messages.end() - 1);
				messages.erase(messages.end() - 1);
			}
			else
			{
				m_callbackData.Empty();
			}

			CComPtr<IObjCollection> pTweetsCollection;
			ASSERT_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pTweetsCollection));
			bool bExists = false;
			for (auto it = messages.cbegin(); it != messages.cend(); it++)
			{
				auto& str = *it;
				auto value = shared_ptr<JSONValue>(JSON::Parse(str));
				if (value.get() == nullptr)
					continue;
				CComPtr<IVariantObject> pTweetObject;
				ASSERT_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pTweetObject));
				auto valueObject = value->AsObject();
				if (valueObject.find(L"id_str") != valueObject.end())
				{
					ASSERT_IF_FAILED(CTwitterConnection::ParseTweet(valueObject, pTweetObject));
					ASSERT_IF_FAILED(pTweetsCollection->AddObject(pTweetObject));
					bExists = true;
				}
			}

			if (bExists)
			{
				CComQIPtr<IObjArray> pObjArray = pTweetsCollection;
				Fire_OnMessages(pObjArray);
			}
		}
		return (int)size;
	}
	return 0;
}

STDMETHODIMP CTwitterStreamingConnection::StartStream(BSTR bstrKey, BSTR bstrSecret)
{
	m_running = true;

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
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
	curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);

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
	m_callbackData.Empty();
	return curlHr;
}

HRESULT CTwitterStreamingConnection::Fire_OnMessages(IObjArray* pObjectArray)
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
			hr = pConnection->OnMessages(pObjectArray);
		}
	}
	return hr;
}

STDMETHODIMP CTwitterStreamingConnection::StopStream()
{
	m_running = false;
	return S_OK;
}
