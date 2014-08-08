

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Thu Aug 07 23:09:14 2014
 */
/* Compiler settings for twitter.idl:
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


#ifndef __twitter_i_h__
#define __twitter_i_h__

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


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "objmdl_contract_i.h"
#include "viewmdl_contract_i.h"
#include "shobjidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __twitterLib_LIBRARY_DEFINED__
#define __twitterLib_LIBRARY_DEFINED__

/* library twitterLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_twitterLib;

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
#endif /* __twitterLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


