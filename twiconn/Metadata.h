#pragma once

namespace IP
{
	namespace Twitter
	{
		namespace Connection
		{
			namespace Metadata
			{
				namespace MediaObject
				{
					const static CComBSTR MediaUrl(L"VAR_TWITTER_MEDIAURL");
					const static CComBSTR MediaUrlShort(L"VAR_TWITTER_MEDIAURL_SHORT");
					const static CComBSTR MediaUrlThumb(L"VAR_TWITTER_MEDIAURL_THUMB");
				}

				namespace ListObject
				{
					const static CComBSTR TypeId(L"TYPE_LIST_OBJECT");
				}

				namespace TweetObject
				{
					const static CComBSTR TypeId(L"TYPE_TWITTER_OBJECT");
					const static CComBSTR UserObject(L"VAR_TWITTER_USER_OBJECT");
					const static CComBSTR Image(L"VAR_TWITTER_IMAGE");
					const static CComBSTR Text(L"VAR_TWITTER_TEXT");
					const static CComBSTR NormalizedText(L"VAR_TWITTER_NORMALIZED_TEXT");
					const static CComBSTR Url(L"VAR_TWITTER_URL");
					const static CComBSTR MediaUrls(L"VAR_TWITTER_MEDIAURLS");
					const static CComBSTR Urls(L"VAR_TWITTER_URLS");
					const static CComBSTR CreatedAt(L"VAR_TWITTER_CREATED_AT");
					const static CComBSTR RetweetedUserDisplayName(L"VAR_TWITTER_RETWEETED_USER_DISPLAY_NAME");
					const static CComBSTR RetweetedUserName(L"VAR_TWITTER_RETWEETED_USER_NAME");
					const static CComBSTR RetweetedUserObject(L"VAR_TWITTER_RETWEETED_USER_OBJECT");
					const static CComBSTR InReplyToStatusId(L"VAR_TWITTER_IN_REPLYTO_STATUS_ID");
					const static CComBSTR OriginalId(L"VAR_TWITTER_ORIGINAL_ID");
				}

				namespace UserObject
				{
					const static CComBSTR DisplayName(L"VAR_TWITTER_USER_DISPLAY_NAME");
					const static CComBSTR Name(L"VAR_TWITTER_USER_NAME");
					const static CComBSTR Image(L"VAR_TWITTER_USER_IMAGE");
					const static CComBSTR BackColor(L"VAR_TWITTER_USER_BACKCOLOR");
					const static CComBSTR ForeColor(L"VAR_TWITTER_USER_FORECOLOR");
					const static CComBSTR Banner(L"VAR_TWITTER_USER_BANNER");
					const static CComBSTR FollowersCount(L"VAR_TWITTER_USER_FOLLOWERS_COUNT");
					const static CComBSTR FriendsCount(L"VAR_TWITTER_USER_FRIENDS_COUNT");
					const static CComBSTR TweetsCount(L"VAR_TWITTER_USER_TWEETS_COUNT");
					const static CComBSTR Description(L"VAR_TWITTER_USER_DESCRIPTION");
					const static CComBSTR DisplayNameUserAccount(L"VAR_TWITTER_USER_DISPLAY_NAME_USER_ACCOUNT");
				}

				namespace FriendshipObject
				{
					const static CComBSTR IsFollowing(L"VAR_TWITTER_ISFOLLOWING");
				}
			}
		}
	}
}