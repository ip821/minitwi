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
