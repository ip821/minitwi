// TwitterConnection.h : Declaration of the CTwitterConnection

#pragma once
#include "resource.h"       // main symbols
#include "twiconn_i.h"

using namespace ATL;

// CTwitterConnection

class ATL_NO_VTABLE CTwitterConnection :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTwitterConnection, &CLSID_TwitterConnection>,
	public ITwitterConnection
{
public:
	CTwitterConnection()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_TWITTERCONNECTION)

	BEGIN_COM_MAP(CTwitterConnection)
		COM_INTERFACE_ENTRY(ITwitterConnection)
	END_COM_MAP()

private:

	std::shared_ptr<twitCurl> m_pTwitObj;
	STDMETHOD(HandleError)(JSONValue* value);
	STDMETHOD(AppendUrls)(IVariantObject* pVariantObject, std::vector<std::wstring>& urlsVector);

public:

	STDMETHOD(GetAuthKeys)(BSTR bstrUser, BSTR bstrPass, BSTR* pbstrKey, BSTR* pbstrSecret);
	STDMETHOD(OpenConnection)(BSTR bstrKey, BSTR bstrSecret);
	STDMETHOD(GetHomeTimeline)(BSTR bstrSinceId, IObjectArray** ppObjectArray);
};

OBJECT_ENTRY_AUTO(__uuidof(TwitterConnection), CTwitterConnection)
