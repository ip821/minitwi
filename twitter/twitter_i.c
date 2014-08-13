

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Tue Aug 12 19:31:02 2014
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


MIDL_DEFINE_GUID(IID, IID_ITimelineControl,0x304C4130,0x701C,0x4951,0xAF,0x30,0x2A,0x38,0xA0,0xA5,0x1D,0x31);


MIDL_DEFINE_GUID(IID, LIBID_twitterLib,0x12BF3C97,0x9ACD,0x43C3,0x9E,0x73,0x69,0x8A,0xF2,0xDA,0xE6,0x45);


MIDL_DEFINE_GUID(CLSID, CLSID_PluginTable,0x6EA78630,0xDC6D,0x4D2B,0x91,0xF0,0x68,0x6B,0x26,0x0F,0x29,0xD6);


MIDL_DEFINE_GUID(CLSID, CLSID_TimelineControl,0x75E49C09,0x05E4,0x4643,0x98,0x3F,0x62,0x4D,0xC0,0x73,0x53,0x5A);


MIDL_DEFINE_GUID(CLSID, CLSID_FormsService,0x5BEE1835,0x5D53,0x48C0,0xAB,0x58,0x05,0xEC,0x15,0x13,0x01,0x0F);


MIDL_DEFINE_GUID(CLSID, CLSID_SettingsControl,0x6AFA6FA2,0x12D6,0x450E,0xAE,0xFF,0x60,0xB6,0x6F,0x8B,0x75,0x74);


MIDL_DEFINE_GUID(CLSID, CLSID_TimelineService,0xFB68EE85,0x7555,0x407C,0xBE,0xCA,0x64,0x40,0x6C,0x74,0xF1,0xBC);


MIDL_DEFINE_GUID(CLSID, CLSID_ViewControllerService,0xA31AF0A5,0xBE10,0x46FE,0x97,0x4F,0x55,0xE0,0xAA,0x68,0x07,0x0D);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



