

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sun Feb 01 13:02:46 2015
 */
/* Compiler settings for twview.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __twview_i_h__
#define __twview_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __PluginTable_FWD_DEFINED__
#define __PluginTable_FWD_DEFINED__

#ifdef __cplusplus
typedef class PluginTable PluginTable;
#else
typedef struct PluginTable PluginTable;
#endif /* __cplusplus */

#endif 	/* __PluginTable_FWD_DEFINED__ */


#ifndef __TimelineControl_FWD_DEFINED__
#define __TimelineControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class TimelineControl TimelineControl;
#else
typedef struct TimelineControl TimelineControl;
#endif /* __cplusplus */

#endif 	/* __TimelineControl_FWD_DEFINED__ */


#ifndef __SettingsControl_FWD_DEFINED__
#define __SettingsControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class SettingsControl SettingsControl;
#else
typedef struct SettingsControl SettingsControl;
#endif /* __cplusplus */

#endif 	/* __SettingsControl_FWD_DEFINED__ */


#ifndef __TimelineControlCopyCommand_FWD_DEFINED__
#define __TimelineControlCopyCommand_FWD_DEFINED__

#ifdef __cplusplus
typedef class TimelineControlCopyCommand TimelineControlCopyCommand;
#else
typedef struct TimelineControlCopyCommand TimelineControlCopyCommand;
#endif /* __cplusplus */

#endif 	/* __TimelineControlCopyCommand_FWD_DEFINED__ */


#ifndef __PictureWindow_FWD_DEFINED__
#define __PictureWindow_FWD_DEFINED__

#ifdef __cplusplus
typedef class PictureWindow PictureWindow;
#else
typedef struct PictureWindow PictureWindow;
#endif /* __cplusplus */

#endif 	/* __PictureWindow_FWD_DEFINED__ */


#ifndef __PictureWindowCopyCommand_FWD_DEFINED__
#define __PictureWindowCopyCommand_FWD_DEFINED__

#ifdef __cplusplus
typedef class PictureWindowCopyCommand PictureWindowCopyCommand;
#else
typedef struct PictureWindowCopyCommand PictureWindowCopyCommand;
#endif /* __cplusplus */

#endif 	/* __PictureWindowCopyCommand_FWD_DEFINED__ */


#ifndef __CustomTabControl_FWD_DEFINED__
#define __CustomTabControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class CustomTabControl CustomTabControl;
#else
typedef struct CustomTabControl CustomTabControl;
#endif /* __cplusplus */

#endif 	/* __CustomTabControl_FWD_DEFINED__ */


#ifndef __UserInfoControl_FWD_DEFINED__
#define __UserInfoControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class UserInfoControl UserInfoControl;
#else
typedef struct UserInfoControl UserInfoControl;
#endif /* __cplusplus */

#endif 	/* __UserInfoControl_FWD_DEFINED__ */


#ifndef __UserAccountControl_FWD_DEFINED__
#define __UserAccountControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class UserAccountControl UserAccountControl;
#else
typedef struct UserAccountControl UserAccountControl;
#endif /* __cplusplus */

#endif 	/* __UserAccountControl_FWD_DEFINED__ */


#ifndef __HomeTimeLineControl_FWD_DEFINED__
#define __HomeTimeLineControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class HomeTimeLineControl HomeTimeLineControl;
#else
typedef struct HomeTimeLineControl HomeTimeLineControl;
#endif /* __cplusplus */

#endif 	/* __HomeTimeLineControl_FWD_DEFINED__ */


#ifndef __TwitViewControl_FWD_DEFINED__
#define __TwitViewControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class TwitViewControl TwitViewControl;
#else
typedef struct TwitViewControl TwitViewControl;
#endif /* __cplusplus */

#endif 	/* __TwitViewControl_FWD_DEFINED__ */


#ifndef __TimelineControlOpenInBrowserCommand_FWD_DEFINED__
#define __TimelineControlOpenInBrowserCommand_FWD_DEFINED__

#ifdef __cplusplus
typedef class TimelineControlOpenInBrowserCommand TimelineControlOpenInBrowserCommand;
#else
typedef struct TimelineControlOpenInBrowserCommand TimelineControlOpenInBrowserCommand;
#endif /* __cplusplus */

#endif 	/* __TimelineControlOpenInBrowserCommand_FWD_DEFINED__ */


#ifndef __SearchControl_FWD_DEFINED__
#define __SearchControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class SearchControl SearchControl;
#else
typedef struct SearchControl SearchControl;
#endif /* __cplusplus */

#endif 	/* __SearchControl_FWD_DEFINED__ */


#ifndef __SearchTimelineControl_FWD_DEFINED__
#define __SearchTimelineControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class SearchTimelineControl SearchTimelineControl;
#else
typedef struct SearchTimelineControl SearchTimelineControl;
#endif /* __cplusplus */

#endif 	/* __SearchTimelineControl_FWD_DEFINED__ */


#ifndef __PictureWindowSaveCommand_FWD_DEFINED__
#define __PictureWindowSaveCommand_FWD_DEFINED__

#ifdef __cplusplus
typedef class PictureWindowSaveCommand PictureWindowSaveCommand;
#else
typedef struct PictureWindowSaveCommand PictureWindowSaveCommand;
#endif /* __cplusplus */

#endif 	/* __PictureWindowSaveCommand_FWD_DEFINED__ */


#ifndef __ListsControl_FWD_DEFINED__
#define __ListsControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class ListsControl ListsControl;
#else
typedef struct ListsControl ListsControl;
#endif /* __cplusplus */

#endif 	/* __ListsControl_FWD_DEFINED__ */


#ifndef __ListTimelineControl_FWD_DEFINED__
#define __ListTimelineControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class ListTimelineControl ListTimelineControl;
#else
typedef struct ListTimelineControl ListTimelineControl;
#endif /* __cplusplus */

#endif 	/* __ListTimelineControl_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "objmdl_contract_i.h"
#include "asyncsvc_contract_i.h"
#include "viewmdl_contract_i.h"
#include "twitter_contract_i.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __twviewLib_LIBRARY_DEFINED__
#define __twviewLib_LIBRARY_DEFINED__

/* library twviewLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_twviewLib;

EXTERN_C const CLSID CLSID_PluginTable;

#ifdef __cplusplus

class DECLSPEC_UUID("6EA78630-DC6D-4D2B-91F0-686B260F29D6")
PluginTable;
#endif

EXTERN_C const CLSID CLSID_TimelineControl;

#ifdef __cplusplus

class DECLSPEC_UUID("75E49C09-05E4-4643-983F-624DC073535A")
TimelineControl;
#endif

EXTERN_C const CLSID CLSID_SettingsControl;

#ifdef __cplusplus

class DECLSPEC_UUID("6AFA6FA2-12D6-450E-AEFF-60B66F8B7574")
SettingsControl;
#endif

EXTERN_C const CLSID CLSID_TimelineControlCopyCommand;

#ifdef __cplusplus

class DECLSPEC_UUID("F09290A9-AF90-416D-9309-C649ECC97935")
TimelineControlCopyCommand;
#endif

EXTERN_C const CLSID CLSID_PictureWindow;

#ifdef __cplusplus

class DECLSPEC_UUID("10AEB079-CE14-4F8F-8CEE-C4C6428BD4E8")
PictureWindow;
#endif

EXTERN_C const CLSID CLSID_PictureWindowCopyCommand;

#ifdef __cplusplus

class DECLSPEC_UUID("5A272730-A01D-48FD-9005-A308F656F12D")
PictureWindowCopyCommand;
#endif

EXTERN_C const CLSID CLSID_CustomTabControl;

#ifdef __cplusplus

class DECLSPEC_UUID("262329E2-B603-4FC0-97CB-40D95D41A1C0")
CustomTabControl;
#endif

EXTERN_C const CLSID CLSID_UserInfoControl;

#ifdef __cplusplus

class DECLSPEC_UUID("C65CC0F7-866D-40F6-89E5-652C0295F15C")
UserInfoControl;
#endif

EXTERN_C const CLSID CLSID_UserAccountControl;

#ifdef __cplusplus

class DECLSPEC_UUID("45B12203-6FD3-492E-858A-7EA20454493B")
UserAccountControl;
#endif

EXTERN_C const CLSID CLSID_HomeTimeLineControl;

#ifdef __cplusplus

class DECLSPEC_UUID("80F4CF87-03B7-4929-8ABB-11DC7D57B99E")
HomeTimeLineControl;
#endif

EXTERN_C const CLSID CLSID_TwitViewControl;

#ifdef __cplusplus

class DECLSPEC_UUID("7C7062D1-A7B6-499B-B6FC-78B2458515DC")
TwitViewControl;
#endif

EXTERN_C const CLSID CLSID_TimelineControlOpenInBrowserCommand;

#ifdef __cplusplus

class DECLSPEC_UUID("03598148-FEBB-406A-BA6A-1092C957B600")
TimelineControlOpenInBrowserCommand;
#endif

EXTERN_C const CLSID CLSID_SearchControl;

#ifdef __cplusplus

class DECLSPEC_UUID("31699CC6-E397-45CB-BBD6-ECF80D9C343C")
SearchControl;
#endif

EXTERN_C const CLSID CLSID_SearchTimelineControl;

#ifdef __cplusplus

class DECLSPEC_UUID("DB25B344-8870-45CE-8C9E-CF6F26C32EBE")
SearchTimelineControl;
#endif

EXTERN_C const CLSID CLSID_PictureWindowSaveCommand;

#ifdef __cplusplus

class DECLSPEC_UUID("3D06F6F5-9BDC-47DA-BEE4-A8EEB57C3E57")
PictureWindowSaveCommand;
#endif

EXTERN_C const CLSID CLSID_ListsControl;

#ifdef __cplusplus

class DECLSPEC_UUID("B720FBFB-E607-463A-91C6-13D82741F4F2")
ListsControl;
#endif

EXTERN_C const CLSID CLSID_ListTimelineControl;

#ifdef __cplusplus

class DECLSPEC_UUID("F35D78C8-546F-4A35-A90C-3FD282925C5A")
ListTimelineControl;
#endif
#endif /* __twviewLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


