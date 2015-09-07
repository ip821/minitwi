#pragma once

namespace IP
{
	namespace Twitter
	{
		namespace Themes
		{
			namespace Metadata
			{
				namespace TabContainer
				{
					const static CComBSTR LayoutName(L"MainTabContainerHeaderLayout");
					const static CComBSTR InfoContainer(L"InfoContainer");
					const static CComBSTR InfoImage(L"InfoImage");
					const static CComBSTR MarqueeProgressBox(L"MarqueeProgressBox");

					namespace Images
					{
						const static CComBSTR Home(L"home");
						const static CComBSTR Lists(L"lists");
						const static CComBSTR Search(L"search");
						const static CComBSTR Settings(L"settings");
						const static CComBSTR Error(L"error");
						const static CComBSTR Info(L"info");
					}
				}

				namespace UserAccountControl
				{
					const static CComBSTR LayoutName(L"UserAccountControlLayout");
					const static CComBSTR FollowButtonText(L"TwitterItemFollowButton");
					const static CComBSTR FollowButtonContainer(L"FollowButtonContainer");
					const static CComBSTR UserAccountControlLayoutBackgroundImage(L"UserAccountControlLayoutBackgroundImage");
					const static CComBSTR TwitterUserImage(L"TwitterUserImage");
				}

				namespace TimelineControl
				{
					const static CComBSTR LayoutName(L"TimelineItem");
				}
			}
		}
	}
}