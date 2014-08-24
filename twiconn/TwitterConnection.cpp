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
		{
			auto errorArray = valueObject[L"errors"]->AsArray();
			auto errObject = errorArray[0]->AsObject();
			m_errMsg = errObject[L"message"]->AsString().c_str();
			SetErrorInfo(0, this);
			return E_FAIL;
		}
	}

	return S_OK;
}

STDMETHODIMP CTwitterConnection::GetDescription(BSTR* pBstrDescription)
{
	CHECK_E_POINTER(pBstrDescription);
	*pBstrDescription = CComBSTR(m_errMsg).Detach();
	return S_OK;
}

STDMETHODIMP CTwitterConnection::GetAuthKeys(BSTR bstrUser, BSTR bstrPass, BSTR* pbstrKey, BSTR* pbstrSecret)
{
	if (!bstrUser || !bstrPass)
	{
		m_errMsg = L"Empty login or password";
		SetErrorInfo(0, this);
		return COMADMIN_E_USERPASSWDNOTVALID;
	}

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
	if (!bRes)
	{
		return ERROR_NETWORK_UNREACHABLE;
	}

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
	{
		m_errMsg = L"Empty login or password";
		SetErrorInfo(0, this);
		return COMADMIN_E_USERPASSWDNOTVALID;
	}

	m_pTwitObj = std::make_shared<twitCurl>();

	m_pTwitObj->getOAuth().setConsumerKey(std::string(APP_KEY));
	m_pTwitObj->getOAuth().setConsumerSecret(std::string(APP_SECRET));

	m_pTwitObj->getOAuth().setOAuthTokenKey(std::string(W2A(bstrKey)));
	m_pTwitObj->getOAuth().setOAuthTokenSecret(std::string(W2A(bstrSecret)));

	auto bRes = m_pTwitObj->accountVerifyCredGet();
	if (!bRes)
	{
		return ERROR_NETWORK_UNREACHABLE;
	}

	std::string strResponse;
	m_pTwitObj->getLastWebResponse(strResponse);

	auto value = std::shared_ptr<JSONValue>(JSON::Parse(strResponse.c_str()));
	auto hr = HandleError(value.get());
	if (FAILED(hr))
		return hr;
	return S_OK;
}

STDMETHODIMP CTwitterConnection::GetHomeTimeline(BSTR bstrSinceId, IObjArray** ppObjectArray)
{
	USES_CONVERSION;

	std::string strId;
	if (bstrSinceId)
	{
		strId = W2A(bstrSinceId);
	}

	auto bRes = m_pTwitObj->timelineHomeGet(strId);
	if (!bRes)
	{
		return ERROR_NETWORK_UNREACHABLE;
	}

	std::string strResponse;
	m_pTwitObj->getLastWebResponse(strResponse);

	auto value = std::shared_ptr<JSONValue>(JSON::Parse(strResponse.c_str()));
	auto hr = HandleError(value.get());
	if (FAILED(hr))
		return hr;

	CComPtr<IObjCollection> pObjectCollection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pObjectCollection));
	RETURN_IF_FAILED(pObjectCollection->QueryInterface(ppObjectArray));

	auto valueArray = value->AsArray();
	for (auto it = valueArray.begin(); it != valueArray.end(); it++)
	{
		auto pItem = (*it);
		auto itemObject = pItem->AsObject();

		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObject));
		RETURN_IF_FAILED(pObjectCollection->AddObject(pVariantObject));

		std::wstring retweetedUserDisplayName;
		std::wstring retweetedUserScreenName;
		auto id = itemObject[L"id_str"]->AsString();

		if (itemObject.find(L"retweeted_status") != itemObject.end())
		{
			auto retweetStatusObject = itemObject[L"retweeted_status"]->AsObject();
			auto retweetedUserObj = itemObject[L"user"]->AsObject();
			retweetedUserDisplayName = retweetedUserObj[L"name"]->AsString();
			retweetedUserScreenName = retweetedUserObj[L"screen_name"]->AsString();
			itemObject = retweetStatusObject;
		}

		auto createdAt = itemObject[L"created_at"]->AsString();
		auto userObj = itemObject[L"user"]->AsObject();
		auto userDisplayName = userObj[L"name"]->AsString();
		auto userScreenName = userObj[L"screen_name"]->AsString();
		auto userImageUrl = userObj[L"profile_image_url"]->AsString();
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

		auto entitiesObj = itemObject[L"entities"]->AsObject();
		if (entitiesObj.find(L"media") != entitiesObj.end())
		{
			CComPtr<IObjCollection> pMediaObjectCollection;
			RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pMediaObjectCollection));
			RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_TWITTER_MEDIAURLS, &CComVariant(pMediaObjectCollection)));

			auto mediaArray = entitiesObj[L"media"]->AsArray();
			for (size_t i = 0; i < mediaArray.size(); i++)
			{
				auto mediaObj = mediaArray[i]->AsObject();
				auto url = mediaObj[L"media_url"]->AsString();
				auto shortUrl = mediaObj[L"url"]->AsString();
				CComPtr<IVariantObject> pMediaObject;
				RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pMediaObject));
				RETURN_IF_FAILED(pMediaObjectCollection->AddObject(pMediaObject));
				RETURN_IF_FAILED(pMediaObject->SetVariantValue(VAR_TWITTER_MEDIAURL_SHORT, &CComVariant(shortUrl.c_str())));
				RETURN_IF_FAILED(pMediaObject->SetVariantValue(VAR_TWITTER_MEDIAURL, &CComVariant(url.c_str())));
				RETURN_IF_FAILED(pMediaObject->SetVariantValue(VAR_TWITTER_MEDIAURL_THUMB, &CComVariant((url + L":small").c_str())));
			}
		}

		auto urlsVector = std::vector<std::wstring>(urlsHashSet.cbegin(), urlsHashSet.cend());
		for (size_t i = 0; i < urlsVector.size(); i++)
		{
			strText.Replace(urlsVector[i].c_str(), L"");
		}

		std::wstring stdStr(strText);

#ifndef DEBUG
		static std::wregex regex(L"((http|https):(\\/*([A-Za-z0-9]*)\\.*)*)");
		static std::regex_constants::match_flag_type fl = std::regex_constants::match_default;

		std::regex_iterator<std::wstring::iterator> regexIterator(stdStr.begin(), stdStr.end(), regex);
		std::regex_iterator<std::wstring::iterator> regexIteratorEnd;
		while (regexIterator != regexIteratorEnd)
		{
			auto strUrl1 = regexIterator->str();
			urlsHashSet.insert(strUrl1);
			regexIterator++;
		}
#endif

		urlsVector = std::vector<std::wstring>(urlsHashSet.cbegin(), urlsHashSet.cend());
		AppendUrls(pVariantObject, urlsVector);

		for (size_t i = 0; i < urlsVector.size(); i++)
		{
			strText.Replace(urlsVector[i].c_str(), L"");
		}

		strText.Replace(L"\r\n", L" ");
		strText.Replace(L"\n", L" ");
		strText.Replace(L"\t", L" ");
		strText = strText.Trim();

		RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_ID, &CComVariant(id.c_str())));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_TWITTER_USER_DISPLAY_NAME, &CComVariant(userDisplayName.c_str())));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_TWITTER_USER_NAME, &CComVariant(userScreenName.c_str())));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_TWITTER_USER_IMAGE, &CComVariant(userImageUrl.c_str())));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_TWITTER_NORMALIZED_TEXT, &CComVariant(strText)));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_TWITTER_TEXT, &CComVariant(text.c_str())));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_TWITTER_CREATED_AT, &CComVariant(createdAt.c_str())));

		if (!retweetedUserDisplayName.empty())
		{
			RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_TWITTER_RETWEETED_USER_DISPLAY_NAME, &CComVariant(retweetedUserDisplayName.c_str())));
			RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_TWITTER_RETWEETED_USER_NAME, &CComVariant(retweetedUserScreenName.c_str())));
		}
	}

	return S_OK;
}

STDMETHODIMP CTwitterConnection::AppendUrls(IVariantObject* pVariantObject, std::vector<std::wstring>& urlsVector)
{
	CComPtr<IBstrCollection> pBstrCollection;
	CComVariant vCollection;
	pVariantObject->GetVariantValue(VAR_TWITTER_URLS, &vCollection);
	if (vCollection.vt == VT_UNKNOWN)
	{
		CComPtr<IUnknown> pUnknown = vCollection.punkVal;
		pUnknown->QueryInterface(&pBstrCollection);
	}

	if (!pBstrCollection)
	{
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_BstrCollection, &pBstrCollection));
	}

	for (size_t i = 0; i < urlsVector.size(); i++)
	{
		RETURN_IF_FAILED(pBstrCollection->AddItem(CComBSTR(urlsVector[i].c_str())));
	}
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_TWITTER_URLS, &CComVariant(pBstrCollection)));
	return S_OK;
}
