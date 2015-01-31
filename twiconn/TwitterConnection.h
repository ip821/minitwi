// TwitterConnection.h : Declaration of the CTwitterConnection

#pragma once
#include "resource.h"       // main symbols
#include "twiconn_i.h"

#include <hash_set>
#include <boost\lexical_cast.hpp>

#include <twitcurl.h>

using namespace ATL;
using namespace std;
using namespace IP;
using namespace IP;

// CTwitterConnection

class ATL_NO_VTABLE CTwitterConnection :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTwitterConnection, &CLSID_TwitterConnection>,
	public ITwitterConnection,
	public IErrorInfo,
	public ISupportErrorInfoImpl<&CLSID_TwitterConnection>
{
public:
	CTwitterConnection()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_TWITTERCONNECTION)

	BEGIN_COM_MAP(CTwitterConnection)
		COM_INTERFACE_ENTRY(ITwitterConnection)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
		COM_INTERFACE_ENTRY(IErrorInfo)
	END_COM_MAP()

private:

	CString m_errMsg;
	shared_ptr<twitCurl> m_pTwitObj;
	wstring m_strAppToken;
	STDMETHOD(HandleError)(JSONValue* value);
	STDMETHOD(AppendUrls)(IVariantObject* pVariantObject, vector<wstring>& urlsVector);
	STDMETHOD(ParseTweet)(JSONObject& itemObject, IVariantObject* pVariantObject);
	STDMETHOD(ParseTweets)(JSONValue* value, IObjCollection* pObjectCollection);
	STDMETHOD(ParseUser(JSONObject& value, IVariantObject* pVariantObject));
	STDMETHOD(ParseMedias)(JSONArray& mediaArray, IObjCollection* pMediaObjectCollection, hash_set<wstring>& processedMediaUrls);

public:

	METHOD_EMPTY(STDMETHOD(GetGUID(GUID *pGUID)));
	METHOD_EMPTY(STDMETHOD(GetSource(BSTR *pBstrSource)));
	STDMETHOD(GetDescription(BSTR *pBstrDescription));
	METHOD_EMPTY(STDMETHOD(GetHelpFile(BSTR *pBstrHelpFile)));
	METHOD_EMPTY(STDMETHOD(GetHelpContext(DWORD *pdwHelpContext)));

	STDMETHOD(GetAuthKeys)(BSTR bstrUser, BSTR bstrPass, BSTR* pbstrKey, BSTR* pbstrSecret);
	STDMETHOD(OpenConnection)(BSTR bstrKey, BSTR bstrSecret);
	STDMETHOD(OpenConnectionWithAppAuth)();
	STDMETHOD(GetTimeline)(BSTR bstrUserId, BSTR bstrMaxId, BSTR bstrSinceId, UINT uiMaxCount, IObjArray** ppObjectArray);
	STDMETHOD(GetTwit)(BSTR bstrId, IVariantObject** ppVariantObject);
	STDMETHOD(Search)(BSTR bstrQuery, BSTR bstrSinceId, UINT uiCount, IObjArray** ppObjectArray);
	STDMETHOD(FollowUser)(BSTR bstrUserName);
	STDMETHOD(UnfollowUser)(BSTR bstrUserName);
	STDMETHOD(GetLists)(IObjArray** ppObjectArray);
	STDMETHOD(GetListTweets)(BSTR bstrListId, UINT uiCount, IObjArray** ppObjectArray);
	STDMETHOD(GetUserSettings)(IVariantObject** ppVariantObject);
};

OBJECT_ENTRY_AUTO(__uuidof(TwitterConnection), CTwitterConnection)
