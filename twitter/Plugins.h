#pragma once

#ifndef __TWITTER__
DEFINE_GUID(CLSID_TimelineControl, 0x75E49C09, 0x05E4, 0x4643, 0x98, 0x3F, 0x62, 0x4D, 0xC0, 0x73, 0x53, 0x5A);
#endif

#define WM_TWIITER_FIRST_MSG WM_NOTIFIERHOST_LAST_MSG + 1
#define NM_LISTBOX_LCLICK WM_TWIITER_FIRST_MSG + 1
#define NM_LISTBOX_RCLICK WM_TWIITER_FIRST_MSG + 2
#define NM_ITEM_REMOVED WM_TWIITER_FIRST_MSG + 3
#define WM_ANIMATION_TIMER WM_TWIITER_FIRST_MSG + 4
#define WM_TWIITER_LAST_MSG WM_TWIITER_FIRST_MSG + 100

// {C2F9FEC6-D542-4601-B7DE-70B1FE19D67C}
DEFINE_GUID(PNAMESP_TIMELINE_CONTROL, 0xc2f9fec6, 0xd542, 0x4601, 0xb7, 0xde, 0x70, 0xb1, 0xfe, 0x19, 0xd6, 0x7c);
// {0C51F0CB-EDD6-4920-9D75-26C27DAE7DA0}
DEFINE_GUID(PNAMESP_PICTUREWINDOW_CONTROL, 0xc51f0cb, 0xedd6, 0x4920, 0x9d, 0x75, 0x26, 0xc2, 0x7d, 0xae, 0x7d, 0xa0);
// {6E6A68FC-7346-4C12-A93D-5FFBEEB30BDD}
DEFINE_GUID(PNAMESP_USERINFO_CONTROL, 0x6e6a68fc, 0x7346, 0x4c12, 0xa9, 0x3d, 0x5f, 0xfb, 0xee, 0xb3, 0xb, 0xdd);

// {9C884E2A-2CBC-41EB-A271-FAA6D6000F62}
DEFINE_GUID(SERVICE_FORMS_SERVICE, 0x9c884e2a, 0x2cbc, 0x41eb, 0xa2, 0x71, 0xfa, 0xa6, 0xd6, 0x0, 0xf, 0x62);
// {B6DB4C1A-7BDD-4BE8-81CF-6BF91864C4F4}
DEFINE_GUID(SERVICE_OPEN_URLS, 0xb6db4c1a, 0x7bdd, 0x4be8, 0x81, 0xcf, 0x6b, 0xf9, 0x18, 0x64, 0xc4, 0xf4);
// {B46C02E7-AD44-4CBA-B35D-B026163B7628}
DEFINE_GUID(SERVICE_TIMELINE_THREAD, 0xb46c02e7, 0xad44, 0x4cba, 0xb3, 0x5d, 0xb0, 0x26, 0x16, 0x3b, 0x76, 0x28);
// {29D60E05-FE71-47A2-8CB9-BFE9E76E1490}
DEFINE_GUID(SERVICE_TIMELINE_SHOWMORE_THREAD, 0x29d60e05, 0xfe71, 0x47a2, 0x8c, 0xb9, 0xbf, 0xe9, 0xe7, 0x6e, 0x14, 0x90);
// {935D79BB-2A0F-481A-A36E-3EDCAAC942EB}
DEFINE_GUID(SERVICE_AUTH_THREAD, 0x935d79bb, 0x2a0f, 0x481a, 0xa3, 0x6e, 0x3e, 0xdc, 0xaa, 0xc9, 0x42, 0xeb);
// {1E41E265-8F00-4A72-B4AD-D63343B4DFF1}
DEFINE_GUID(SERVICE_TIMELINE_TIMER, 0x1e41e265, 0x8f00, 0x4a72, 0xb4, 0xad, 0xd6, 0x33, 0x43, 0xb4, 0xdf, 0xf1);
// {C814289F-34D3-4BDA-9A80-BB7ADBD16B31}
DEFINE_GUID(SERVICE_TIMELINE_CLENUP_TIMER, 0xc814289f, 0x34d3, 0x4bda, 0x9a, 0x80, 0xbb, 0x7a, 0xdb, 0xd1, 0x6b, 0x31);

// {C3E600B0-879B-4A0A-AC0C-5D2054E78A1F}
DEFINE_GUID(SERVICE_UPDATEIMAGES_TIMER, 0xc3e600b0, 0x879b, 0x4a0a, 0xac, 0xc, 0x5d, 0x20, 0x54, 0xe7, 0x8a, 0x1f);
// {6C18AD48-92D3-4693-8468-FF9C862B37C8}
DEFINE_GUID(SERVICE_UPDATES_TIMER, 0x6c18ad48, 0x92d3, 0x4693, 0x84, 0x68, 0xff, 0x9c, 0x86, 0x2b, 0x37, 0xc8);
// {34FB9A34-15BC-439D-B56B-8A56A9AB7DC1}
DEFINE_GUID(SERVICE_UPDATES_THREAD, 0x34fb9a34, 0x15bc, 0x439d, 0xb5, 0x6b, 0x8a, 0x56, 0xa9, 0xab, 0x7d, 0xc1);
// {A9EF7DE8-B384-45E7-8A4A-C1A7F6EE8461}
DEFINE_GUID(SERVICE_USERINFO_THREAD, 0xa9ef7de8, 0xb384, 0x45e7, 0x8a, 0x4a, 0xc1, 0xa7, 0xf6, 0xee, 0x84, 0x61);

// {8326819C-07EE-4CAB-B234-F67FFF6AAB98}
DEFINE_GUID(PTYPE_THEME, 0x8326819c, 0x7ee, 0x4cab, 0xb2, 0x34, 0xf6, 0x7f, 0xff, 0x6a, 0xab, 0x98);
// {DD5761EF-3151-435A-A06A-029951D933C8}
DEFINE_GUID(PTYPE_THEMECOLORMAP, 0xdd5761ef, 0x3151, 0x435a, 0xa0, 0x6a, 0x2, 0x99, 0x51, 0xd9, 0x33, 0xc8);

// {5F6E5739-B25C-4376-BEF8-5CEC9C16603D}
DEFINE_GUID(THEME_DEFAULT, 0x5f6e5739, 0xb25c, 0x4376, 0xbe, 0xf8, 0x5c, 0xec, 0x9c, 0x16, 0x60, 0x3d);

// {3F084ED1-2FEF-4AEE-B40E-FB663D8F1309}
DEFINE_GUID(COMMAND_COPY_TEXT, 0x3f084ed1, 0x2fef, 0x4aee, 0xb4, 0xe, 0xfb, 0x66, 0x3d, 0x8f, 0x13, 0x9);
// {EDFD49B8-1543-456E-B3A8-D46A0540F186}
DEFINE_GUID(COMMAND_COPY_URL, 0xedfd49b8, 0x1543, 0x456e, 0xb3, 0xa8, 0xd4, 0x6a, 0x5, 0x40, 0xf1, 0x86);
// {8E2B59BD-8269-41C2-AB75-915D06B3A3A0}
DEFINE_GUID(COMMAND_COPY_IMAGE_URL, 0x8e2b59bd, 0x8269, 0x41c2, 0xab, 0x75, 0x91, 0x5d, 0x6, 0xb3, 0xa3, 0xa0);

#define SETTINGS_PATH L"Twitter"
#define SETTINGS_PATH_THEMES L"Twitter"

#define KEY_PIN L"Pin"
#define KEY_USER L"User"
#define KEY_PASSWORD L"Password"
#define KEY_TWITTERKEY L"TwitterKey"
#define KEY_TWITTERSECRET L"TwitterSecret"

#define VAR_RESULT L"VAR_RESULT"
#define VAR_URL L"VAR_URL"
#define VAR_FILE_EXT L"VAR_FILE_EXT"
#define VAR_KEEP_FILE L"VAR_KEEP_FILE"
#define VAR_FILEPATH L"VAR_FILEPATH"
#define VAR_TWITTER_RELATIVE_TIME L"VAR_TWITTER_RELATIVE_TIME"
#define VAR_COLUMN_SHOW_MORE L"VAR_COLUMN_SHOW_MORE"

#define VAR_COLUMN_NAME L"VAR_COLUMN_NAME"
#define VAR_ITEM_INDEX L"VAR_ITEM_INDEX"

#define VAR_BRUSH_SELECTED L"VAR_BRUSH_SELECTED"
#define VAR_BRUSH_BACKGROUND L"VAR_BRUSH_BACKGROUND"
#define VAR_TAB_HEADER L"VAR_TAB_HEADER"
#define VAR_TAB_HEADER_SELECTED L"VAR_TAB_HEADER_SELECTED"

#define VAR_SELECTED_POSTFIX L"SELECTED"
#define VAR_ITEM_DISABLED L"VAR_ITEM_DISABLED"
#define VAR_ITEM_SELECTION_DISABLED L"VAR_ITEM_SELECTION_DISABLED"
#define VAR_ITEM_ANIMATION_INACTIVE L"VAR_ITEM_ANIMATION_INACTIVE"
#define VAR_ITEM_ANIMATION_ACTIVE L"VAR_ITEM_ANIMATION_ACTIVE"

#define TYPE_IMAGE L"TYPE_IMAGE"
#define TYPE_IMAGE_PICTURE_WINDOW L"TYPE_IMAGE_PICTURE_WINDOW"
#define TYPE_SOFTWARE_UPDATE_VERSION L"TYPE_SOFTWARE_UPDATE_VERSION"
#define TYPE_SOFTWARE_UPDATE_MSI L"TYPE_SOFTWARE_UPDATE_MSI"
#define TYPE_SHOWMORE_OBJECT L"TYPE_SHOWMORE_OBJECT"

#define VAR_SETTINGS_TIMELINE_PATH L"Timeline"
#define VAR_SETTINGS_TIMELINE_DISABLE_ANIMATION L"DisableAnimation"