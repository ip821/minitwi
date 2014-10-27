#include "stdafx.h"
#include "TwitViewControl.h"
#include "Plugins.h"

STDMETHODIMP CTwitViewControl::SetVariantObject(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	CComQIPtr<IInitializeWithVariantObject> pInit = m_pTimelineService;
	ATLASSERT(pInit);
	RETURN_IF_FAILED(pInit->SetVariantObject(pVariantObject));
	return S_OK;
}
