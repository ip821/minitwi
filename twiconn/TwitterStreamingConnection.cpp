#include "stdafx.h"
#include "TwitterStreamingConnection.h"

STDMETHODIMP CTwitterStreamingConnection::StartStream()
{
	return S_OK;
}

HRESULT CTwitterStreamingConnection::Fire_OnMessages(IObjCollection* pObjCollection)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	HRESULT hr = S_OK;
	CTwitterStreamingConnection* pThis = static_cast<CTwitterStreamingConnection*>(this);
	int cConnections = m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
		pThis->Unlock();

		ITwitterStreamingConnectionEventSink * pConnection = static_cast<ITwitterStreamingConnectionEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnMessages(pObjCollection);
		}
	}
	return hr;
}
