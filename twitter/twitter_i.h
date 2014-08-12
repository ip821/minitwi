

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Tue Aug 12 14:45:08 2014
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

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __twitter_i_h__
#define __twitter_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IFormsService_FWD_DEFINED__
#define __IFormsService_FWD_DEFINED__
typedef interface IFormsService IFormsService;

#endif 	/* __IFormsService_FWD_DEFINED__ */


#ifndef __ITimelineService_FWD_DEFINED__
#define __ITimelineService_FWD_DEFINED__
typedef interface ITimelineService ITimelineService;

#endif 	/* __ITimelineService_FWD_DEFINED__ */


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


#ifndef __FormsService_FWD_DEFINED__
#define __FormsService_FWD_DEFINED__

#ifdef __cplusplus
typedef class FormsService FormsService;
#else
typedef struct FormsService FormsService;
#endif /* __cplusplus */

#endif 	/* __FormsService_FWD_DEFINED__ */


#ifndef __SettingsControl_FWD_DEFINED__
#define __SettingsControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class SettingsControl SettingsControl;
#else
typedef struct SettingsControl SettingsControl;
#endif /* __cplusplus */

#endif 	/* __SettingsControl_FWD_DEFINED__ */


#ifndef __TimelineService_FWD_DEFINED__
#define __TimelineService_FWD_DEFINED__

#ifdef __cplusplus
typedef class TimelineService TimelineService;
#else
typedef struct TimelineService TimelineService;
#endif /* __cplusplus */

#endif 	/* __TimelineService_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "objmdl_contract_i.h"
#include "viewmdl_contract_i.h"
#include "shobjidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IFormsService_INTERFACE_DEFINED__
#define __IFormsService_INTERFACE_DEFINED__

/* interface IFormsService */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IFormsService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DC001682-90CC-497F-BCE6-D00D712FD241")
    IFormsService : public IUnknown
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IFormsServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFormsService * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFormsService * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFormsService * This);
        
        END_INTERFACE
    } IFormsServiceVtbl;

    interface IFormsService
    {
        CONST_VTBL struct IFormsServiceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFormsService_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFormsService_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFormsService_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFormsService_INTERFACE_DEFINED__ */


#ifndef __ITimelineService_INTERFACE_DEFINED__
#define __ITimelineService_INTERFACE_DEFINED__

/* interface ITimelineService */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ITimelineService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("03F5DD74-E610-409A-991F-20BDDBA9A625")
    ITimelineService : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Start( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ITimelineServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITimelineService * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITimelineService * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITimelineService * This);
        
        HRESULT ( STDMETHODCALLTYPE *Start )( 
            ITimelineService * This);
        
        HRESULT ( STDMETHODCALLTYPE *Stop )( 
            ITimelineService * This);
        
        END_INTERFACE
    } ITimelineServiceVtbl;

    interface ITimelineService
    {
        CONST_VTBL struct ITimelineServiceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITimelineService_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITimelineService_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITimelineService_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITimelineService_Start(This)	\
    ( (This)->lpVtbl -> Start(This) ) 

#define ITimelineService_Stop(This)	\
    ( (This)->lpVtbl -> Stop(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITimelineService_INTERFACE_DEFINED__ */



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

EXTERN_C const CLSID CLSID_FormsService;

#ifdef __cplusplus

class DECLSPEC_UUID("5BEE1835-5D53-48C0-AB58-05EC1513010F")
FormsService;
#endif

EXTERN_C const CLSID CLSID_SettingsControl;

#ifdef __cplusplus

class DECLSPEC_UUID("6AFA6FA2-12D6-450E-AEFF-60B66F8B7574")
SettingsControl;
#endif

EXTERN_C const CLSID CLSID_TimelineService;

#ifdef __cplusplus

class DECLSPEC_UUID("FB68EE85-7555-407C-BECA-64406C74F1BC")
TimelineService;
#endif
#endif /* __twitterLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


