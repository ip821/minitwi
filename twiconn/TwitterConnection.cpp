// TwitterConnection.cpp : Implementation of CTwitterConnection

#include "stdafx.h"
#include "TwitterConnection.h"
#include "Plugins.h"
#include "TextNormalizeTable.h"
#include <hash_set>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

// CTwitterConnection

STDMETHODIMP CTwitterConnection::HandleError(JSONValue* value)
{
	if (value == nullptr)
	{
		m_errMsg.Empty();
		return E_FAIL;
	}
	else if (value->IsObject())
	{
		auto valueObject = value->AsObject();
		if (valueObject.find(L"errors") != valueObject.end())
		{
			auto errorArray = valueObject[L"errors"]->AsArray();
			auto errObject = errorArray[0]->AsObject();
			m_errMsg = errObject[L"message"]->AsString().c_str();
			ASSERT_IF_FAILED(SetErrorInfo(0, this));
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

	auto strUser = string(CW2A(bstrUser));
	twitterObj.setTwitterUsername(strUser);
	auto strPass = string(CW2A(bstrPass));
	twitterObj.setTwitterPassword(strPass);

	twitterObj.getOAuth().setConsumerKey(string(APP_KEY));
	twitterObj.getOAuth().setConsumerSecret(string(APP_SECRET));

	string authUrl;
	twitterObj.oAuthRequestToken(authUrl);

	twitterObj.oAuthHandlePIN(authUrl);
	twitterObj.oAuthAccessToken();

	string myOAuthAccessTokenKey;
	string myOAuthAccessTokenSecret;

	twitterObj.getOAuth().getOAuthTokenKey(myOAuthAccessTokenKey);
	twitterObj.getOAuth().getOAuthTokenSecret(myOAuthAccessTokenSecret);

	*pbstrKey = CComBSTR(CA2W(myOAuthAccessTokenKey.c_str())).Detach();
	*pbstrSecret = CComBSTR(CA2W(myOAuthAccessTokenSecret.c_str())).Detach();

	auto bRes = twitterObj.accountVerifyCredGet();
	if (!bRes)
	{
		return HRESULT_FROM_WIN32(ERROR_NETWORK_UNREACHABLE);
	}

	string strResponse;
	twitterObj.getLastWebResponse(strResponse);

	auto value = shared_ptr<JSONValue>(JSON::Parse(strResponse.c_str()));
	auto hr = HandleError(value.get());
	if (FAILED(hr))
		return hr;

	return S_OK;
}

STDMETHODIMP CTwitterConnection::OpenConnectionWithAppAuth()
{
	USES_CONVERSION;

	m_pTwitObj = make_shared<twitCurl>();
	auto strAppKey = string(CW2A(CString(APP_KEY)));
	auto strAppSecret = string(CW2A(CString(APP_SECRET)));
	auto bRes = m_pTwitObj->authAppOnly(strAppKey, strAppSecret);

	if (!bRes)
	{
		return HRESULT_FROM_WIN32(ERROR_NETWORK_UNREACHABLE);
	}

	string strResponse;
	m_pTwitObj->getLastWebResponse(strResponse);

	auto value = shared_ptr<JSONValue>(JSON::Parse(strResponse.c_str()));
	auto hr = HandleError(value.get());
	if (FAILED(hr))
		return hr;

	auto objValue = value->AsObject();
	m_strAppToken = objValue[L"access_token"]->AsString();
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

	m_pTwitObj = make_shared<twitCurl>();

	m_pTwitObj->getOAuth().setConsumerKey(string(APP_KEY));
	m_pTwitObj->getOAuth().setConsumerSecret(string(APP_SECRET));

	m_pTwitObj->getOAuth().setOAuthTokenKey(string(CW2A(bstrKey)));
	m_pTwitObj->getOAuth().setOAuthTokenSecret(string(CW2A(bstrSecret)));

	auto bRes = m_pTwitObj->accountVerifyCredGet();
	if (!bRes)
	{
		return HRESULT_FROM_WIN32(ERROR_NETWORK_UNREACHABLE);
	}

	string strResponse;
	m_pTwitObj->getLastWebResponse(strResponse);

	auto value = shared_ptr<JSONValue>(JSON::Parse(strResponse.c_str()));
	auto hr = HandleError(value.get());
	if (FAILED(hr))
		return hr;
	return S_OK;
}

STDMETHODIMP CTwitterConnection::GetListTweets(BSTR bstrListId, UINT uiCount, IObjArray** ppObjectArray)
{
	CHECK_E_POINTER(bstrListId);
	CHECK_E_POINTER(ppObjectArray);

	USES_CONVERSION;

	string strListId = CW2A(bstrListId);

	if (!m_pTwitObj->listStatuses(strListId, boost::lexical_cast<string>(uiCount)))
	{
		return HRESULT_FROM_WIN32(ERROR_NETWORK_UNREACHABLE);
	}

	string strResponse;
	m_pTwitObj->getLastWebResponse(strResponse);

	auto value = shared_ptr<JSONValue>(JSON::Parse(strResponse.c_str()));
	auto hr = HandleError(value.get());
	if (FAILED(hr))
		return hr;

	CComPtr<IObjCollection> pObjectCollection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pObjectCollection));
	auto items = value.get();
	RETURN_IF_FAILED(ParseTweets(items, pObjectCollection));
	RETURN_IF_FAILED(pObjectCollection->QueryInterface(ppObjectArray));
	return S_OK;
}

STDMETHODIMP CTwitterConnection::GetFollowStatus(BSTR bstrTargetUserName, BOOL* pbFollowing)
{
	CHECK_E_POINTER(bstrTargetUserName);
	CHECK_E_POINTER(pbFollowing);
	USES_CONVERSION;

	string userName = CW2A(bstrTargetUserName);
	if (!m_pTwitObj->friendshipShow(userName))
	{
		return HRESULT_FROM_WIN32(ERROR_NETWORK_UNREACHABLE);
	}

	string strResponse;
	m_pTwitObj->getLastWebResponse(strResponse);

	auto value = shared_ptr<JSONValue>(JSON::Parse(strResponse.c_str()));
	auto hr = HandleError(value.get());
	if (FAILED(hr))
		return hr;

	CComPtr<IVariantObject> pVariantObjectSettings;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObjectSettings));
	auto valueObject = value.get()->AsObject();
	auto relationshipObject = valueObject[L"relationship"]->AsObject();
	auto targetObject = relationshipObject[L"target"]->AsObject();
	*pbFollowing = targetObject[L"followed_by"]->AsBool();
	return S_OK;
}

STDMETHODIMP CTwitterConnection::GetUserSettings(IVariantObject** ppVariantObject)
{
	CHECK_E_POINTER(ppVariantObject);

	USES_CONVERSION;

	if (!m_pTwitObj->accountGetSettings())
	{
		return HRESULT_FROM_WIN32(ERROR_NETWORK_UNREACHABLE);
	}

	string strResponse;
	m_pTwitObj->getLastWebResponse(strResponse);

	auto value = shared_ptr<JSONValue>(JSON::Parse(strResponse.c_str()));
	auto hr = HandleError(value.get());
	if (FAILED(hr))
		return hr;

	CComPtr<IVariantObject> pVariantObjectSettings;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObjectSettings));
	auto settingsObject = value.get()->AsObject();
	auto userScreenName = settingsObject[L"screen_name"]->AsString();
	RETURN_IF_FAILED(pVariantObjectSettings->SetVariantValue(Twitter::Connection::Metadata::UserObject::Name, &CComVariant(userScreenName.c_str())));
	RETURN_IF_FAILED(pVariantObjectSettings->QueryInterface(ppVariantObject));
	return S_OK;
}

STDMETHODIMP CTwitterConnection::GetLists(IObjArray** ppObjectArray)
{
	CHECK_E_POINTER(ppObjectArray);

	USES_CONVERSION;

	string strAppToken = CW2A(m_strAppToken.c_str());

	if (!m_pTwitObj->listsGet())
	{
		return HRESULT_FROM_WIN32(ERROR_NETWORK_UNREACHABLE);
	}

	string strResponse;
	m_pTwitObj->getLastWebResponse(strResponse);

	auto value = shared_ptr<JSONValue>(JSON::Parse(strResponse.c_str()));
	auto hr = HandleError(value.get());
	if (FAILED(hr))
		return hr;

	CComPtr<IObjCollection> pObjectCollection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pObjectCollection));
	auto items = value.get()->AsArray();
	for (size_t i = 0; i < items.size(); i++)
	{
		auto item = items[i]->AsObject();
		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObject));

		auto strId = item[L"id_str"]->AsString();
		auto strName = item[L"name"]->AsString();
		auto userObj = item[L"user"]->AsObject();
		auto userScreenName = userObj[L"screen_name"]->AsString();
		auto userImageUrl = userObj[L"profile_image_url_https"]->AsString();
		auto userDisplayName = userObj[L"name"]->AsString();

		RETURN_IF_FAILED(pVariantObject->SetVariantValue(ObjectModel::Metadata::Object::Id, &CComVariant(strId.c_str())));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(ObjectModel::Metadata::Object::Name, &CComVariant(strName.c_str())));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::TweetObject::Text, &CComVariant(strName.c_str())));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::TweetObject::NormalizedText, &CComVariant(strName.c_str())));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::UserObject::Name, &CComVariant(userScreenName.c_str())));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::UserObject::DisplayName, &CComVariant(userDisplayName.c_str())));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::UserObject::Image, &CComVariant(userImageUrl.c_str())));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(ObjectModel::Metadata::Object::Type, &CComVariant(Twitter::Connection::Metadata::ListObject::TypeId)));

		CComPtr<IVariantObject> pUserVariantObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pUserVariantObject));
		RETURN_IF_FAILED(ParseUser(userObj, pUserVariantObject));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::TweetObject::UserObject, &CComVariant(pUserVariantObject)));

		RETURN_IF_FAILED(pObjectCollection->AddObject(pVariantObject));
	}
	RETURN_IF_FAILED(pObjectCollection->QueryInterface(ppObjectArray));
	return S_OK;
}

STDMETHODIMP CTwitterConnection::Search(BSTR bstrQuery, BSTR bstrSinceId, UINT uiCount, IObjArray** ppObjectArray)
{
	CHECK_E_POINTER(bstrQuery);
	CHECK_E_POINTER(ppObjectArray);

	USES_CONVERSION;

	string strQuery = CW2A(bstrQuery, CP_UTF8);
	string strAppToken = CW2A(m_strAppToken.c_str());
	string strSinceId;
	if (bstrSinceId != nullptr)
	{
		strSinceId = CW2A(bstrSinceId);
	}

	if (!m_pTwitObj->searchWithAppAuth(strAppToken, strQuery, strSinceId, boost::lexical_cast<string>(uiCount)))
	{
		return HRESULT_FROM_WIN32(ERROR_NETWORK_UNREACHABLE);
	}

	string strResponse;
	m_pTwitObj->getLastWebResponse(strResponse);

	auto value = shared_ptr<JSONValue>(JSON::Parse(strResponse.c_str()));
	auto hr = HandleError(value.get());
	if (FAILED(hr))
		return hr;

	CComPtr<IObjCollection> pObjectCollection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pObjectCollection));
	auto val = value.get()->AsObject();
	auto items = val[L"statuses"];
	RETURN_IF_FAILED(ParseTweets(items, pObjectCollection));
	RETURN_IF_FAILED(pObjectCollection->QueryInterface(ppObjectArray));
	return S_OK;
}

STDMETHODIMP CTwitterConnection::GetTwit(BSTR bstrId, IVariantObject** ppVariantObject)
{
	CHECK_E_POINTER(ppVariantObject);
	if (!bstrId)
		return E_POINTER;

	if (bstrId == CComBSTR(""))
		return E_INVALIDARG;

	USES_CONVERSION;

	string strId = CW2A(bstrId);

	string strAppToken = CW2A(m_strAppToken.c_str());
	if (!m_pTwitObj->statusShowByIdWithAppAuth(strAppToken, strId))
	{
		return HRESULT_FROM_WIN32(ERROR_NETWORK_UNREACHABLE);
	}

	string strResponse;
	m_pTwitObj->getLastWebResponse(strResponse);

	auto value = shared_ptr<JSONValue>(JSON::Parse(strResponse.c_str()));
	auto hr = HandleError(value.get());
	if (FAILED(hr))
		return hr;

	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, ppVariantObject));
	auto itemObject = value.get()->AsObject();
	RETURN_IF_FAILED(ParseTweet(itemObject, *ppVariantObject));

	return S_OK;
}

STDMETHODIMP CTwitterConnection::UnfollowUser(BSTR bstrUserName)
{
	CHECK_E_POINTER(bstrUserName);

	USES_CONVERSION;

	string strUserName = CW2A(bstrUserName);

	bool bRes = false;
	bRes = m_pTwitObj->friendshipDestroy(strUserName);

	if (!bRes)
	{
		return HRESULT_FROM_WIN32(ERROR_NETWORK_UNREACHABLE);
	}

	string strResponse;
	m_pTwitObj->getLastWebResponse(strResponse);

	auto value = shared_ptr<JSONValue>(JSON::Parse(strResponse.c_str()));
	auto hr = HandleError(value.get());
	if (FAILED(hr))
		return hr;

	return S_OK;
}

STDMETHODIMP CTwitterConnection::FollowUser(BSTR bstrUserName)
{
	CHECK_E_POINTER(bstrUserName);
	
	USES_CONVERSION;
	
	string strUserName = CW2A(bstrUserName);

	bool bRes = false;
	bRes = m_pTwitObj->friendshipCreate(strUserName);

	if (!bRes)
	{
		return HRESULT_FROM_WIN32(ERROR_NETWORK_UNREACHABLE);
	}

	string strResponse;
	m_pTwitObj->getLastWebResponse(strResponse);

	auto value = shared_ptr<JSONValue>(JSON::Parse(strResponse.c_str()));
	auto hr = HandleError(value.get());
	if (FAILED(hr))
		return hr;

	return S_OK;
}

STDMETHODIMP CTwitterConnection::GetTimeline(BSTR bstrUserId, BSTR bstrMaxId, BSTR bstrSinceId, UINT uiMaxCount, IObjArray** ppObjectArray)
{
	USES_CONVERSION;

	string strId;
	if (bstrMaxId)
	{
		strId = CW2A(bstrMaxId);
	}

	string strMaxCount;
	if (uiMaxCount)
	{
		strMaxCount = boost::lexical_cast<string>(uiMaxCount);
	}

	string strSinceId;
	if (bstrSinceId)
	{
		strSinceId = CW2A(bstrSinceId);
	}

	string strUserId;
	if (bstrUserId)
	{
		strUserId = CW2A(bstrUserId);
	}

	bool bRes = false;
	if (strUserId.empty())
	{
		bRes = m_pTwitObj->timelineHomeGet(strId, strSinceId, strMaxCount);
	}
	else
	{
		if (m_strAppToken.empty())
		{
			bRes = m_pTwitObj->timelineUserGet(strUserId, strId, strSinceId, strMaxCount);
		}
		else
		{
			string strAppToken = CW2A(m_strAppToken.c_str());
			bRes = m_pTwitObj->timelineUserGetWithAppAuth(strAppToken, strUserId, strId, strSinceId, strMaxCount);
		}
	}

	if (!bRes)
	{
		return HRESULT_FROM_WIN32(ERROR_NETWORK_UNREACHABLE);
	}

	string strResponse;
	m_pTwitObj->getLastWebResponse(strResponse);

	auto value = shared_ptr<JSONValue>(JSON::Parse(strResponse.c_str()));
	auto hr = HandleError(value.get());
	if (FAILED(hr))
		return hr;

	CComPtr<IObjCollection> pObjectCollection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pObjectCollection));
	RETURN_IF_FAILED(pObjectCollection->QueryInterface(ppObjectArray));
	RETURN_IF_FAILED(ParseTweets(value.get(), pObjectCollection));
	return S_OK;
}

HRESULT CTwitterConnection::ParseUser(JSONObject& value, IVariantObject* pVariantObject)
{
	auto userDisplayName = value[L"name"]->AsString();
	auto userScreenName = value[L"screen_name"]->AsString();
	auto userImageUrl = value[L"profile_image_url_https"]->AsString();
	auto userBackgroundColor = FromHexString(value[L"profile_background_color"]->AsString());
	auto userForegroundColor = FromHexString(value[L"profile_text_color"]->AsString());
	auto followersCount = value[L"followers_count"]->AsNumber();
	auto friendsCount = value[L"friends_count"]->AsNumber();
	auto statusesCount = value[L"statuses_count"]->AsNumber();
	auto description = value[L"description"]->AsString();
	
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::UserObject::DisplayName, &CComVariant(userDisplayName.c_str())));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::UserObject::Name, &CComVariant(userScreenName.c_str())));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::UserObject::Image, &CComVariant(userImageUrl.c_str())));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::UserObject::BackColor, &CComVariant((int)userBackgroundColor)));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::UserObject::ForeColor, &CComVariant((int)userForegroundColor)));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::UserObject::FollowersCount, &CComVariant((int)followersCount)));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::UserObject::FriendsCount, &CComVariant((int)friendsCount)));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::UserObject::TweetsCount, &CComVariant((int)statusesCount)));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::UserObject::Description, &CComVariant(description.c_str())));

	if (value.find(L"profile_banner_url") != value.end())
	{
		auto userBannerUrl = value[L"profile_banner_url"]->AsString();
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::UserObject::Banner, &CComVariant(userBannerUrl.c_str())));
	}
	return S_OK;
}

HRESULT CTwitterConnection::ParseTweet(JSONObject& itemObject, IVariantObject* pVariantObject)
{
	wstring retweetedUserDisplayName;
	wstring retweetedUserScreenName;
	auto id = itemObject[L"id_str"]->AsString();

	if (itemObject.find(L"in_reply_to_status_id_str") != itemObject.end() && itemObject.find(L"in_reply_to_status_id_str")->second->IsString())
	{
		auto inReplyToStatusId = itemObject[L"in_reply_to_status_id_str"]->AsString();
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::TweetObject::InReplyToStatusId, &CComVariant(inReplyToStatusId.c_str())));
	}

	if (itemObject.find(L"retweeted_status") != itemObject.end())
	{
		auto retweetStatusObject = itemObject[L"retweeted_status"]->AsObject();
		auto retweetedUserObj = itemObject[L"user"]->AsObject();
		retweetedUserDisplayName = retweetedUserObj[L"name"]->AsString();
		retweetedUserScreenName = retweetedUserObj[L"screen_name"]->AsString();
		auto idOriginal = retweetStatusObject[L"id_str"]->AsString();
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::TweetObject::OriginalId, &CComVariant(idOriginal.c_str())));
		itemObject = retweetStatusObject;

		CComPtr<IVariantObject> pRetwettedUserVariantObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pRetwettedUserVariantObject));
		RETURN_IF_FAILED(ParseUser(retweetedUserObj, pRetwettedUserVariantObject));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::TweetObject::RetweetedUserObject, &CComVariant(pRetwettedUserVariantObject)));
	}

	auto createdAt = itemObject[L"created_at"]->AsString();
	auto userObj = itemObject[L"user"]->AsObject();

	CComPtr<IVariantObject> pUserVariantObject;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pUserVariantObject));
	RETURN_IF_FAILED(ParseUser(userObj, pUserVariantObject));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::TweetObject::UserObject, &CComVariant(pUserVariantObject)));

	auto userDisplayName = userObj[L"name"]->AsString();
	auto userScreenName = userObj[L"screen_name"]->AsString();
	auto userImageUrl = userObj[L"profile_image_url_https"]->AsString();
	auto text = itemObject[L"text"]->AsString();
	auto entities = itemObject[L"entities"]->AsObject();
	auto urls = entities[L"urls"]->AsArray();

	//CString strText = L"test\ntest\ntest\ntest\ntest\ntest\ntest\ntest\ntest\ntest\ntest\ntest\ntest\ntest\ntest\ntest\ntest\ntest\ntest\ntest\ntest\ntest\ntest\ntest\n";
	CString strText = text.c_str();
	hash_set<wstring> urlsHashSet;

	if (urls.size())
	{
		for (size_t i = 0; i < urls.size(); i++)
		{
			auto urlObject = urls[i]->AsObject();
			urlsHashSet.insert(urlObject[L"url"]->AsString().c_str());
		}
	}

	CComPtr<IObjCollection> pMediaObjectCollection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pMediaObjectCollection));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::TweetObject::MediaUrls, &CComVariant(pMediaObjectCollection)));

	hash_set<wstring> processedMediaUrls;

	{
		auto entitiesObj = itemObject[L"entities"]->AsObject();
		if (entitiesObj.find(L"media") != entitiesObj.end())
		{
			auto mediaArray = entitiesObj[L"media"]->AsArray();
			ParseMedias(mediaArray, pMediaObjectCollection, processedMediaUrls);
		}
	}

	if (itemObject.find(L"extended_entities") != itemObject.end())
	{
		auto entitiesObj = itemObject[L"extended_entities"]->AsObject();
		if (entitiesObj.find(L"media") != entitiesObj.end())
		{
			auto mediaArray = entitiesObj[L"media"]->AsArray();
			ParseMedias(mediaArray, pMediaObjectCollection, processedMediaUrls);
		}
	}

	auto urlsVector = vector<wstring>(urlsHashSet.cbegin(), urlsHashSet.cend());
	for (size_t i = 0; i < urlsVector.size(); i++)
	{
		strText.Replace(urlsVector[i].c_str(), L"");
	}

	wstring stdStr(strText);

#ifndef DEBUG
	static boost::wregex regex(L"((http|https):(\\/*([A-Za-z0-9]*)\\.*)*)");
	static boost::regex_constants::match_flag_type fl = boost::regex_constants::match_default;

	boost::regex_iterator<wstring::iterator> regexIterator(stdStr.begin(), stdStr.end(), regex);
	boost::regex_iterator<wstring::iterator> regexIteratorEnd;
	while (regexIterator != regexIteratorEnd)
	{
		auto strUrl1 = regexIterator->str();
		urlsHashSet.insert(strUrl1);
		regexIterator++;
	}
#endif

	urlsVector = vector<wstring>(urlsHashSet.cbegin(), urlsHashSet.cend());
	AppendUrls(pVariantObject, urlsVector);

	for (size_t i = 0; i < urlsVector.size(); i++)
	{
		strText.Replace(urlsVector[i].c_str(), L"");
	}

	for (auto it = NormalizeTable.begin(); it != NormalizeTable.end(); it++)
	{
		strText.Replace(it->first, it->second);
	}
	strText = strText.Trim();

	RETURN_IF_FAILED(pVariantObject->SetVariantValue(ObjectModel::Metadata::Object::Id, &CComVariant(id.c_str())));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::UserObject::DisplayName, &CComVariant(userDisplayName.c_str())));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::UserObject::Name, &CComVariant(userScreenName.c_str())));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::UserObject::Image, &CComVariant(userImageUrl.c_str())));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::TweetObject::NormalizedText, &CComVariant(strText)));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::TweetObject::Text, &CComVariant(text.c_str())));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::TweetObject::CreatedAt, &CComVariant(createdAt.c_str())));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(ObjectModel::Metadata::Object::Type, &CComVariant(Twitter::Connection::Metadata::TweetObject::TypeId)));

	if (!retweetedUserDisplayName.empty())
	{
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::TweetObject::RetweetedUserDisplayName, &CComVariant(retweetedUserDisplayName.c_str())));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::TweetObject::RetweetedUserName, &CComVariant(retweetedUserScreenName.c_str())));
	}
	return S_OK;
}

STDMETHODIMP CTwitterConnection::ParseTweets(JSONValue* value, IObjCollection* pObjectCollection)
{
	auto valueArray = value->AsArray();
	for (auto it = valueArray.begin(); it != valueArray.end(); it++)
	{
		auto pItem = (*it);
		auto itemObject = pItem->AsObject();

		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObject));
		RETURN_IF_FAILED(pObjectCollection->AddObject(pVariantObject));

		RETURN_IF_FAILED(ParseTweet(itemObject, pVariantObject));
	}
	return S_OK;
}

HRESULT CTwitterConnection::ParseMedias(JSONArray& mediaArray, IObjCollection* pMediaObjectCollection, hash_set<wstring>& processedMediaUrls)
{
	for (size_t i = 0; i < mediaArray.size(); i++)
	{
		auto mediaObj = mediaArray[i]->AsObject();
		auto url = mediaObj[L"media_url"]->AsString();
		if (processedMediaUrls.find(url) != processedMediaUrls.end())
			continue;
		processedMediaUrls.insert(url);
		auto shortUrl = mediaObj[L"url"]->AsString();
		CComPtr<IVariantObject> pMediaObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pMediaObject));
		RETURN_IF_FAILED(pMediaObjectCollection->AddObject(pMediaObject));
		RETURN_IF_FAILED(pMediaObject->SetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrlShort, &CComVariant(shortUrl.c_str())));
		RETURN_IF_FAILED(pMediaObject->SetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrl, &CComVariant(url.c_str())));
		RETURN_IF_FAILED(pMediaObject->SetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrlThumb, &CComVariant((url + L":small").c_str())));
	}
	return S_OK;
}

HRESULT CTwitterConnection::AppendUrls(IVariantObject* pVariantObject, vector<wstring>& urlsVector)
{
	CComPtr<IBstrCollection> pBstrCollection;
	CComVariant vCollection;
	pVariantObject->GetVariantValue(Twitter::Connection::Metadata::TweetObject::MediaUrls, &vCollection);
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
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::TweetObject::Urls, &CComVariant(pBstrCollection)));
	return S_OK;
}
