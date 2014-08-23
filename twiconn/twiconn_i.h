

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Fri Aug 22 20:55:19 2014
 */
/* Compiler settings for twiconn.idl:
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


#ifndef __twiconn_i_h__
#define __twiconn_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __TwitterConnection_FWD_DEFINED__
#define __TwitterConnection_FWD_DEFINED__

#ifdef __cplusplus
typedef class TwitterConnection TwitterConnection;
#else
typedef struct TwitterConnection TwitterConnection;
#endif /* __cplusplus */

#endif 	/* __TwitterConnection_FWD_DEFINED__ */


#ifndef __PluginTable_FWD_DEFINED__
#define __PluginTable_FWD_DEFINED__

#ifdef __cplusplus
typedef class PluginTable PluginTable;
#else
typedef struct PluginTable PluginTable;
#endif /* __cplusplus */

#endif 	/* __PluginTable_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "twitconn_contract_i.h"
#include "shobjidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __twiconnLib_LIBRARY_DEFINED__
#define __twiconnLib_LIBRARY_DEFINED__

/* library twiconnLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_twiconnLib;

EXTERN_C const CLSID CLSID_TwitterConnection;

#ifdef __cplusplus

class DECLSPEC_UUID("20841FB0-9F07-4DA6-A878-41921C82C440")
TwitterConnection;
#endif

EXTERN_C const CLSID CLSID_PluginTable;

#ifdef __cplusplus

class DECLSPEC_UUID("6EA78630-DC6D-4D2B-91F0-686B260F29D6")
PluginTable;
#endif
#endif /* __twiconnLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


