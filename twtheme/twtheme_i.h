

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sun Feb 01 13:03:09 2015
 */
/* Compiler settings for twtheme.idl:
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


#ifndef __twtheme_i_h__
#define __twtheme_i_h__

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


#ifndef __SkinTimeline_FWD_DEFINED__
#define __SkinTimeline_FWD_DEFINED__

#ifdef __cplusplus
typedef class SkinTimeline SkinTimeline;
#else
typedef struct SkinTimeline SkinTimeline;
#endif /* __cplusplus */

#endif 	/* __SkinTimeline_FWD_DEFINED__ */


#ifndef __ThemeColorMap_FWD_DEFINED__
#define __ThemeColorMap_FWD_DEFINED__

#ifdef __cplusplus
typedef class ThemeColorMap ThemeColorMap;
#else
typedef struct ThemeColorMap ThemeColorMap;
#endif /* __cplusplus */

#endif 	/* __ThemeColorMap_FWD_DEFINED__ */


#ifndef __ThemeFontMap_FWD_DEFINED__
#define __ThemeFontMap_FWD_DEFINED__

#ifdef __cplusplus
typedef class ThemeFontMap ThemeFontMap;
#else
typedef struct ThemeFontMap ThemeFontMap;
#endif /* __cplusplus */

#endif 	/* __ThemeFontMap_FWD_DEFINED__ */


#ifndef __SkinTabControl_FWD_DEFINED__
#define __SkinTabControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class SkinTabControl SkinTabControl;
#else
typedef struct SkinTabControl SkinTabControl;
#endif /* __cplusplus */

#endif 	/* __SkinTabControl_FWD_DEFINED__ */


#ifndef __SkinCommonControl_FWD_DEFINED__
#define __SkinCommonControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class SkinCommonControl SkinCommonControl;
#else
typedef struct SkinCommonControl SkinCommonControl;
#endif /* __cplusplus */

#endif 	/* __SkinCommonControl_FWD_DEFINED__ */


#ifndef __SkinUserAccountControl_FWD_DEFINED__
#define __SkinUserAccountControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class SkinUserAccountControl SkinUserAccountControl;
#else
typedef struct SkinUserAccountControl SkinUserAccountControl;
#endif /* __cplusplus */

#endif 	/* __SkinUserAccountControl_FWD_DEFINED__ */


#ifndef __ThemeDefault_FWD_DEFINED__
#define __ThemeDefault_FWD_DEFINED__

#ifdef __cplusplus
typedef class ThemeDefault ThemeDefault;
#else
typedef struct ThemeDefault ThemeDefault;
#endif /* __cplusplus */

#endif 	/* __ThemeDefault_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "objmdl_contract_i.h"
#include "viewmdl_contract_i.h"
#include "twitter_contract_i.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __twthemeLib_LIBRARY_DEFINED__
#define __twthemeLib_LIBRARY_DEFINED__

/* library twthemeLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_twthemeLib;

EXTERN_C const CLSID CLSID_PluginTable;

#ifdef __cplusplus

class DECLSPEC_UUID("6EA78630-DC6D-4D2B-91F0-686B260F29D6")
PluginTable;
#endif

EXTERN_C const CLSID CLSID_SkinTimeline;

#ifdef __cplusplus

class DECLSPEC_UUID("B682B92C-ACB3-446D-A37D-E11C901385CA")
SkinTimeline;
#endif

EXTERN_C const CLSID CLSID_ThemeColorMap;

#ifdef __cplusplus

class DECLSPEC_UUID("41F33BEE-3D82-4173-9F5E-79C91523843B")
ThemeColorMap;
#endif

EXTERN_C const CLSID CLSID_ThemeFontMap;

#ifdef __cplusplus

class DECLSPEC_UUID("986AF532-E457-4A2D-894D-0805AB6BD0B1")
ThemeFontMap;
#endif

EXTERN_C const CLSID CLSID_SkinTabControl;

#ifdef __cplusplus

class DECLSPEC_UUID("A4B1D02D-648F-4D71-9907-7B84B9DF2422")
SkinTabControl;
#endif

EXTERN_C const CLSID CLSID_SkinCommonControl;

#ifdef __cplusplus

class DECLSPEC_UUID("D9BF55FB-091B-4858-82EC-EBD0137D04FB")
SkinCommonControl;
#endif

EXTERN_C const CLSID CLSID_SkinUserAccountControl;

#ifdef __cplusplus

class DECLSPEC_UUID("0B2EE239-BF11-41FF-8DF0-525232084378")
SkinUserAccountControl;
#endif

EXTERN_C const CLSID CLSID_ThemeDefault;

#ifdef __cplusplus

class DECLSPEC_UUID("A7C215DD-BB09-4045-9029-9EE9D1D0A389")
ThemeDefault;
#endif
#endif /* __twthemeLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


