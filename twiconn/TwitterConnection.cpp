// TwitterConnection.cpp : Implementation of CTwitterConnection

#include "stdafx.h"
#include "TwitterConnection.h"

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
