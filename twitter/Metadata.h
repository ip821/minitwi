#pragma once 

namespace IP
{
	namespace Twitter
	{
		namespace Metadata
		{
			namespace Settings
			{
				const static CComBSTR PathRoot(L"Twitter");
				const static CComBSTR PathThemes(L"Themes");
				const static CComBSTR PathTimeline(L"Timeline");

				namespace Timeline
				{
					const static CComBSTR DisableAnimation(L"DisableAnimation");
				}

				namespace Twitter
				{
					const static CComBSTR Pin(L"Pin");
					const static CComBSTR User(L"User");
					const static CComBSTR TwitterKey(L"TwitterKey");
					const static CComBSTR TwitterSecret(L"TwitterSecret");
				}
			}

			namespace Types
			{
				const static CComBSTR Image(L"TYPE_IMAGE");
				const static CComBSTR ImagePictureWindow(L"TYPE_IMAGE_PICTURE_WINDOW");
				const static CComBSTR ImageUserBanner(L"TYPE_IMAGE_USER_BANNER");
				const static CComBSTR ImageUserImage(L"TYPE_IMAGE_USER_IMAGE");
				const static CComBSTR SoftwareUpdateVersion(L"TYPE_SOFTWARE_UPDATE_VERSION");
				const static CComBSTR SoftwareUpdateMsi(L"TYPE_SOFTWARE_UPDATE_MSI");
				const static CComBSTR CustomTimelineObject(L"TYPE_CUSTOM_TIMELINE_OBJECT");
			}

			namespace Item
			{
#define VAR_SELECTED_POSTFIX L"SELECTED"
#define VAR_ITEM_DISABLED L"VAR_ITEM_DISABLED"
#define VAR_ITEM_DISABLED_TEXT L"VAR_ITEM_DISABLED_TEXT"
#define VAR_ITEM_DOUBLE_SIZE L"VAR_ITEM_DOUBLE_SIZE"
#define VAR_DOUBLE_SIZE_POSTFIX L"DOUBLE_SIZE"
#define VAR_ITEM_SELECTION_DISABLED L"VAR_ITEM_SELECTION_DISABLED"
#define VAR_ITEM_ANIMATION_INACTIVE L"VAR_ITEM_ANIMATION_INACTIVE"
#define VAR_ITEM_ANIMATION_ACTIVE L"VAR_ITEM_ANIMATION_ACTIVE"
#define VAR_ITEM_FOLLOW_BUTTON L"VAR_ITEM_FOLLOW_BUTTON"
#define VAR_ITEM_FOLLOW_BUTTON_RECT L"VAR_ITEM_FOLLOW_BUTTON_RECT"
#define VAR_ITEM_FOLLOW_BUTTON_RECT_PUSHED L"VAR_ITEM_FOLLOW_BUTTON_RECT_PUSHED"
#define VAR_ITEM_FOLLOW_BUTTON_RECT_DISABLED L"VAR_ITEM_FOLLOW_BUTTON_RECT_DISABLED"
#define VAR_TWITTER_RELATIVE_TIME L"VAR_TWITTER_RELATIVE_TIME"
#define VAR_TWITTER_DELIMITER L"VAR_TWITTER_DELIMITER"
#define VAR_IS_URL L"VAR_IS_URL"
#define VAR_IS_IMAGE L"VAR_IS_IMAGE"
#define VAR_IS_WORDWRAP L"VAR_IS_WORDWRAP"
#define VAR_ITEM_INDEX L"VAR_ITEM_INDEX"
			}

			namespace Column
			{
				const static CComBSTR ShowMoreColumn(L"VAR_COLUMN_SHOW_MORE");
				const static CComBSTR Name(L"VAR_COLUMN_NAME");
			}

			namespace File
			{
				const static CComBSTR Extension(L"VAR_FILE_EXT");
				const static CComBSTR KeepFileFlag(L"VAR_KEEP_FILE");
				const static CComBSTR Path(L"VAR_FILEPATH");
			}

			namespace Object
			{
#define VAR_RESULT L"VAR_RESULT"
#define VAR_URL L"VAR_URL"
#define VAR_PARENT_RESULT L"VAR_PARENT_RESULT"
#define VAR_TEXT L"VAR_TEXT"
#define VAR_VALUE L"VAR_VALUE"
#define VAR_MAX_ID L"VAR_MAX_ID"
#define VAR_SINCE_ID L"VAR_SINCE_ID"
			}

			namespace Tabs
			{
				const static CComBSTR Header(L"VAR_TAB_HEADER");
				const static CComBSTR HeaderSelected(L"VAR_TAB_HEADER_SELECTED");
			}

			namespace Drawing
			{
				const static CComBSTR BrushSelected(L"VAR_BRUSH_SELECTED");
				const static CComBSTR BrushBackground(L"VAR_BRUSH_BACKGROUND");
			}
		}
	}
}

#define VAR_PICTURE_WINDOW_TEXT L"VAR_PICTURE_WINDOW_TEXT"
#define VAR_ACCOUNT_CONTROL_TEXT L"VAR_ACCOUNT_CONTROL_TEXT"

