#include "stdafx.h"
#include "ListsControl.h"
#include "Plugins.h"
#include "UpdateScope.h"

STDMETHODIMP CListsControl::GetText(BSTR* pbstr)
{
	*pbstr = CComBSTR(L"Lists").Detach();
	return S_OK;
}

HRESULT CListsControl::Initializing()
{
	RETURN_IF_FAILED(__super::Initializing());
	RETURN_IF_FAILED(m_pPluginSupport->InitializePlugins(PNAMESP_LISTS_CONTROL, PVIEWTYPE_WINDOW_SERVICE));
	return S_OK;
}

HRESULT CListsControl::Initialized()
{
	return S_OK;
}

HRESULT CListsControl::OnActivateInternal()
{
	CComPtr<IThreadService> pThreadService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &pThreadService));
	
	RETURN_IF_FAILED(pThreadService->Join());

	BOOL bEmpty = FALSE;
	RETURN_IF_FAILED(m_pTimelineControl->IsEmpty(&bEmpty));
	if (bEmpty)
	{
		RETURN_IF_FAILED(pThreadService->Run());
	}

	HWND hWnd = 0;
	RETURN_IF_FAILED(m_pTimelineControl->GetHWND(&hWnd));
	::SetFocus(hWnd);

	return S_OK;
}

HRESULT CListsControl::OnDeactivateInternal()
{
	return S_OK;
}