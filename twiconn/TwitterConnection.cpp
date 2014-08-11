// TwitterConnection.cpp : Implementation of CTwitterConnection

#include "stdafx.h"
#include "TwitterConnection.h"


// CTwitterConnection

STDMETHODIMP CTwitterConnection::GetAuthKeys(BSTR bstrUser, BSTR bstrPass, BSTR* pbstrKey, BSTR* pbstrSecret)
{
	twitCurl twitterObj;

	USES_CONVERSION;

	twitterObj.setTwitterUsername(std::string(W2A(bstrUser)));
	twitterObj.setTwitterPassword(std::string(W2A(bstrPass)));

	twitterObj.getOAuth().setConsumerKey(std::string("9FA16n89DvlcZCO07IkVHcxio"));
	twitterObj.getOAuth().setConsumerSecret(std::string("stU8OFXzqG9dsp80fnH92d9iSsm11WpPIWnvctKYB7zpcz7WqX"));

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

	return S_OK;
}