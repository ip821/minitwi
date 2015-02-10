// TwitterStreamingConnection.h : Declaration of the CTwitterStreamingConnection

#pragma once
#include "resource.h"       // main symbols
#include "twiconn_i.h"

using namespace ATL;
using namespace std;
using namespace IP;
using namespace IP;

// CTwitterStreamingConnection

class ATL_NO_VTABLE CTwitterStreamingConnection :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTwitterStreamingConnection, &CLSID_TwitterStreamingConnection>,
	public ITwitterStreamingConnection,
	public IErrorInfo,
	public ISupportErrorInfoImpl<&CLSID_TwitterStreamingConnection>,
	public IConnectionPointContainerImpl<CTwitterStreamingConnection>,
	public IConnectionPointImpl<CTwitterStreamingConnection, &__uuidof(ITwitterStreamingConnectionEventSink)>
{
public:
	CTwitterStreamingConnection()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CTwitterStreamingConnection)
		COM_INTERFACE_ENTRY(ITwitterStreamingConnection)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
		COM_INTERFACE_ENTRY(IErrorInfo)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CTwitterStreamingConnection)
		CONNECTION_POINT_ENTRY(__uuidof(ITwitterStreamingConnectionEventSink))
	END_CONNECTION_POINT_MAP()

private:
	string m_callbackData;
	
	int SaveLastWebResponse(char*& data, size_t size);
	static size_t WriteCallback(char *ptr, size_t size, size_t nmemb, CTwitterStreamingConnection* pObj);
	HRESULT Fire_OnMessages(IObjCollection* pObjCollection);
public:

	METHOD_EMPTY(STDMETHOD(GetGUID(GUID *pGUID)));
	METHOD_EMPTY(STDMETHOD(GetSource(BSTR *pBstrSource)));
	METHOD_EMPTY(STDMETHOD(GetDescription(BSTR *pBstrDescription)));
	METHOD_EMPTY(STDMETHOD(GetHelpFile(BSTR *pBstrHelpFile)));
	METHOD_EMPTY(STDMETHOD(GetHelpContext(DWORD *pdwHelpContext)));

	STDMETHOD(StartStream)(BSTR bstrKey, BSTR bstrSecret);
};

OBJECT_ENTRY_AUTO(__uuidof(TwitterStreamingConnection), CTwitterStreamingConnection)
