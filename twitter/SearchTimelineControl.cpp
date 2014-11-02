#include "stdafx.h"
#include "SearchTimelineControl.h"
#include "Plugins.h"

HRESULT CSearchTimelineControl::Initializing()
{
	RETURN_IF_FAILED(__super::Initializing());
	RETURN_IF_FAILED(m_pPluginSupport->InitializePlugins(PNAMESP_TIMELINE_SEARCH_CONTROL, PVIEWTYPE_WINDOW_SERVICE));
	return S_OK;
}

HRESULT CSearchTimelineControl::Initialized()
{
	RETURN_IF_FAILED(__super::Initialized());
	return S_OK;
}
