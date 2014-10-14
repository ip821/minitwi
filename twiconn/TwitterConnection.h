// TwitterConnection.h : Declaration of the CTwitterConnection

#pragma once
#include "resource.h"       // main symbols
#include "twiconn_i.h"

using namespace ATL;

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
	std::shared_ptr<twitCurl> m_pTwitObj;
	std::wstring m_strAppToken;
	STDMETHOD(HandleError)(JSONValue* value);
	STDMETHOD(AppendUrls)(IVariantObject* pVariantObject, std::vector<std::wstring>& urlsVector);
	STDMETHOD(ParseTweets)(JSONValue* value, IObjCollection* pObjectCollection);
	STDMETHOD(ParseUser(JSONObject& value, IVariantObject* pVariantObject));

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
};

OBJECT_ENTRY_AUTO(__uuidof(TwitterConnection), CTwitterConnection)
