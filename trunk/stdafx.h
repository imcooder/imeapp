// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#define _DDKIMM_H_
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include "tchar.h"
#include <windows.h>
#include <strsafe.h>
#include "bddebug.h"
#include <string>
// TODO: reference additional headers your program requires here
#include <strsafe.h>
#include <atlbase.h>
#include <atlwin.h>
#include <atlstr.h>
#include <atltypes.h>
#include <atlsync.h>

#ifdef UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif


#include "bddebug.h"
#include "Data.h"