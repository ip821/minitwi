

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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

MIDL_DEFINE_GUID(IID, LIBID_twthemeLib,0xF7BFC6EF,0xD8BE,0x4C1A,0xA9,0xD5,0x9F,0xC5,0x7D,0x58,0xBF,0x5C);


MIDL_DEFINE_GUID(CLSID, CLSID_PluginTable,0x6EA78630,0xDC6D,0x4D2B,0x91,0xF0,0x68,0x6B,0x26,0x0F,0x29,0xD6);


MIDL_DEFINE_GUID(CLSID, CLSID_SkinTimeline,0xB682B92C,0xACB3,0x446D,0xA3,0x7D,0xE1,0x1C,0x90,0x13,0x85,0xCA);


MIDL_DEFINE_GUID(CLSID, CLSID_ThemeColorMap,0x41F33BEE,0x3D82,0x4173,0x9F,0x5E,0x79,0xC9,0x15,0x23,0x84,0x3B);


MIDL_DEFINE_GUID(CLSID, CLSID_ThemeFontMap,0x986AF532,0xE457,0x4A2D,0x89,0x4D,0x08,0x05,0xAB,0x6B,0xD0,0xB1);


MIDL_DEFINE_GUID(CLSID, CLSID_SkinTabControl,0xA4B1D02D,0x648F,0x4D71,0x99,0x07,0x7B,0x84,0xB9,0xDF,0x24,0x22);


MIDL_DEFINE_GUID(CLSID, CLSID_SkinCommonControl,0xD9BF55FB,0x091B,0x4858,0x82,0xEC,0xEB,0xD0,0x13,0x7D,0x04,0xFB);


MIDL_DEFINE_GUID(CLSID, CLSID_SkinUserAccountControl,0x0B2EE239,0xBF11,0x41FF,0x8D,0xF0,0x52,0x52,0x32,0x08,0x43,0x78);


MIDL_DEFINE_GUID(CLSID, CLSID_ThemeDefault,0xA7C215DD,0xBB09,0x4045,0x90,0x29,0x9E,0xE9,0xD1,0xD0,0xA3,0x89);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



