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
#define __TWITTER__

#define _CRT_SECURE_NO_WARNINGS
#define BOOST_LIB_DIAGNOSTIC 1

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

#include <vector>
#include <map>
#include <list>
#include <hash_set>
#include <mutex>
#include <boost/lexical_cast.hpp>

#pragma warning(push)
#pragma warning(disable:4245)
#include <boost\date_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#pragma warning(pop)

#include <initguid.h>
#include "..\ObjMdl\Macros.h"
#include "..\ObjMdl\ManualComObjectLoader.h"
#include "..\ObjMdl\Plugins.h"
#include "..\ObjMdl\Functions.h"

#include "..\ViewMdl\Plugins.h"
#include "..\ViewMdl\Functions.h"

#include "..\model-libs\asyncsvc\Plugins.h"

#pragma comment (lib,"Gdiplus.lib")
#pragma comment (lib,"Winhttp.lib")
#ifdef DEBUG
#pragma comment (lib,"..\\model-libs\\boost\\stage\\lib\\libboost_system-vc120-mt-sgd-1_55.lib")
#else
#pragma comment (lib,"..\\model-libs\\boost\\stage\\lib\\libboost_system-vc120-mt-s-1_55.lib")
#endif
