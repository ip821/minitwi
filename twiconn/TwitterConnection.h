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

public:

	STDMETHOD(GetAuthKeys)(BSTR bstrUser, BSTR bstrPass, BSTR* pbstrKey, BSTR* pbstrSecret);

};

OBJECT_ENTRY_AUTO(__uuidof(TwitterConnection), CTwitterConnection)
