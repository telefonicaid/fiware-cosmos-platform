// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifdef _WIN32

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED

#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit


#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>


#import "C:\Program Files\Common Files\System\ado\msado27.tlb" no_namespace rename("EOF","EndOfFile")
//Uncomment following if running on some machine older than Windows 7 OR If the Windows 7 has no SP1
//#import "C:\Users\nshastri\Downloads\New folder (3)\msado60_Backcompat_i386.tlb" no_namespace rename ("EOF","EndOfFile")
#include <list>

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>


#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
