#include "stdafx.h"
#include "PluginTable.h"
#include "Plugins.h"

HRESULT CPluginTable::FinalConstruct()
{
	RETURN_IF_FAILED(AddObject(CLSID_ThemeDefault, L"Default theme object"));
	RETURN_IF_FAILED(AddObject(CLSID_SkinTimeline, L"Timeline skin object"));
	RETURN_IF_FAILED(AddObject(CLSID_ColumnRects, L"Column rects object"));
	RETURN_IF_FAILED(AddObject(CLSID_ThemeColorMap, L"Color map object"));
	RETURN_IF_FAILED(AddObject(CLSID_ThemeFontMap, L"Font map object"));

	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PTYPE_THEME, THEME_DEFAULT, CLSID_ThemeDefault, L"Default theme"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PTYPE_THEMECOLORMAP, THEME_DEFAULT, CLSID_ThemeDefault, L"Default theme colors"));

	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimerService, CLSID_TimerService, L"Timer service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_ThreadService, CLSID_ThreadService, L"Thread service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_INPLACE_CONTROL, CLSID_TimelineControl, CLSID_TimelineControl, L"Timeline control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_INPLACE_CONTROL, CLSID_SettingsControl, CLSID_SettingsControl, L"Settings control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_FORMS_SERVICE, CLSID_FormsService, _T("Forms service")));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineService, CLSID_TimelineService, L"Timeline service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_ThemeService, CLSID_ThemeService, L"Theme service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_ViewControllerService, CLSID_ViewControllerService, L"View controller service"));
	return S_OK;
}
