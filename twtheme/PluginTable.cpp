#include "stdafx.h"
#include "PluginTable.h"

HRESULT CPluginTable::FinalConstruct()
{
	RETURN_IF_FAILED(AddObject(CLSID_ThemeDefault, L"Default theme object"));
	RETURN_IF_FAILED(AddObject(CLSID_SkinTimeline, L"Timeline skin object"));
	RETURN_IF_FAILED(AddObject(CLSID_SkinTabControl, L"Tab control skin object"));
	RETURN_IF_FAILED(AddObject(CLSID_SkinCommonControl, L"Common control skin object"));
	RETURN_IF_FAILED(AddObject(CLSID_SkinUserAccountControl, L"User account control skin object"));

	return S_OK;
}
