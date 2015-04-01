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
					const static CComBSTR Password(L"Password");
					const static CComBSTR TwitterKey(L"TwitterKey");
					const static CComBSTR TwitterSecret(L"TwitterSecret");
				}
			}

			namespace Types
			{
				const static CComBSTR Image(L"TYPE_IMAGE");
				const static CComBSTR ImagePictureWindow(L"TYPE_IMAGE_PICTURE_WINDOW");
				const static CComBSTR VideoPictureWindow(L"TYPE_VIDEO_PICTURE_WINDOW");
				const static CComBSTR ImageUserBanner(L"TYPE_IMAGE_USER_BANNER");
				const static CComBSTR ImageUserImage(L"TYPE_IMAGE_USER_IMAGE");
				const static CComBSTR SoftwareUpdateVersion(L"TYPE_SOFTWARE_UPDATE_VERSION");
				const static CComBSTR SoftwareUpdateMsi(L"TYPE_SOFTWARE_UPDATE_MSI");
				const static CComBSTR CustomTimelineObject(L"TYPE_CUSTOM_TIMELINE_OBJECT");
			}

			namespace Item
			{
				const static CComBSTR VAR_SELECTED_POSTFIX(L"SELECTED");
				const static CComBSTR VAR_DOUBLE_SIZE_POSTFIX(L"DOUBLE_SIZE");
				const static CComBSTR VAR_ITEM_DISABLED(L"VAR_ITEM_DISABLED");
				const static CComBSTR VAR_ITEM_DISABLED_TEXT(L"VAR_ITEM_DISABLED_TEXT");
				const static CComBSTR VAR_ITEM_DOUBLE_SIZE(L"VAR_ITEM_DOUBLE_SIZE");
				const static CComBSTR VAR_ITEM_SELECTION_DISABLED(L"VAR_ITEM_SELECTION_DISABLED");
				const static CComBSTR VAR_ITEM_ANIMATION_INACTIVE(L"VAR_ITEM_ANIMATION_INACTIVE");
				const static CComBSTR VAR_ITEM_ANIMATION_ACTIVE (L"VAR_ITEM_ANIMATION_ACTIVE");
				const static CComBSTR VAR_ITEM_FOLLOW_BUTTON(L"VAR_ITEM_FOLLOW_BUTTON");
				const static CComBSTR VAR_ITEM_FOLLOW_BUTTON_RECT(L"VAR_ITEM_FOLLOW_BUTTON_RECT");
				const static CComBSTR VAR_ITEM_FOLLOW_BUTTON_RECT_PUSHED(L"VAR_ITEM_FOLLOW_BUTTON_RECT_PUSHED");
				const static CComBSTR VAR_ITEM_FOLLOW_BUTTON_RECT_DISABLED(L"VAR_ITEM_FOLLOW_BUTTON_RECT_DISABLED");
				const static CComBSTR VAR_TWITTER_RELATIVE_TIME(L"VAR_TWITTER_RELATIVE_TIME");
				const static CComBSTR VAR_TWITTER_DELIMITER(L"VAR_TWITTER_DELIMITER");
				const static CComBSTR VAR_IS_URL(L"VAR_IS_URL");
				const static CComBSTR VAR_IS_IMAGE(L"VAR_IS_IMAGE");
				const static CComBSTR VAR_IS_WORDWRAP(L"VAR_IS_WORDWRAP");
				const static CComBSTR VAR_IS_FOLLOWING(L"VAR_IS_FOLLOWING");
				const static CComBSTR VAR_ITEM_INDEX(L"VAR_ITEM_INDEX");
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
				const static CComBSTR StreamObject(L"VAR_STREAM_OBJECT");
			}

			namespace Object
			{
				const static CComBSTR Result(L"VAR_RESULT");
				const static CComBSTR Url(L"VAR_URL");
				const static CComBSTR ParentResult(L"VAR_PARENT_RESULT");
				const static CComBSTR Text(L"VAR_TEXT");
				const static CComBSTR Value(L"VAR_VALUE");
				const static CComBSTR MaxId(L"VAR_MAX_ID");
				const static CComBSTR SinceId(L"VAR_SINCE_ID");
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
				const static CComBSTR PictureWindowText(L"VAR_PICTURE_WINDOW_TEXT");
				const static CComBSTR AccountControlText(L"VAR_ACCOUNT_CONTROL_TEXT");
			}
		}
	}
}


