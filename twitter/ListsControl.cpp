#include "stdafx.h"
#include "ListsControl.h"
#include "Plugins.h"
#include "UpdateScope.h"

STDMETHODIMP CListsControl::GetText(BSTR* pbstr)
{
	*pbstr = CComBSTR(L"Lists").Detach();
	return S_OK;
}

STDMETHODIMP CListsControl::GetVariantObject(IVariantObject** ppVariantObject)
{
	CHECK_E_POINTER(ppVariantObject);
	RETURN_IF_FAILED(m_pVariantObject->QueryInterface(ppVariantObject));
	return S_OK;
}

STDMETHODIMP CListsControl::SetVariantObject(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	CComQIPtr<IInitializeWithVariantObject> pInit = m_pTimelineService;
	ATLASSERT(pInit);
	m_pVariantObject = pVariantObject;
	RETURN_IF_FAILED(pInit->SetVariantObject(m_pVariantObject));

	CComPtr<IThreadService> pTimelineThread;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &pTimelineThread));
	RETURN_IF_FAILED(pTimelineThread->Run());

	return S_OK;
}

HRESULT CListsControl::Initializing()
{
	RETURN_IF_FAILED(__super::Initializing());
	RETURN_IF_FAILED(m_pPluginSupport->InitializePlugins(PNAMESP_TWITVIEW_CONTROL, PVIEWTYPE_WINDOW_SERVICE));
	return S_OK;
}

HRESULT CListsControl::Initialized()
{
	RETURN_IF_FAILED(__super::Initialized());
	return S_OK;
}

HRESULT CListsControl::ShuttingDown()
{
	RETURN_IF_FAILED(__super::ShuttingDown());
	m_pVariantObject.Release();
	return S_OK;
}
