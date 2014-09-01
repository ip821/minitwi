#include "stdafx.h"
#include "PluginTable.h"
#include "Plugins.h"

HRESULT CPluginTable::FinalConstruct()
{
	RETURN_IF_FAILED(AddObject(CLSID_TwitterConnection, L"Twitter connection"));
	return S_OK;
}