#include "stdafx.h"
#include "SearchTimelineControl.h"
#include "Plugins.h"

HRESULT CSearchTimelineControl::Initializing()
{
	RETURN_IF_FAILED(__super::Initializing());
	RETURN_IF_FAILED(m_pPluginSupport->InitializePlugins(PNAMESP_TIMELINE_SEARCH_CONTROL, PVIEWTYPE_WINDOW_SERVICE));
	return S_OK;
}

HRESULT CSearchTimelineControl::OnActivateInternal()
{
	HWND hWnd = 0;
	RETURN_IF_FAILED(m_pTimelineControl->GetHWND(&hWnd));
	::SetFocus(hWnd);
	return S_OK;
}

HRESULT CSearchTimelineControl::Initialized()
{
	RETURN_IF_FAILED(__super::Initialized());
	return S_OK;
}
