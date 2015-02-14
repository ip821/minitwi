#include "stdafx.h"
#include "HomeTimeLineControl.h"
#include "Plugins.h"

STDMETHODIMP CHomeTimeLineControl::GetText(BSTR* pbstr)
{
	*pbstr = CComBSTR(L"Home").Detach();
	return S_OK;
}

HRESULT CHomeTimeLineControl::Initializing()
{
	RETURN_IF_FAILED(__super::Initializing());
	RETURN_IF_FAILED(m_pPluginSupport->InitializePlugins(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE));
	return S_OK;
}

HRESULT CHomeTimeLineControl::Initialized()
{
	RETURN_IF_FAILED(__super::Initialized());
	return S_OK;
}
