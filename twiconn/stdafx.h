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
#define __TWITCONN__

#define BOOST_LIB_DIAGNOSTIC 1

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlstr.h>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>
#include <unordered_map>

#include <regex>

#include <curl/curl.h>
#include <curl/easy.h>
#include <Winhttp.h>

#define APP_KEY "9FA16n89DvlcZCO07IkVHcxio"
#define APP_SECRET "stU8OFXzqG9dsp80fnH92d9iSsm11WpPIWnvctKYB7zpcz7WqX"

#include <initguid.h>
#include "..\model-libs\ObjMdl\Macros.h"
#include "..\model-libs\ObjMdl\Plugins.h"
#include "..\model-libs\ObjMdl\Functions.h"
#include "..\model-libs\ObjMdl\Metadata.h"

#include "Metadata.h"

#include "..\model-libs\json\src\JSON.h"
#include "..\model-libs\json\src\JSONValue.h"

//#pragma comment(lib, "ws2_32.lib")
//#pragma comment(lib, "wldap32.lib")
#pragma comment (lib,"Winhttp.lib")
