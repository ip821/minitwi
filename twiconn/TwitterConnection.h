// TwitterConnection.h : Declaration of the CTwitterConnection

#pragma once
#include "resource.h"       // main symbols
#include "twconn_i.h"

#include <unordered_set>
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

	friend class CTwitterStreamingConnection;

	struct Indexes
	{
		int Start = 0;
		int End = 0;
	};

private:

	CString m_errMsg;
	shared_ptr<twitCurl> m_pTwitObj;
	wstring m_strAppToken;
	STDMETHOD(HandleError)(JSONValue* value);
	static HRESULT AppendUrls(IVariantObject* pVariantObject, vector<pair<wstring, Indexes>>& urlsVector);
	static HRESULT ParseTweet(JSONObject& itemObject, IVariantObject* pVariantObject);
	STDMETHOD(ParseTweets)(JSONValue* value, IObjCollection* pObjectCollection);
	static HRESULT ParseUser(JSONObject& value, IVariantObject* pVariantObject);
	static HRESULT ParseMedias(JSONArray& mediaArray, IObjCollection* pMediaObjectCollection, unordered_map<wstring,wstring>& processedMediaUrls);

public:

	METHOD_EMPTY(STDMETHOD(GetGUID(GUID *pGUID)));
	METHOD_EMPTY(STDMETHOD(GetSource(BSTR *pBstrSource)));
	STDMETHOD(GetDescription(BSTR *pBstrDescription));
	METHOD_EMPTY(STDMETHOD(GetHelpFile(BSTR *pBstrHelpFile)));
	METHOD_EMPTY(STDMETHOD(GetHelpContext(DWORD *pdwHelpContext)));

	STDMETHOD(GetAccessUrl)(BSTR* pbstrAuthKey, BSTR* pbstrAuthSecret, BSTR* pbstrUrl);
	STDMETHOD(GetAccessTokens)(BSTR bstrAuthKey, BSTR bstrAuthSecret, BSTR bstrPin, BSTR* pbstrUser, BSTR* pbstrKey, BSTR* pbstrKeySecret);
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
	STDMETHOD(GetFollowStatus)(BSTR bstrTargetUserName, BOOL* pbFollowing);
	STDMETHOD(GetUser)(BSTR bstrUserName, IVariantObject** ppVariantObject);
    STDMETHOD(GetFollowingUsers)(BSTR bstrUserName, BSTR bstrNextCursor, IVariantObject** ppVariantObject);
};

OBJECT_ENTRY_AUTO(__uuidof(TwitterConnection), CTwitterConnection)
