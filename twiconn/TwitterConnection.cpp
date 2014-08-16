// TwitterConnection.cpp : Implementation of CTwitterConnection

#include "stdafx.h"
#include "TwitterConnection.h"
#include "Plugins.h"
#include <hash_set>

// CTwitterConnection

#define APP_KEY "9FA16n89DvlcZCO07IkVHcxio"
#define APP_SECRET "stU8OFXzqG9dsp80fnH92d9iSsm11WpPIWnvctKYB7zpcz7WqX"

STDMETHODIMP CTwitterConnection::HandleError(JSONValue* value)
{
	if (value->IsObject())
	{
		auto valueObject = value->AsObject();
		if (valueObject.find(L"errors") != valueObject.end())
			return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CTwitterConnection::GetAuthKeys(BSTR bstrUser, BSTR bstrPass, BSTR* pbstrKey, BSTR* pbstrSecret)
{
	twitCurl twitterObj;

	USES_CONVERSION;

	twitterObj.setTwitterUsername(std::string(W2A(bstrUser)));
	twitterObj.setTwitterPassword(std::string(W2A(bstrPass)));

	twitterObj.getOAuth().setConsumerKey(std::string(APP_KEY));
	twitterObj.getOAuth().setConsumerSecret(std::string(APP_SECRET));

	std::string authUrl;
	twitterObj.oAuthRequestToken(authUrl);

	twitterObj.oAuthHandlePIN(authUrl);
	twitterObj.oAuthAccessToken();

	std::string myOAuthAccessTokenKey;
	std::string myOAuthAccessTokenSecret;

	twitterObj.getOAuth().getOAuthTokenKey(myOAuthAccessTokenKey);
	twitterObj.getOAuth().getOAuthTokenSecret(myOAuthAccessTokenSecret);

	*pbstrKey = CComBSTR(A2W(myOAuthAccessTokenKey.c_str())).Detach();
	*pbstrSecret = CComBSTR(A2W(myOAuthAccessTokenSecret.c_str())).Detach();

	auto bRes = twitterObj.accountVerifyCredGet();
	std::string strResponse;
	twitterObj.getLastWebResponse(strResponse);

	auto value = std::shared_ptr<JSONValue>(JSON::Parse(strResponse.c_str()));
	auto hr = HandleError(value.get());
	if (FAILED(hr))
		return hr;

	return S_OK;
}

STDMETHODIMP CTwitterConnection::OpenConnection(BSTR bstrKey, BSTR bstrSecret)
{
	USES_CONVERSION;

	if (!bstrKey || !bstrSecret)
		return COMADMIN_E_USERPASSWDNOTVALID;

	m_pTwitObj = std::make_shared<twitCurl>();

	m_pTwitObj->getOAuth().setConsumerKey(std::string(APP_KEY));
	m_pTwitObj->getOAuth().setConsumerSecret(std::string(APP_SECRET));

	m_pTwitObj->getOAuth().setOAuthTokenKey(std::string(W2A(bstrKey)));
	m_pTwitObj->getOAuth().setOAuthTokenSecret(std::string(W2A(bstrSecret)));

	auto bRes = m_pTwitObj->accountVerifyCredGet();
	std::string strResponse;
	m_pTwitObj->getLastWebResponse(strResponse);

	auto value = std::shared_ptr<JSONValue>(JSON::Parse(strResponse.c_str()));
	auto hr = HandleError(value.get());
	if (FAILED(hr))
		return hr;
	return S_OK;
}

STDMETHODIMP CTwitterConnection::GetHomeTimeline(BSTR bstrSinceId, IObjectArray** ppObjectArray)
{
	USES_CONVERSION;

	std::string strId;
	if (bstrSinceId)
	{
		strId = W2A(bstrSinceId);
	}

	m_pTwitObj->timelineHomeGet(strId);
	std::string strResponse;
	m_pTwitObj->getLastWebResponse(strResponse);

	auto value = std::shared_ptr<JSONValue>(JSON::Parse(strResponse.c_str()));
	auto hr = HandleError(value.get());
	if (FAILED(hr))
		return hr;

	CComPtr<IObjectCollection> pObjectCollection;
	RETURN_IF_FAILED(CoCreateInstance(CLSID_EnumerableObjectCollection, NULL, CLSCTX_INPROC_SERVER, IID_IObjectCollection, (LPVOID*)&pObjectCollection));
	RETURN_IF_FAILED(pObjectCollection->QueryInterface(ppObjectArray));

	auto valueArray = value->AsArray();
	for (auto it = valueArray.begin(); it != valueArray.end(); it++)
	{
		auto pItem = (*it);
		auto itemObject = pItem->AsObject();

		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObject));
		RETURN_IF_FAILED(pObjectCollection->AddObject(pVariantObject));

		auto userObj = itemObject[L"user"]->AsObject();
		auto userDisplayName = userObj[L"name"]->AsString();
		auto userScreenName = userObj[L"screen_name"]->AsString();
		auto text = itemObject[L"text"]->AsString();
		auto entities = itemObject[L"entities"]->AsObject();
		auto urls = entities[L"urls"]->AsArray();

		CString strText = text.c_str();
		std::hash_set<std::wstring> urlsHashSet;

		if (urls.size())
		{
			for (size_t i = 0; i < urls.size(); i++)
			{
				auto urlObject = urls[i]->AsObject();
				urlsHashSet.insert(urlObject[L"url"]->AsString().c_str());
			}
		}

		auto index = strText.Find(L"http");
		while (index != -1)
		{
			auto indexNext = strText.Find(L" ", index);
			if (indexNext == -1)
				indexNext = strText.GetLength();

			urlsHashSet.insert(std::wstring(strText.Mid(index, indexNext - index)));
			index = strText.Find(L"http", indexNext);
		}

		auto urlsVector = std::vector<std::wstring>(urlsHashSet.cbegin(), urlsHashSet.cend());
		AppendUrls(pVariantObject, urlsVector);

		for (size_t i = 0; i < urlsVector.size(); i++)
		{
			strText.Replace(urlsVector[i].c_str(), L"");
		}

		strText.Replace(L"\r\n", L" ");
		strText.Replace(L"\n", L" ");
		strText.Replace(L"\t", L" ");

		RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_ID, &CComVariant(itemObject[L"id_str"]->AsString().c_str())));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_TWITTER_USER_DISPLAY_NAME, &CComVariant(userDisplayName.c_str())));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_TWITTER_USER_NAME, &CComVariant(userScreenName.c_str())));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_TWITTER_NORMALIZED_TEXT, &CComVariant(strText)));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_TWITTER_TEXT, &CComVariant(text.c_str())));
	}

	return S_OK;
}

STDMETHODIMP CTwitterConnection::AppendUrls(IVariantObject* pVariantObject, std::vector<std::wstring>& urlsVector)
{
	CComPtr<IBstrCollection> pBstrCollection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_BstrCollection, &pBstrCollection));
	for (size_t i = 0; i < urlsVector.size(); i++)
	{
		RETURN_IF_FAILED(pBstrCollection->AddItem(CComBSTR(urlsVector[i].c_str())));
	}
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_TWITTER_URLS, &CComVariant(pBstrCollection)));
	return S_OK;
}