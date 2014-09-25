#include "stdafx.h"
#include "PluginTable.h"
#include "Plugins.h"

HRESULT CPluginTable::FinalConstruct()
{
	RETURN_IF_FAILED(AddObject(CLSID_ThemeDefault, L"Default theme object"));
	RETURN_IF_FAILED(AddObject(CLSID_SkinTimeline, L"Timeline skin object"));
	RETURN_IF_FAILED(AddObject(CLSID_SkinTabControl, L"Tab control skin object"));
	RETURN_IF_FAILED(AddObject(CLSID_SkinCommonControl, L"Common control skin object"));
	RETURN_IF_FAILED(AddObject(CLSID_ColumnRects, L"Column rects object"));
	RETURN_IF_FAILED(AddObject(CLSID_ThemeColorMap, L"Color map object"));
	RETURN_IF_FAILED(AddObject(CLSID_ThemeFontMap, L"Font map object"));
	RETURN_IF_FAILED(AddObject(CLSID_PictureWindow, L"Picture window object"));
	RETURN_IF_FAILED(AddObject(CLSID_UserAccountControl, L"User account control object"));
	RETURN_IF_FAILED(AddObject(CLSID_UserInfoControl, L"User info control object"));

	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PTYPE_THEME, THEME_DEFAULT, CLSID_ThemeDefault, L"Default theme"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PTYPE_THEMECOLORMAP, THEME_DEFAULT, CLSID_ThemeDefault, L"Default theme colors"));

	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_CONTAINERWINDOW, CONTROL_TABCONTAINER, CLSID_CustomTabControl, _T("Skin tab control")));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_INPLACE_CONTROL, CLSID_TimelineControl, CLSID_TimelineControl, L"Timeline control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_INPLACE_CONTROL, CLSID_SettingsControl, CLSID_SettingsControl, L"Settings control"));

	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_TIMER, CLSID_TimerService, L"Timer service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_UPDATEIMAGES_TIMER, CLSID_TimerService, L"Timer update images service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_UPDATES_TIMER, CLSID_TimerService, L"Timer update software service"));

	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_THREAD, CLSID_ThreadService, L"Timeline thread service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_SHOWMORE_THREAD, CLSID_ThreadService, L"Timeline thread show more items service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_AUTH_THREAD, CLSID_ThreadService, L"Auth thread service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_UPDATES_THREAD, CLSID_ThreadService, L"Update software thread service"));

	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_CLENUP_TIMER, CLSID_TimerService, L"Timeline control items clenup timer service"));

	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_FORMS_SERVICE, CLSID_FormsService, _T("Forms service")));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineService, CLSID_TimelineService, L"Timeline service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_ThemeService, CLSID_ThemeService, L"Theme service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_ViewControllerService, CLSID_ViewControllerService, L"View controller service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_OPEN_URLS, CLSID_OpenUrlService, L"Open url service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_ImageManagerService, CLSID_ImageManagerService, L"Image manager service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_InfoControlService, CLSID_InfoControlService, L"Info control service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_ThreadPoolService, CLSID_ThreadPoolService, L"Thread pool service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_DownloadService, CLSID_DownloadService, L"Download service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_UpdateService, CLSID_UpdateService, L"Software update service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_WindowService, CLSID_WindowService, L"Window service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineImageService, CLSID_TimelineImageService, L"Timeline control image service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineCleanupService, CLSID_TimelineCleanupService, L"Timeline control clenup service"));

	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_TIMELINE_CONTROL, PVIEWTYPE_COMMAND, CLSID_TimelineControlCopyCommand, CLSID_TimelineControlCopyCommand, L"Copy commands for timeline control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_PICTUREWINDOW_CONTROL, PVIEWTYPE_COMMAND, CLSID_PictureWindowCopyCommand, CLSID_PictureWindowCopyCommand, L"Copy commands for picture window"));

	//RETURN_IF_FAILED(AddPluginInfo(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_DownloadService, CLSID_DownloadService, L"Download service for user info control"));
	//RETURN_IF_FAILED(AddPluginInfo(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_USERINFO_THREAD, CLSID_ThreadService, L"Thread service for user info control"));
	return S_OK;
}
