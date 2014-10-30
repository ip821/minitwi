#include "stdafx.h"
#include "TwitViewControl.h"
#include "Plugins.h"
#include "UpdateScope.h"

STDMETHODIMP CTwitViewControl::SetVariantObject(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	CComQIPtr<IInitializeWithVariantObject> pInit = m_pTimelineService;
	ATLASSERT(pInit);
	CComPtr<IVariantObject> pVariantObjectCopy;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObjectCopy));
	RETURN_IF_FAILED(pVariantObject->CopyTo(pVariantObjectCopy));
	RETURN_IF_FAILED(pVariantObjectCopy->SetVariantValue(VAR_ITEM_DOUBLE_SIZE, &CComVariant(true)));
	RETURN_IF_FAILED(pInit->SetVariantObject(pVariantObjectCopy));
	CUpdateScope scope(m_pTimelineControl);
	RETURN_IF_FAILED(m_pTimelineControl->InsertItem(pVariantObjectCopy, 0));
	RETURN_IF_FAILED(m_pTimelineControl->RefreshItem(0));

	CComPtr<IThreadService> pTimelineThread;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &pTimelineThread));
	RETURN_IF_FAILED(pTimelineThread->Run());

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
	return S_OK;
}