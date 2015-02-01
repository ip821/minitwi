#include "stdafx.h"
#include "PluginTable.h"

HRESULT CPluginTable::FinalConstruct()
{
	RETURN_IF_FAILED(AddObject(CLSID_PictureWindow, L"Picture window object"));
	RETURN_IF_FAILED(AddObject(CLSID_TimelineControl, L"Timeline control object"));
	RETURN_IF_FAILED(AddObject(CLSID_UserAccountControl, L"User account control object"));
	RETURN_IF_FAILED(AddObject(CLSID_UserInfoControl, L"User info control object"));
	RETURN_IF_FAILED(AddObject(CLSID_TwitViewControl, L"Twit view control object"));
	RETURN_IF_FAILED(AddObject(CLSID_SearchTimelineControl, L"Search timeline control object"));
	RETURN_IF_FAILED(AddObject(CLSID_ListTimelineControl, L"List timeline control object"));

	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_CONTAINERWINDOW, CONTROL_TABCONTAINER, CLSID_CustomTabControl, _T("Skin tab control")));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_INPLACE_CONTROL, CLSID_HomeTimeLineControl, CLSID_HomeTimeLineControl, L"Home timeline control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_INPLACE_CONTROL, CLSID_ListsControl, CLSID_ListsControl, L"Lists control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_INPLACE_CONTROL, CLSID_SearchControl, CLSID_SearchControl, L"Search control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_INPLACE_CONTROL, CLSID_SettingsControl, CLSID_SettingsControl, L"Settings control"));

	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_TIMELINE_CONTROL, PVIEWTYPE_COMMAND, CLSID_TimelineControlCopyCommand, CLSID_TimelineControlCopyCommand, L"Copy commands for timeline control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_TIMELINE_CONTROL, PVIEWTYPE_COMMAND, CLSID_TimelineControlOpenInBrowserCommand, CLSID_TimelineControlOpenInBrowserCommand, L"Open in browser command for timeline control"));

	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_PICTUREWINDOW_CONTROL, PVIEWTYPE_COMMAND, CLSID_PictureWindowCopyCommand, CLSID_PictureWindowCopyCommand, L"Copy commands for picture window"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_PICTUREWINDOW_CONTROL, PVIEWTYPE_COMMAND, CLSID_PictureWindowSaveCommand, CLSID_PictureWindowSaveCommand, L"Save command for picture window"));

	return S_OK;
}
