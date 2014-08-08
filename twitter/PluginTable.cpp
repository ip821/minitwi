#include "stdafx.h"
#include "PluginTable.h"

HRESULT CPluginTable::FinalConstruct()
{
	AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_CONTAINERWINDOW, CONTROL_TABCONTAINER, CLSID_TimelineControl, L"TimelineControl");
	return S_OK;
}