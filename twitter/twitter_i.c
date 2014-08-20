

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Wed Aug 20 23:26:06 2014
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


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IFormsService,0xDC001682,0x90CC,0x497F,0xBC,0xE6,0xD0,0x0D,0x71,0x2F,0xD2,0x41);


MIDL_DEFINE_GUID(IID, IID_ITimelineService,0x03F5DD74,0xE610,0x409A,0x99,0x1F,0x20,0xBD,0xDB,0xA9,0xA6,0x25);


MIDL_DEFINE_GUID(IID, IID_IViewControllerService,0x27812A7C,0xA85F,0x4184,0xB1,0xDA,0x7E,0x9E,0xA5,0x5B,0x58,0x16);


MIDL_DEFINE_GUID(IID, IID_IThemeColorMap,0x627BE6CD,0x5CB1,0x4364,0x81,0x7A,0x1E,0xAB,0xA0,0x4B,0x52,0xC9);


MIDL_DEFINE_GUID(IID, IID_IThemeFontMap,0xDD372332,0x232D,0x4CA9,0x98,0x07,0xB2,0xF2,0x1D,0x3C,0x68,0x9A);


MIDL_DEFINE_GUID(IID, IID_IColumnRects,0x2EC751CC,0xD4F3,0x4B3B,0xB0,0xC7,0x86,0x29,0x16,0xA0,0x85,0xFC);


MIDL_DEFINE_GUID(IID, IID_ISkinTimeline,0xCAE8EE20,0xA39A,0x4677,0xA9,0xC0,0xFD,0xD8,0xD3,0xD7,0x01,0x5E);


MIDL_DEFINE_GUID(IID, IID_ITimelineControl,0x304C4130,0x701C,0x4951,0xAF,0x30,0x2A,0x38,0xA0,0xA5,0x1D,0x31);


MIDL_DEFINE_GUID(IID, IID_ITheme,0x825B6027,0xF439,0x4586,0x8D,0xAE,0xDF,0x9A,0xE8,0x52,0x7A,0xBD);


MIDL_DEFINE_GUID(IID, IID_IThemeService,0x3A2E1784,0x55A2,0x4075,0x86,0x17,0x22,0x53,0xA7,0x07,0x62,0xFF);


MIDL_DEFINE_GUID(IID, IID_IOpenUrlService,0x924BA62A,0x74BF,0x463A,0x80,0x08,0x49,0x7B,0x24,0x1F,0xC4,0x48);


MIDL_DEFINE_GUID(IID, LIBID_twitterLib,0x12BF3C97,0x9ACD,0x43C3,0x9E,0x73,0x69,0x8A,0xF2,0xDA,0xE6,0x45);


MIDL_DEFINE_GUID(CLSID, CLSID_PluginTable,0x6EA78630,0xDC6D,0x4D2B,0x91,0xF0,0x68,0x6B,0x26,0x0F,0x29,0xD6);


MIDL_DEFINE_GUID(CLSID, CLSID_TimelineControl,0x75E49C09,0x05E4,0x4643,0x98,0x3F,0x62,0x4D,0xC0,0x73,0x53,0x5A);


MIDL_DEFINE_GUID(CLSID, CLSID_FormsService,0x5BEE1835,0x5D53,0x48C0,0xAB,0x58,0x05,0xEC,0x15,0x13,0x01,0x0F);


MIDL_DEFINE_GUID(CLSID, CLSID_SettingsControl,0x6AFA6FA2,0x12D6,0x450E,0xAE,0xFF,0x60,0xB6,0x6F,0x8B,0x75,0x74);


MIDL_DEFINE_GUID(CLSID, CLSID_TimelineService,0xFB68EE85,0x7555,0x407C,0xBE,0xCA,0x64,0x40,0x6C,0x74,0xF1,0xBC);


MIDL_DEFINE_GUID(CLSID, CLSID_ViewControllerService,0xA31AF0A5,0xBE10,0x46FE,0x97,0x4F,0x55,0xE0,0xAA,0x68,0x07,0x0D);


MIDL_DEFINE_GUID(CLSID, CLSID_ThemeService,0xC4587505,0xCBA9,0x4583,0xA0,0xF0,0xCA,0x7B,0xEF,0x45,0x7F,0xA0);


MIDL_DEFINE_GUID(CLSID, CLSID_ThemeDefault,0xA7C215DD,0xBB09,0x4045,0x90,0x29,0x9E,0xE9,0xD1,0xD0,0xA3,0x89);


MIDL_DEFINE_GUID(CLSID, CLSID_SkinTimeline,0xB682B92C,0xACB3,0x446D,0xA3,0x7D,0xE1,0x1C,0x90,0x13,0x85,0xCA);


MIDL_DEFINE_GUID(CLSID, CLSID_ThemeColorMap,0x41F33BEE,0x3D82,0x4173,0x9F,0x5E,0x79,0xC9,0x15,0x23,0x84,0x3B);


MIDL_DEFINE_GUID(CLSID, CLSID_ThemeFontMap,0x986AF532,0xE457,0x4A2D,0x89,0x4D,0x08,0x05,0xAB,0x6B,0xD0,0xB1);


MIDL_DEFINE_GUID(CLSID, CLSID_ColumnRects,0x89CA168E,0x8568,0x4653,0x9D,0xCF,0x8D,0xEB,0x3C,0xA2,0x1A,0x37);


MIDL_DEFINE_GUID(CLSID, CLSID_OpenUrlService,0x07E747E2,0x451F,0x498F,0xB4,0x60,0xE4,0x6A,0x44,0xC5,0x12,0x6C);


MIDL_DEFINE_GUID(CLSID, CLSID_TimelineControlCopyCommand,0xF09290A9,0xAF90,0x416D,0x93,0x09,0xC6,0x49,0xEC,0xC9,0x79,0x35);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



