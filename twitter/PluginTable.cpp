#include "stdafx.h"
#include "PluginTable.h"
#include "Plugins.h"

HRESULT CPluginTable::FinalConstruct()
{
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PTYPE_THEME, THEME_DEFAULT, CLSID_ThemeDefault, L"Default theme"));

	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_UPDATES_TIMER, CLSID_TimerService, L"Timer update software service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_AUTH_THREAD, CLSID_ThreadService, L"Auth thread service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_LoginService, CLSID_LoginService, L"Login service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_UPDATES_THREAD, CLSID_ThreadService, L"Update software thread service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_FORMS_SERVICE, CLSID_FormsService, _T("Forms service")));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_ThemeService, CLSID_ThemeService, L"Theme service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_ViewControllerService, CLSID_ViewControllerService, L"View controller service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_InfoControlService, CLSID_InfoControlService, L"Info control service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_ThreadPoolService, CLSID_ThreadPoolService, L"Thread pool service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_DownloadService, CLSID_DownloadService, L"Download service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_UpdateService, CLSID_UpdateService, L"Software update service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_WindowService, CLSID_WindowService, L"Window service"));
    RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_STREAMING_THREAD, CLSID_ThreadService, L"Twitter streaming thread service"));
    RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, CLSID_HomeTimelineStreamingService, CLSID_HomeTimelineStreamingService, L"Twitter streaming service"));

	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ThreadPoolService, CLSID_ThreadPoolService, L"Thread pool service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_DownloadService, CLSID_DownloadService, L"Download service for home timeline control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_TIMER, CLSID_TimerService, L"Timer service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_UPDATEIMAGES_TIMER, CLSID_TimerService, L"Timer update images service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_UPDATE_THREAD, CLSID_ThreadService, L"Timeline connection thread service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_SHOWMORE_THREAD, CLSID_ThreadService, L"Timeline thread show more items service"));
    RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_HomeTimelineStreamingServiceEventListener, CLSID_HomeTimelineStreamingServiceEventListener, L"Timeline streaming service event listener"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_CLENUP_TIMER, CLSID_TimerService, L"Timeline control items clenup timer service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineLoadingService, CLSID_TimelineLoadingService, L"Timeline loading service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ShowMoreSupportService, CLSID_ShowMoreSupportService, L"Show more service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE, CLSID_HomeTimelineService, L"Timeline service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_THREAD, CLSID_ThreadService, L"Timeline queue thread service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_QUEUE, CLSID_TimelineQueueService, L"Timeline queue service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ImageManagerService, CLSID_ImageManagerService, L"Image manager service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineImageService, CLSID_TimelineImageService, L"Timeline control image service for home timeline control"));
    RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineCleanupService, CLSID_TimelineCleanupService, L"Timeline control clenup service"));
    RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TabbedControlStatusService, CLSID_TabbedControlStatusService, L"Tabbed control animation and errors service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineRelativeTimeService, CLSID_TimelineRelativeTimeService, L"Timeline control relative time update service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_OPEN_URLS, CLSID_OpenUrlService, L"Open url service for home timeline control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOME_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_HomeTimelineControllerService, CLSID_HomeTimelineControllerService, L"Home timeline controller service"));

	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_PICTUREWINDOW_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ImageManagerService, CLSID_ImageManagerService, L"Image manager service for picture window"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_PICTUREWINDOW_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ThreadPoolService, CLSID_ThreadPoolService, L"Thread pool service for picture window"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_PICTUREWINDOW_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_DownloadService, CLSID_DownloadService, L"Download service for picture window"));
	
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ThreadPoolService, CLSID_ThreadPoolService, L"Thread pool service for user info control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_DownloadService, CLSID_DownloadService, L"Download service user for info control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ImageManagerService, CLSID_ImageManagerService, L"Image manager service for user info control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_UPDATEIMAGES_TIMER, CLSID_TimerService, L"Timer update images service for user info control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_UPDATE_THREAD, CLSID_ThreadService, L"Timeline thread service for user info control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_SHOWMORE_THREAD, CLSID_ThreadService, L"Timeline thread show more items service for user info control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineLoadingService, CLSID_TimelineLoadingService, L"Timeline loading service for user info control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ShowMoreSupportService, CLSID_ShowMoreSupportService, L"Show more service for user info control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE, CLSID_UserTimelineService, L"Timeline service for user info control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_THREAD, CLSID_ThreadService, L"Timeline queue thread service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_QUEUE, CLSID_TimelineQueueService, L"Timeline queue service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineImageService, CLSID_TimelineImageService, L"Timeline control image service for user info control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TabbedControlStatusService, CLSID_TabbedControlStatusService, L"Tabbed control animation and errors service for user info control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineRelativeTimeService, CLSID_TimelineRelativeTimeService, L"Timeline control relative time update service for user control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_OPEN_URLS, CLSID_OpenUrlService, L"Open url service for user info control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_FOLLOW_STATUS_THREAD, CLSID_ThreadService, L"Thread service for follow status service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_FollowStatusService, CLSID_FollowStatusService, L"Follow status service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_FOLLOW_THREAD, CLSID_ThreadService, L"Thread service for follow service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_FollowService, CLSID_FollowService, L"Follow service"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_GETUSER_THREAD, CLSID_ThreadService, L"Thread service for get user service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_USERINFO_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_GetUserService, CLSID_GetUserService, L"Get user service"));

	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_TWITVIEW_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ThreadPoolService, CLSID_ThreadPoolService, L"Thread pool service for twit view control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_TWITVIEW_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_DownloadService, CLSID_DownloadService, L"Download service user for twit view control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_TWITVIEW_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ImageManagerService, CLSID_ImageManagerService, L"Image manager service for twit view control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_TWITVIEW_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_UPDATEIMAGES_TIMER, CLSID_TimerService, L"Timer update images service for twit view control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_TWITVIEW_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_UPDATE_THREAD, CLSID_ThreadService, L"Timeline thread service for twit view control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_TWITVIEW_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE, CLSID_TwitViewRepliesService, L"Timeline service for twit view control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_TWITVIEW_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_THREAD, CLSID_ThreadService, L"Timeline queue thread service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_TWITVIEW_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_QUEUE, CLSID_TimelineQueueService, L"Timeline queue service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_TWITVIEW_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineImageService, CLSID_TimelineImageService, L"Timeline control image service for twit view control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_TWITVIEW_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TabbedControlStatusService, CLSID_TabbedControlStatusService, L"Tabbed control animation and errors service for twit view control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_TWITVIEW_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineRelativeTimeService, CLSID_TimelineRelativeTimeService, L"Timeline control relative time update service for twit view control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_TWITVIEW_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_OPEN_URLS, CLSID_OpenUrlService, L"Open url service for twit view control"));

	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_TIMELINE_SEARCH_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ThreadPoolService, CLSID_ThreadPoolService, L"Thread pool service for search control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_TIMELINE_SEARCH_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_DownloadService, CLSID_DownloadService, L"Download service user for search control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_TIMELINE_SEARCH_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ImageManagerService, CLSID_ImageManagerService, L"Image manager service for search control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_TIMELINE_SEARCH_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_UPDATEIMAGES_TIMER, CLSID_TimerService, L"Timer update images service for search control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_TIMELINE_SEARCH_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_UPDATE_THREAD, CLSID_ThreadService, L"Timeline thread service for search control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_TIMELINE_SEARCH_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE, CLSID_SearchTimelineService, L"Timeline service for search control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_TIMELINE_SEARCH_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_THREAD, CLSID_ThreadService, L"Timeline queue thread service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_TIMELINE_SEARCH_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_QUEUE, CLSID_TimelineQueueService, L"Timeline queue service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_TIMELINE_SEARCH_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineLoadingService, CLSID_TimelineLoadingService, L"Timeline loading service for search control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_TIMELINE_SEARCH_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineImageService, CLSID_TimelineImageService, L"Timeline control image service for search control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_TIMELINE_SEARCH_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TabbedControlStatusService, CLSID_TabbedControlStatusService, L"Tabbed control animation and errors service for search control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_TIMELINE_SEARCH_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineRelativeTimeService, CLSID_TimelineRelativeTimeService, L"Timeline control relative time update service for search control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_TIMELINE_SEARCH_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_OPEN_URLS, CLSID_OpenUrlService, L"Open url service for search control"));

	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_LISTS_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_UPDATE_THREAD, CLSID_ThreadService, L"Timeline thread service for lists control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_LISTS_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE, CLSID_ListsTimelineService, L"Timeline service for lists control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_LISTS_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_THREAD, CLSID_ThreadService, L"Timeline queue thread service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_LISTS_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_QUEUE, CLSID_TimelineQueueService, L"Timeline queue service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_LISTS_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineLoadingService, CLSID_TimelineLoadingService, L"Timeline loading service for lists control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_LISTS_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ImageManagerService, CLSID_ImageManagerService, L"Image manager service for lists control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_LISTS_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ThreadPoolService, CLSID_ThreadPoolService, L"Thread pool service for lists control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_LISTS_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TabbedControlStatusService, CLSID_TabbedControlStatusService, L"Tabbed control animation and errors service for lists control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_LISTS_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_UPDATEIMAGES_TIMER, CLSID_TimerService, L"Timer update images service for lists control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_LISTS_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineImageService, CLSID_TimelineImageService, L"Timeline control image service for lists control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_LISTS_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_OPEN_URLS, CLSID_OpenUrlService, L"Open url service for list timeline control"));

	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ThreadPoolService, CLSID_ThreadPoolService, L"Thread pool service for list timeline control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_DownloadService, CLSID_DownloadService, L"Download service user for list timeline control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ImageManagerService, CLSID_ImageManagerService, L"Image manager service for list timeline control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_UPDATEIMAGES_TIMER, CLSID_TimerService, L"Timer update images service for list timeline control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_UPDATE_THREAD, CLSID_ThreadService, L"Timeline thread service for list timeline control"));
    RETURN_IF_FAILED(AddPluginInfo(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ListTimelineStreamingServiceEventListener, CLSID_ListTimelineStreamingServiceEventListener, L"Timeline streaming service event listener for list timeline control"));
    RETURN_IF_FAILED(AddPluginInfo(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE, CLSID_ListTimelineControlService, L"Timeline service for list timeline control"));
	RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_THREAD, CLSID_ThreadService, L"Timeline queue thread service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_QUEUE, CLSID_TimelineQueueService, L"Timeline queue service"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineLoadingService, CLSID_TimelineLoadingService, L"Timeline loading service for list timeline control"));
    RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_SHOWMORE_THREAD, CLSID_ThreadService, L"Timeline thread show more items service"));
    RETURN_IF_FAILED(AddPluginInfo(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ShowMoreSupportService, CLSID_ShowMoreSupportService, L"Show more service for list timeline control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineImageService, CLSID_TimelineImageService, L"Timeline control image service for list timeline control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TabbedControlStatusService, CLSID_TabbedControlStatusService, L"Tabbed control animation and errors service for list timeline control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineRelativeTimeService, CLSID_TimelineRelativeTimeService, L"Timeline control relative time update service for list timeline control"));
	RETURN_IF_FAILED(AddPluginInfo(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_OPEN_URLS, CLSID_OpenUrlService, L"Open url service for list timeline control"));
    RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_CLENUP_TIMER, CLSID_TimerService, L"Timeline control items clenup timer service"));
    RETURN_IF_FAILED(AddPluginInfo(PNAMESP_LIST_TIMELINE_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineCleanupService, CLSID_TimelineCleanupService, L"Timeline control clenup service for list timeline control"));

    RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_FOLLOWING_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ThreadPoolService, CLSID_ThreadPoolService, L"Thread pool service for following control"));
    RETURN_IF_FAILED(AddPluginInfo(PNAMESP_FOLLOWING_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_DownloadService, CLSID_DownloadService, L"Download service user for following control"));
    RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_FOLLOWING_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ImageManagerService, CLSID_ImageManagerService, L"Image manager service for following control"));
    RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_FOLLOWING_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_UPDATEIMAGES_TIMER, CLSID_TimerService, L"Timer update images service for following control"));
    RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_FOLLOWING_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_UPDATE_THREAD, CLSID_ThreadService, L"Timeline thread service for following control"));
    RETURN_IF_FAILED(AddPluginInfo(PNAMESP_FOLLOWING_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE, CLSID_FollowingControlService, L"Timeline service for following control"));
    RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_FOLLOWING_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_THREAD, CLSID_ThreadService, L"Timeline queue thread service"));
    RETURN_IF_FAILED(AddPluginInfo(PNAMESP_FOLLOWING_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_QUEUE, CLSID_TimelineQueueService, L"Timeline queue service"));
    RETURN_IF_FAILED(AddPluginInfo(PNAMESP_FOLLOWING_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineLoadingService, CLSID_TimelineLoadingService, L"Timeline loading service for following control"));
    RETURN_IF_FAILED(AddPluginInfo(PNAMESP_FOLLOWING_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineImageService, CLSID_TimelineImageService, L"Timeline control image service for following control"));
    RETURN_IF_FAILED(AddPluginInfo(PNAMESP_FOLLOWING_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TabbedControlStatusService, CLSID_TabbedControlStatusService, L"Tabbed control animation and errors service for following control"));
    RETURN_IF_FAILED(AddPluginInfo(PNAMESP_FOLLOWING_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_TimelineRelativeTimeService, CLSID_TimelineRelativeTimeService, L"Timeline control relative time update service for following control"));
    RETURN_IF_FAILED(AddPluginInfo(PNAMESP_FOLLOWING_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_OPEN_URLS, CLSID_OpenUrlService, L"Open url service for following timeline control"));
    RETURN_IF_FAILED(AddPluginInfo(PNAMESP_FOLLOWING_CONTROL, PVIEWTYPE_WINDOW_SERVICE, CLSID_ShowMoreSupportService, CLSID_ShowMoreSupportService, L"Show more service for following timeline control"));
    RETURN_IF_FAILED(AddPluginInfoRemote(PNAMESP_FOLLOWING_CONTROL, PVIEWTYPE_WINDOW_SERVICE, SERVICE_TIMELINE_SHOWMORE_THREAD, CLSID_ThreadService, L"Timeline thread show more items service for following timeline control"));

	return S_OK;
}
