

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Tue Aug 26 11:55:31 2014
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


#ifndef __IImageManagerService_FWD_DEFINED__
#define __IImageManagerService_FWD_DEFINED__
typedef interface IImageManagerService IImageManagerService;

#endif 	/* __IImageManagerService_FWD_DEFINED__ */


#ifndef __ITimelineService_FWD_DEFINED__
#define __ITimelineService_FWD_DEFINED__
typedef interface ITimelineService ITimelineService;

#endif 	/* __ITimelineService_FWD_DEFINED__ */


#ifndef __IViewControllerService_FWD_DEFINED__
#define __IViewControllerService_FWD_DEFINED__
typedef interface IViewControllerService IViewControllerService;

#endif 	/* __IViewControllerService_FWD_DEFINED__ */


#ifndef __IThemeColorMap_FWD_DEFINED__
#define __IThemeColorMap_FWD_DEFINED__
typedef interface IThemeColorMap IThemeColorMap;

#endif 	/* __IThemeColorMap_FWD_DEFINED__ */


#ifndef __IThemeFontMap_FWD_DEFINED__
#define __IThemeFontMap_FWD_DEFINED__
typedef interface IThemeFontMap IThemeFontMap;

#endif 	/* __IThemeFontMap_FWD_DEFINED__ */


#ifndef __IColumnRects_FWD_DEFINED__
#define __IColumnRects_FWD_DEFINED__
typedef interface IColumnRects IColumnRects;

#endif 	/* __IColumnRects_FWD_DEFINED__ */


#ifndef __ISkinTimeline_FWD_DEFINED__
#define __ISkinTimeline_FWD_DEFINED__
typedef interface ISkinTimeline ISkinTimeline;

#endif 	/* __ISkinTimeline_FWD_DEFINED__ */


#ifndef __ITimelineControlEventSink_FWD_DEFINED__
#define __ITimelineControlEventSink_FWD_DEFINED__
typedef interface ITimelineControlEventSink ITimelineControlEventSink;

#endif 	/* __ITimelineControlEventSink_FWD_DEFINED__ */


#ifndef __ITimelineControl_FWD_DEFINED__
#define __ITimelineControl_FWD_DEFINED__
typedef interface ITimelineControl ITimelineControl;

#endif 	/* __ITimelineControl_FWD_DEFINED__ */


#ifndef __ITheme_FWD_DEFINED__
#define __ITheme_FWD_DEFINED__
typedef interface ITheme ITheme;

#endif 	/* __ITheme_FWD_DEFINED__ */


#ifndef __IThemeService_FWD_DEFINED__
#define __IThemeService_FWD_DEFINED__
typedef interface IThemeService IThemeService;

#endif 	/* __IThemeService_FWD_DEFINED__ */


#ifndef __IOpenUrlService_FWD_DEFINED__
#define __IOpenUrlService_FWD_DEFINED__
typedef interface IOpenUrlService IOpenUrlService;

#endif 	/* __IOpenUrlService_FWD_DEFINED__ */


#ifndef __IDownloadServiceEventSink_FWD_DEFINED__
#define __IDownloadServiceEventSink_FWD_DEFINED__
typedef interface IDownloadServiceEventSink IDownloadServiceEventSink;

#endif 	/* __IDownloadServiceEventSink_FWD_DEFINED__ */


#ifndef __IDownloadService_FWD_DEFINED__
#define __IDownloadService_FWD_DEFINED__
typedef interface IDownloadService IDownloadService;

#endif 	/* __IDownloadService_FWD_DEFINED__ */


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


#ifndef __ViewControllerService_FWD_DEFINED__
#define __ViewControllerService_FWD_DEFINED__

#ifdef __cplusplus
typedef class ViewControllerService ViewControllerService;
#else
typedef struct ViewControllerService ViewControllerService;
#endif /* __cplusplus */

#endif 	/* __ViewControllerService_FWD_DEFINED__ */


#ifndef __ThemeService_FWD_DEFINED__
#define __ThemeService_FWD_DEFINED__

#ifdef __cplusplus
typedef class ThemeService ThemeService;
#else
typedef struct ThemeService ThemeService;
#endif /* __cplusplus */

#endif 	/* __ThemeService_FWD_DEFINED__ */


#ifndef __ThemeDefault_FWD_DEFINED__
#define __ThemeDefault_FWD_DEFINED__

#ifdef __cplusplus
typedef class ThemeDefault ThemeDefault;
#else
typedef struct ThemeDefault ThemeDefault;
#endif /* __cplusplus */

#endif 	/* __ThemeDefault_FWD_DEFINED__ */


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


#ifndef __ColumnRects_FWD_DEFINED__
#define __ColumnRects_FWD_DEFINED__

#ifdef __cplusplus
typedef class ColumnRects ColumnRects;
#else
typedef struct ColumnRects ColumnRects;
#endif /* __cplusplus */

#endif 	/* __ColumnRects_FWD_DEFINED__ */


#ifndef __OpenUrlService_FWD_DEFINED__
#define __OpenUrlService_FWD_DEFINED__

#ifdef __cplusplus
typedef class OpenUrlService OpenUrlService;
#else
typedef struct OpenUrlService OpenUrlService;
#endif /* __cplusplus */

#endif 	/* __OpenUrlService_FWD_DEFINED__ */


#ifndef __TimelineControlCopyCommand_FWD_DEFINED__
#define __TimelineControlCopyCommand_FWD_DEFINED__

#ifdef __cplusplus
typedef class TimelineControlCopyCommand TimelineControlCopyCommand;
#else
typedef struct TimelineControlCopyCommand TimelineControlCopyCommand;
#endif /* __cplusplus */

#endif 	/* __TimelineControlCopyCommand_FWD_DEFINED__ */


#ifndef __ImageManagerService_FWD_DEFINED__
#define __ImageManagerService_FWD_DEFINED__

#ifdef __cplusplus
typedef class ImageManagerService ImageManagerService;
#else
typedef struct ImageManagerService ImageManagerService;
#endif /* __cplusplus */

#endif 	/* __ImageManagerService_FWD_DEFINED__ */


#ifndef __DownloadService_FWD_DEFINED__
#define __DownloadService_FWD_DEFINED__

#ifdef __cplusplus
typedef class DownloadService DownloadService;
#else
typedef struct DownloadService DownloadService;
#endif /* __cplusplus */

#endif 	/* __DownloadService_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "objmdl_contract_i.h"
#include "viewmdl_contract_i.h"
#include "shobjidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_twitter_0000_0000 */
/* [local] */ 

typedef struct TtagDRAWITEMSTRUCT
    {
    UINT CtlType;
    UINT CtlID;
    UINT itemID;
    UINT itemAction;
    UINT itemState;
    HWND hwndItem;
    HDC hDC;
    RECT rcItem;
    ULONG_PTR itemData;
    } 	TDRAWITEMSTRUCT;

typedef struct TtagMEASUREITEMSTRUCT
    {
    UINT CtlType;
    UINT CtlID;
    UINT itemID;
    UINT itemWidth;
    UINT itemHeight;
    ULONG_PTR itemData;
    } 	TMEASUREITEMSTRUCT;

typedef struct TtagBITMAP
    {
    UINT Width;
    UINT Height;
    } 	TBITMAP;



extern RPC_IF_HANDLE __MIDL_itf_twitter_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_twitter_0000_0000_v0_0_s_ifspec;

#ifndef __IFormsService_INTERFACE_DEFINED__
#define __IFormsService_INTERFACE_DEFINED__

/* interface IFormsService */
/* [unique][uuid][object] */ 


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


#ifndef __IImageManagerService_INTERFACE_DEFINED__
#define __IImageManagerService_INTERFACE_DEFINED__

/* interface IImageManagerService */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IImageManagerService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("828C2337-535F-491B-943D-EDF93F760CB7")
    IImageManagerService : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetImageInfo( 
            BSTR bstrKey,
            TBITMAP *ptBitmap) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateImageBitmap( 
            BSTR bstrKey,
            HBITMAP *phBitmap) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetImage( 
            BSTR bstrKey,
            BSTR bstrFileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ContainsImageKey( 
            BSTR bstrKey,
            BOOL *pbContains) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveImage( 
            BSTR bstrKey) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IImageManagerServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IImageManagerService * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IImageManagerService * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IImageManagerService * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetImageInfo )( 
            IImageManagerService * This,
            BSTR bstrKey,
            TBITMAP *ptBitmap);
        
        HRESULT ( STDMETHODCALLTYPE *CreateImageBitmap )( 
            IImageManagerService * This,
            BSTR bstrKey,
            HBITMAP *phBitmap);
        
        HRESULT ( STDMETHODCALLTYPE *SetImage )( 
            IImageManagerService * This,
            BSTR bstrKey,
            BSTR bstrFileName);
        
        HRESULT ( STDMETHODCALLTYPE *ContainsImageKey )( 
            IImageManagerService * This,
            BSTR bstrKey,
            BOOL *pbContains);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveImage )( 
            IImageManagerService * This,
            BSTR bstrKey);
        
        END_INTERFACE
    } IImageManagerServiceVtbl;

    interface IImageManagerService
    {
        CONST_VTBL struct IImageManagerServiceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IImageManagerService_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IImageManagerService_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IImageManagerService_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IImageManagerService_GetImageInfo(This,bstrKey,ptBitmap)	\
    ( (This)->lpVtbl -> GetImageInfo(This,bstrKey,ptBitmap) ) 

#define IImageManagerService_CreateImageBitmap(This,bstrKey,phBitmap)	\
    ( (This)->lpVtbl -> CreateImageBitmap(This,bstrKey,phBitmap) ) 

#define IImageManagerService_SetImage(This,bstrKey,bstrFileName)	\
    ( (This)->lpVtbl -> SetImage(This,bstrKey,bstrFileName) ) 

#define IImageManagerService_ContainsImageKey(This,bstrKey,pbContains)	\
    ( (This)->lpVtbl -> ContainsImageKey(This,bstrKey,pbContains) ) 

#define IImageManagerService_RemoveImage(This,bstrKey)	\
    ( (This)->lpVtbl -> RemoveImage(This,bstrKey) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IImageManagerService_INTERFACE_DEFINED__ */


#ifndef __ITimelineService_INTERFACE_DEFINED__
#define __ITimelineService_INTERFACE_DEFINED__

/* interface ITimelineService */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITimelineService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("03F5DD74-E610-409A-991F-20BDDBA9A625")
    ITimelineService : public IUnknown
    {
    public:
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


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITimelineService_INTERFACE_DEFINED__ */


#ifndef __IViewControllerService_INTERFACE_DEFINED__
#define __IViewControllerService_INTERFACE_DEFINED__

/* interface IViewControllerService */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IViewControllerService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("27812A7C-A85F-4184-B1DA-7E9EA55B5816")
    IViewControllerService : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE StartTimers( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StopTimers( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IViewControllerServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IViewControllerService * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IViewControllerService * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IViewControllerService * This);
        
        HRESULT ( STDMETHODCALLTYPE *StartTimers )( 
            IViewControllerService * This);
        
        HRESULT ( STDMETHODCALLTYPE *StopTimers )( 
            IViewControllerService * This);
        
        END_INTERFACE
    } IViewControllerServiceVtbl;

    interface IViewControllerService
    {
        CONST_VTBL struct IViewControllerServiceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IViewControllerService_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IViewControllerService_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IViewControllerService_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IViewControllerService_StartTimers(This)	\
    ( (This)->lpVtbl -> StartTimers(This) ) 

#define IViewControllerService_StopTimers(This)	\
    ( (This)->lpVtbl -> StopTimers(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IViewControllerService_INTERFACE_DEFINED__ */


#ifndef __IThemeColorMap_INTERFACE_DEFINED__
#define __IThemeColorMap_INTERFACE_DEFINED__

/* interface IThemeColorMap */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IThemeColorMap;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("627BE6CD-5CB1-4364-817A-1EABA04B52C9")
    IThemeColorMap : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetColor( 
            BSTR bstrColorName,
            DWORD *dwColor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetColor( 
            BSTR bstrColorName,
            DWORD dwColor) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IThemeColorMapVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IThemeColorMap * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IThemeColorMap * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IThemeColorMap * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetColor )( 
            IThemeColorMap * This,
            BSTR bstrColorName,
            DWORD *dwColor);
        
        HRESULT ( STDMETHODCALLTYPE *SetColor )( 
            IThemeColorMap * This,
            BSTR bstrColorName,
            DWORD dwColor);
        
        END_INTERFACE
    } IThemeColorMapVtbl;

    interface IThemeColorMap
    {
        CONST_VTBL struct IThemeColorMapVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IThemeColorMap_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IThemeColorMap_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IThemeColorMap_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IThemeColorMap_GetColor(This,bstrColorName,dwColor)	\
    ( (This)->lpVtbl -> GetColor(This,bstrColorName,dwColor) ) 

#define IThemeColorMap_SetColor(This,bstrColorName,dwColor)	\
    ( (This)->lpVtbl -> SetColor(This,bstrColorName,dwColor) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IThemeColorMap_INTERFACE_DEFINED__ */


#ifndef __IThemeFontMap_INTERFACE_DEFINED__
#define __IThemeFontMap_INTERFACE_DEFINED__

/* interface IThemeFontMap */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IThemeFontMap;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DD372332-232D-4CA9-9807-B2F21D3C689A")
    IThemeFontMap : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetFont( 
            BSTR bstrFontName,
            HFONT *phFont) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFont( 
            BSTR bstrFontName,
            BSTR bstrFontFamily,
            DWORD dwSize,
            BOOL bBold,
            BOOL bUnderline) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IThemeFontMapVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IThemeFontMap * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IThemeFontMap * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IThemeFontMap * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetFont )( 
            IThemeFontMap * This,
            BSTR bstrFontName,
            HFONT *phFont);
        
        HRESULT ( STDMETHODCALLTYPE *SetFont )( 
            IThemeFontMap * This,
            BSTR bstrFontName,
            BSTR bstrFontFamily,
            DWORD dwSize,
            BOOL bBold,
            BOOL bUnderline);
        
        END_INTERFACE
    } IThemeFontMapVtbl;

    interface IThemeFontMap
    {
        CONST_VTBL struct IThemeFontMapVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IThemeFontMap_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IThemeFontMap_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IThemeFontMap_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IThemeFontMap_GetFont(This,bstrFontName,phFont)	\
    ( (This)->lpVtbl -> GetFont(This,bstrFontName,phFont) ) 

#define IThemeFontMap_SetFont(This,bstrFontName,bstrFontFamily,dwSize,bBold,bUnderline)	\
    ( (This)->lpVtbl -> SetFont(This,bstrFontName,bstrFontFamily,dwSize,bBold,bUnderline) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IThemeFontMap_INTERFACE_DEFINED__ */


#ifndef __IColumnRects_INTERFACE_DEFINED__
#define __IColumnRects_INTERFACE_DEFINED__

/* interface IColumnRects */
/* [local][unique][uuid][object] */ 


EXTERN_C const IID IID_IColumnRects;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2EC751CC-D4F3-4B3B-B0C7-862916A085FC")
    IColumnRects : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddRect( 
            RECT rect,
            UINT *puiIndex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRect( 
            UINT uiIndex,
            RECT *rect) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            UINT *puiCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetRectProp( 
            UINT uiIndex,
            BSTR bstrKey,
            BSTR bstrValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRectProp( 
            UINT uiIndex,
            BSTR bstrKey,
            BSTR *bstrValue) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IColumnRectsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IColumnRects * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IColumnRects * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IColumnRects * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddRect )( 
            IColumnRects * This,
            RECT rect,
            UINT *puiIndex);
        
        HRESULT ( STDMETHODCALLTYPE *GetRect )( 
            IColumnRects * This,
            UINT uiIndex,
            RECT *rect);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IColumnRects * This,
            UINT *puiCount);
        
        HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IColumnRects * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetRectProp )( 
            IColumnRects * This,
            UINT uiIndex,
            BSTR bstrKey,
            BSTR bstrValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetRectProp )( 
            IColumnRects * This,
            UINT uiIndex,
            BSTR bstrKey,
            BSTR *bstrValue);
        
        END_INTERFACE
    } IColumnRectsVtbl;

    interface IColumnRects
    {
        CONST_VTBL struct IColumnRectsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IColumnRects_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IColumnRects_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IColumnRects_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IColumnRects_AddRect(This,rect,puiIndex)	\
    ( (This)->lpVtbl -> AddRect(This,rect,puiIndex) ) 

#define IColumnRects_GetRect(This,uiIndex,rect)	\
    ( (This)->lpVtbl -> GetRect(This,uiIndex,rect) ) 

#define IColumnRects_GetCount(This,puiCount)	\
    ( (This)->lpVtbl -> GetCount(This,puiCount) ) 

#define IColumnRects_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IColumnRects_SetRectProp(This,uiIndex,bstrKey,bstrValue)	\
    ( (This)->lpVtbl -> SetRectProp(This,uiIndex,bstrKey,bstrValue) ) 

#define IColumnRects_GetRectProp(This,uiIndex,bstrKey,bstrValue)	\
    ( (This)->lpVtbl -> GetRectProp(This,uiIndex,bstrKey,bstrValue) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IColumnRects_INTERFACE_DEFINED__ */


#ifndef __ISkinTimeline_INTERFACE_DEFINED__
#define __ISkinTimeline_INTERFACE_DEFINED__

/* interface ISkinTimeline */
/* [local][unique][uuid][object] */ 


EXTERN_C const IID IID_ISkinTimeline;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CAE8EE20-A39A-4677-A9C0-FDD8D3D7015E")
    ISkinTimeline : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetColorMap( 
            IThemeColorMap *pThemeColorMap) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFontMap( 
            IThemeFontMap *pThemeFontMap) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetImageManagerService( 
            IImageManagerService *pImageManagerService) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DrawItem( 
            HWND hwndControl,
            IColumnRects *pColumnRects,
            TDRAWITEMSTRUCT *lpdi,
            int iHoveredItem,
            int iHoveredColumn) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MeasureItem( 
            HWND hwndControl,
            IVariantObject *pItemObject,
            TMEASUREITEMSTRUCT *lpMeasureItemStruct,
            IColumnRects *pColumnRects) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ISkinTimelineVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISkinTimeline * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISkinTimeline * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISkinTimeline * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetColorMap )( 
            ISkinTimeline * This,
            IThemeColorMap *pThemeColorMap);
        
        HRESULT ( STDMETHODCALLTYPE *SetFontMap )( 
            ISkinTimeline * This,
            IThemeFontMap *pThemeFontMap);
        
        HRESULT ( STDMETHODCALLTYPE *SetImageManagerService )( 
            ISkinTimeline * This,
            IImageManagerService *pImageManagerService);
        
        HRESULT ( STDMETHODCALLTYPE *DrawItem )( 
            ISkinTimeline * This,
            HWND hwndControl,
            IColumnRects *pColumnRects,
            TDRAWITEMSTRUCT *lpdi,
            int iHoveredItem,
            int iHoveredColumn);
        
        HRESULT ( STDMETHODCALLTYPE *MeasureItem )( 
            ISkinTimeline * This,
            HWND hwndControl,
            IVariantObject *pItemObject,
            TMEASUREITEMSTRUCT *lpMeasureItemStruct,
            IColumnRects *pColumnRects);
        
        END_INTERFACE
    } ISkinTimelineVtbl;

    interface ISkinTimeline
    {
        CONST_VTBL struct ISkinTimelineVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISkinTimeline_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISkinTimeline_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISkinTimeline_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISkinTimeline_SetColorMap(This,pThemeColorMap)	\
    ( (This)->lpVtbl -> SetColorMap(This,pThemeColorMap) ) 

#define ISkinTimeline_SetFontMap(This,pThemeFontMap)	\
    ( (This)->lpVtbl -> SetFontMap(This,pThemeFontMap) ) 

#define ISkinTimeline_SetImageManagerService(This,pImageManagerService)	\
    ( (This)->lpVtbl -> SetImageManagerService(This,pImageManagerService) ) 

#define ISkinTimeline_DrawItem(This,hwndControl,pColumnRects,lpdi,iHoveredItem,iHoveredColumn)	\
    ( (This)->lpVtbl -> DrawItem(This,hwndControl,pColumnRects,lpdi,iHoveredItem,iHoveredColumn) ) 

#define ISkinTimeline_MeasureItem(This,hwndControl,pItemObject,lpMeasureItemStruct,pColumnRects)	\
    ( (This)->lpVtbl -> MeasureItem(This,hwndControl,pItemObject,lpMeasureItemStruct,pColumnRects) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISkinTimeline_INTERFACE_DEFINED__ */


#ifndef __ITimelineControlEventSink_INTERFACE_DEFINED__
#define __ITimelineControlEventSink_INTERFACE_DEFINED__

/* interface ITimelineControlEventSink */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITimelineControlEventSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("53ADEAC2-0827-4AAF-9B82-E695ACC14918")
    ITimelineControlEventSink : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnItemRemoved( 
            IVariantObject *pItemObject) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ITimelineControlEventSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITimelineControlEventSink * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITimelineControlEventSink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITimelineControlEventSink * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnItemRemoved )( 
            ITimelineControlEventSink * This,
            IVariantObject *pItemObject);
        
        END_INTERFACE
    } ITimelineControlEventSinkVtbl;

    interface ITimelineControlEventSink
    {
        CONST_VTBL struct ITimelineControlEventSinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITimelineControlEventSink_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITimelineControlEventSink_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITimelineControlEventSink_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITimelineControlEventSink_OnItemRemoved(This,pItemObject)	\
    ( (This)->lpVtbl -> OnItemRemoved(This,pItemObject) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITimelineControlEventSink_INTERFACE_DEFINED__ */


#ifndef __ITimelineControl_INTERFACE_DEFINED__
#define __ITimelineControl_INTERFACE_DEFINED__

/* interface ITimelineControl */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITimelineControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("304C4130-701C-4951-AF30-2A38A0A51D31")
    ITimelineControl : public IControl2
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetItems( 
            IObjArray **ppObjectArray) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetItems( 
            IObjArray *pObjectArray) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Invalidate( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSkinTimeline( 
            ISkinTimeline *pSkinTimeline) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BeginUpdate( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EndUpdate( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnItemsUpdated( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ITimelineControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITimelineControl * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITimelineControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITimelineControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetHWND )( 
            ITimelineControl * This,
            HWND *hWnd);
        
        HRESULT ( STDMETHODCALLTYPE *CreateEx )( 
            ITimelineControl * This,
            HWND hWndParent,
            HWND *hWnd);
        
        HRESULT ( STDMETHODCALLTYPE *PreTranslateMessage )( 
            ITimelineControl * This,
            MSG *pMsg,
            BOOL *pbResult);
        
        HRESULT ( STDMETHODCALLTYPE *CreateEx2 )( 
            ITimelineControl * This,
            HWND hWndParent,
            RECT rect,
            HWND *hWnd);
        
        HRESULT ( STDMETHODCALLTYPE *GetText )( 
            ITimelineControl * This,
            BSTR *pbstr);
        
        HRESULT ( STDMETHODCALLTYPE *OnActivate )( 
            ITimelineControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnDeactivate )( 
            ITimelineControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnClose )( 
            ITimelineControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetItems )( 
            ITimelineControl * This,
            IObjArray **ppObjectArray);
        
        HRESULT ( STDMETHODCALLTYPE *SetItems )( 
            ITimelineControl * This,
            IObjArray *pObjectArray);
        
        HRESULT ( STDMETHODCALLTYPE *Invalidate )( 
            ITimelineControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clear )( 
            ITimelineControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetSkinTimeline )( 
            ITimelineControl * This,
            ISkinTimeline *pSkinTimeline);
        
        HRESULT ( STDMETHODCALLTYPE *BeginUpdate )( 
            ITimelineControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *EndUpdate )( 
            ITimelineControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnItemsUpdated )( 
            ITimelineControl * This);
        
        END_INTERFACE
    } ITimelineControlVtbl;

    interface ITimelineControl
    {
        CONST_VTBL struct ITimelineControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITimelineControl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITimelineControl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITimelineControl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITimelineControl_GetHWND(This,hWnd)	\
    ( (This)->lpVtbl -> GetHWND(This,hWnd) ) 

#define ITimelineControl_CreateEx(This,hWndParent,hWnd)	\
    ( (This)->lpVtbl -> CreateEx(This,hWndParent,hWnd) ) 

#define ITimelineControl_PreTranslateMessage(This,pMsg,pbResult)	\
    ( (This)->lpVtbl -> PreTranslateMessage(This,pMsg,pbResult) ) 


#define ITimelineControl_CreateEx2(This,hWndParent,rect,hWnd)	\
    ( (This)->lpVtbl -> CreateEx2(This,hWndParent,rect,hWnd) ) 

#define ITimelineControl_GetText(This,pbstr)	\
    ( (This)->lpVtbl -> GetText(This,pbstr) ) 

#define ITimelineControl_OnActivate(This)	\
    ( (This)->lpVtbl -> OnActivate(This) ) 

#define ITimelineControl_OnDeactivate(This)	\
    ( (This)->lpVtbl -> OnDeactivate(This) ) 

#define ITimelineControl_OnClose(This)	\
    ( (This)->lpVtbl -> OnClose(This) ) 


#define ITimelineControl_GetItems(This,ppObjectArray)	\
    ( (This)->lpVtbl -> GetItems(This,ppObjectArray) ) 

#define ITimelineControl_SetItems(This,pObjectArray)	\
    ( (This)->lpVtbl -> SetItems(This,pObjectArray) ) 

#define ITimelineControl_Invalidate(This)	\
    ( (This)->lpVtbl -> Invalidate(This) ) 

#define ITimelineControl_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define ITimelineControl_SetSkinTimeline(This,pSkinTimeline)	\
    ( (This)->lpVtbl -> SetSkinTimeline(This,pSkinTimeline) ) 

#define ITimelineControl_BeginUpdate(This)	\
    ( (This)->lpVtbl -> BeginUpdate(This) ) 

#define ITimelineControl_EndUpdate(This)	\
    ( (This)->lpVtbl -> EndUpdate(This) ) 

#define ITimelineControl_OnItemsUpdated(This)	\
    ( (This)->lpVtbl -> OnItemsUpdated(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITimelineControl_INTERFACE_DEFINED__ */


#ifndef __ITheme_INTERFACE_DEFINED__
#define __ITheme_INTERFACE_DEFINED__

/* interface ITheme */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITheme;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("825B6027-F439-4586-8DAE-DF9AE8527ABD")
    ITheme : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetColorMap( 
            IThemeColorMap *pThemeColorMap) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTimelineSkin( 
            ISkinTimeline **ppSkinTimeline) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetImageManagerService( 
            IImageManagerService *pImageManagerService) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IThemeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITheme * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITheme * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITheme * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetColorMap )( 
            ITheme * This,
            IThemeColorMap *pThemeColorMap);
        
        HRESULT ( STDMETHODCALLTYPE *GetTimelineSkin )( 
            ITheme * This,
            ISkinTimeline **ppSkinTimeline);
        
        HRESULT ( STDMETHODCALLTYPE *SetImageManagerService )( 
            ITheme * This,
            IImageManagerService *pImageManagerService);
        
        END_INTERFACE
    } IThemeVtbl;

    interface ITheme
    {
        CONST_VTBL struct IThemeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITheme_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITheme_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITheme_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITheme_SetColorMap(This,pThemeColorMap)	\
    ( (This)->lpVtbl -> SetColorMap(This,pThemeColorMap) ) 

#define ITheme_GetTimelineSkin(This,ppSkinTimeline)	\
    ( (This)->lpVtbl -> GetTimelineSkin(This,ppSkinTimeline) ) 

#define ITheme_SetImageManagerService(This,pImageManagerService)	\
    ( (This)->lpVtbl -> SetImageManagerService(This,pImageManagerService) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITheme_INTERFACE_DEFINED__ */


#ifndef __IThemeService_INTERFACE_DEFINED__
#define __IThemeService_INTERFACE_DEFINED__

/* interface IThemeService */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IThemeService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3A2E1784-55A2-4075-8617-2253A70762FF")
    IThemeService : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddColorMap( 
            GUID gThemeId,
            IThemeColorMap *pThemeColorMap) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveColorMap( 
            GUID gThemeId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddTheme( 
            GUID gThemeId,
            ITheme *pTheme) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveTheme( 
            GUID gThemeId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetThemes( 
            IObjArray **ppObjectArray) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ApplyTheme( 
            GUID gId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ApplyThemeFromSettings( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IThemeServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IThemeService * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IThemeService * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IThemeService * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddColorMap )( 
            IThemeService * This,
            GUID gThemeId,
            IThemeColorMap *pThemeColorMap);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveColorMap )( 
            IThemeService * This,
            GUID gThemeId);
        
        HRESULT ( STDMETHODCALLTYPE *AddTheme )( 
            IThemeService * This,
            GUID gThemeId,
            ITheme *pTheme);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveTheme )( 
            IThemeService * This,
            GUID gThemeId);
        
        HRESULT ( STDMETHODCALLTYPE *GetThemes )( 
            IThemeService * This,
            IObjArray **ppObjectArray);
        
        HRESULT ( STDMETHODCALLTYPE *ApplyTheme )( 
            IThemeService * This,
            GUID gId);
        
        HRESULT ( STDMETHODCALLTYPE *ApplyThemeFromSettings )( 
            IThemeService * This);
        
        END_INTERFACE
    } IThemeServiceVtbl;

    interface IThemeService
    {
        CONST_VTBL struct IThemeServiceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IThemeService_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IThemeService_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IThemeService_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IThemeService_AddColorMap(This,gThemeId,pThemeColorMap)	\
    ( (This)->lpVtbl -> AddColorMap(This,gThemeId,pThemeColorMap) ) 

#define IThemeService_RemoveColorMap(This,gThemeId)	\
    ( (This)->lpVtbl -> RemoveColorMap(This,gThemeId) ) 

#define IThemeService_AddTheme(This,gThemeId,pTheme)	\
    ( (This)->lpVtbl -> AddTheme(This,gThemeId,pTheme) ) 

#define IThemeService_RemoveTheme(This,gThemeId)	\
    ( (This)->lpVtbl -> RemoveTheme(This,gThemeId) ) 

#define IThemeService_GetThemes(This,ppObjectArray)	\
    ( (This)->lpVtbl -> GetThemes(This,ppObjectArray) ) 

#define IThemeService_ApplyTheme(This,gId)	\
    ( (This)->lpVtbl -> ApplyTheme(This,gId) ) 

#define IThemeService_ApplyThemeFromSettings(This)	\
    ( (This)->lpVtbl -> ApplyThemeFromSettings(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IThemeService_INTERFACE_DEFINED__ */


#ifndef __IOpenUrlService_INTERFACE_DEFINED__
#define __IOpenUrlService_INTERFACE_DEFINED__

/* interface IOpenUrlService */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IOpenUrlService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("924BA62A-74BF-463A-8008-497B241FC448")
    IOpenUrlService : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OpenColumnAsUrl( 
            BSTR bstrColumnName,
            DWORD dwColumnIndex,
            IColumnRects *pColumnRects,
            IVariantObject *pVariantObject) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IOpenUrlServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOpenUrlService * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOpenUrlService * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOpenUrlService * This);
        
        HRESULT ( STDMETHODCALLTYPE *OpenColumnAsUrl )( 
            IOpenUrlService * This,
            BSTR bstrColumnName,
            DWORD dwColumnIndex,
            IColumnRects *pColumnRects,
            IVariantObject *pVariantObject);
        
        END_INTERFACE
    } IOpenUrlServiceVtbl;

    interface IOpenUrlService
    {
        CONST_VTBL struct IOpenUrlServiceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOpenUrlService_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IOpenUrlService_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IOpenUrlService_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IOpenUrlService_OpenColumnAsUrl(This,bstrColumnName,dwColumnIndex,pColumnRects,pVariantObject)	\
    ( (This)->lpVtbl -> OpenColumnAsUrl(This,bstrColumnName,dwColumnIndex,pColumnRects,pVariantObject) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IOpenUrlService_INTERFACE_DEFINED__ */


#ifndef __IDownloadServiceEventSink_INTERFACE_DEFINED__
#define __IDownloadServiceEventSink_INTERFACE_DEFINED__

/* interface IDownloadServiceEventSink */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDownloadServiceEventSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5304DEAC-784A-457C-9037-1E878B9D2EAA")
    IDownloadServiceEventSink : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnDownloadComplete( 
            IVariantObject *pResult) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IDownloadServiceEventSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDownloadServiceEventSink * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDownloadServiceEventSink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDownloadServiceEventSink * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnDownloadComplete )( 
            IDownloadServiceEventSink * This,
            IVariantObject *pResult);
        
        END_INTERFACE
    } IDownloadServiceEventSinkVtbl;

    interface IDownloadServiceEventSink
    {
        CONST_VTBL struct IDownloadServiceEventSinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDownloadServiceEventSink_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDownloadServiceEventSink_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDownloadServiceEventSink_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDownloadServiceEventSink_OnDownloadComplete(This,pResult)	\
    ( (This)->lpVtbl -> OnDownloadComplete(This,pResult) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDownloadServiceEventSink_INTERFACE_DEFINED__ */


#ifndef __IDownloadService_INTERFACE_DEFINED__
#define __IDownloadService_INTERFACE_DEFINED__

/* interface IDownloadService */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDownloadService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("18A1A514-1F78-4811-B469-509CFE4AAF94")
    IDownloadService : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddDownload( 
            IVariantObject *pVariantObject) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IDownloadServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDownloadService * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDownloadService * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDownloadService * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddDownload )( 
            IDownloadService * This,
            IVariantObject *pVariantObject);
        
        END_INTERFACE
    } IDownloadServiceVtbl;

    interface IDownloadService
    {
        CONST_VTBL struct IDownloadServiceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDownloadService_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDownloadService_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDownloadService_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDownloadService_AddDownload(This,pVariantObject)	\
    ( (This)->lpVtbl -> AddDownload(This,pVariantObject) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDownloadService_INTERFACE_DEFINED__ */



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

EXTERN_C const CLSID CLSID_ViewControllerService;

#ifdef __cplusplus

class DECLSPEC_UUID("A31AF0A5-BE10-46FE-974F-55E0AA68070D")
ViewControllerService;
#endif

EXTERN_C const CLSID CLSID_ThemeService;

#ifdef __cplusplus

class DECLSPEC_UUID("C4587505-CBA9-4583-A0F0-CA7BEF457FA0")
ThemeService;
#endif

EXTERN_C const CLSID CLSID_ThemeDefault;

#ifdef __cplusplus

class DECLSPEC_UUID("A7C215DD-BB09-4045-9029-9EE9D1D0A389")
ThemeDefault;
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

EXTERN_C const CLSID CLSID_ColumnRects;

#ifdef __cplusplus

class DECLSPEC_UUID("89CA168E-8568-4653-9DCF-8DEB3CA21A37")
ColumnRects;
#endif

EXTERN_C const CLSID CLSID_OpenUrlService;

#ifdef __cplusplus

class DECLSPEC_UUID("07E747E2-451F-498F-B460-E46A44C5126C")
OpenUrlService;
#endif

EXTERN_C const CLSID CLSID_TimelineControlCopyCommand;

#ifdef __cplusplus

class DECLSPEC_UUID("F09290A9-AF90-416D-9309-C649ECC97935")
TimelineControlCopyCommand;
#endif

EXTERN_C const CLSID CLSID_ImageManagerService;

#ifdef __cplusplus

class DECLSPEC_UUID("204D2E4F-72BE-41EF-8B7D-992B89BE36DA")
ImageManagerService;
#endif

EXTERN_C const CLSID CLSID_DownloadService;

#ifdef __cplusplus

class DECLSPEC_UUID("8A7C5710-9FCB-4FD9-83A2-36EE81E10FB9")
DownloadService;
#endif
#endif /* __twitterLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  HBITMAP_UserSize(     unsigned long *, unsigned long            , HBITMAP * ); 
unsigned char * __RPC_USER  HBITMAP_UserMarshal(  unsigned long *, unsigned char *, HBITMAP * ); 
unsigned char * __RPC_USER  HBITMAP_UserUnmarshal(unsigned long *, unsigned char *, HBITMAP * ); 
void                      __RPC_USER  HBITMAP_UserFree(     unsigned long *, HBITMAP * ); 

unsigned long             __RPC_USER  HFONT_UserSize(     unsigned long *, unsigned long            , HFONT * ); 
unsigned char * __RPC_USER  HFONT_UserMarshal(  unsigned long *, unsigned char *, HFONT * ); 
unsigned char * __RPC_USER  HFONT_UserUnmarshal(unsigned long *, unsigned char *, HFONT * ); 
void                      __RPC_USER  HFONT_UserFree(     unsigned long *, HFONT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


