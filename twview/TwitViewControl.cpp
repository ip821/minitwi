#include "stdafx.h"
#include "TwitViewControl.h"
#include "Plugins.h"
#include "..\twitter\UpdateScope.h"

STDMETHODIMP CTwitViewControl::GetVariantObject(IVariantObject** ppVariantObject)
{
	CHECK_E_POINTER(ppVariantObject);
	RETURN_IF_FAILED(m_pVariantObject->QueryInterface(ppVariantObject));
	return S_OK;
}

STDMETHODIMP CTwitViewControl::SetVariantObject(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	m_pVariantObject = pVariantObject;
	return S_OK;
}

HRESULT CTwitViewControl::OnActivateInternal()
{
	CComQIPtr<IInitializeWithVariantObject> pInit = m_pTimelineService;
	ATLASSERT(pInit);
	CComPtr<IVariantObject> pVariantObjectCopy;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObjectCopy));
	RETURN_IF_FAILED(m_pVariantObject->CopyTo(pVariantObjectCopy));
	m_pVariantObject = pVariantObjectCopy;
	RETURN_IF_FAILED(pVariantObjectCopy->SetVariantValue(Twitter::Metadata::Item::VAR_ITEM_DOUBLE_SIZE, &CComVar(true)));
	RETURN_IF_FAILED(pInit->SetVariantObject(pVariantObjectCopy));

	{
		CUpdateScope scope(m_pTimelineControl);
		RETURN_IF_FAILED(m_pTimelineControl->InsertItem(pVariantObjectCopy, 0));
	}

	CComPtr<IThreadService> pTimelineThread;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_UPDATE_THREAD, &pTimelineThread));
	RETURN_IF_FAILED(pTimelineThread->Run());

	HWND hWnd = 0;
	RETURN_IF_FAILED(m_pTimelineControl->GetHWND(&hWnd));
	::SetFocus(hWnd);

	return S_OK;
}

HRESULT CTwitViewControl::Initializing()
{
	RETURN_IF_FAILED(__super::Initializing());
	RETURN_IF_FAILED(m_pPluginSupport->InitializePlugins(PNAMESP_TWITVIEW_CONTROL, PVIEWTYPE_WINDOW_SERVICE));
	return S_OK;
}

HRESULT CTwitViewControl::Initialized()
{
	RETURN_IF_FAILED(__super::Initialized());
	return S_OK;
}

HRESULT CTwitViewControl::ShuttingDown()
{
	RETURN_IF_FAILED(__super::ShuttingDown());
	m_pVariantObject.Release();
	return S_OK;
}
