// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#ifdef __WINXP__
#define WINVER		0x0501
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200
#else
#define WINVER		0x0600
#define _WIN32_WINNT	0x0601
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW
#define _WTL_NO_CSTRING
#define __TWTHEME__

#define BOOST_LIB_DIAGNOSTIC 1
#define _CRT_SECURE_NO_WARNINGS


#include "resource.h"
#include <atlstr.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlcom.h>
#include <atlctl.h>

#include <atlhost.h>
#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlctrlx.h>
#include <atlmisc.h>
#include <comdef.h>

#include <gdiplus.h>
#include <map>
#include <unordered_set>

#include <boost/lexical_cast.hpp>

#include <initguid.h>
#include "..\ObjMdl\Macros.h"
#include "..\ObjMdl\ManualComObjectLoader.h"
#include "..\ObjMdl\Plugins.h"
#include "..\ObjMdl\Functions.h"
#include "..\ObjMdl\Metadata.h"
#include "..\ViewMdl\Plugins.h"
#include "..\ViewMdl\Functions.h"
#include "..\twiconn\Metadata.h"
#include "..\twitter\Plugins.h"
#include "..\twitter\Metadata.h"

#include "..\model-libs\json\src\JSON.h"
#include "..\model-libs\json\src\JSONValue.h"

#include "..\model-libs\layout\Plugins.h"
#include "..\model-libs\layout\Metadata.h"

using namespace IP;

#pragma comment (lib,"Gdiplus.lib")
