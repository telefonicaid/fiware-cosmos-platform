// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#ifdef _WIN32
#include "resource.h"
#include "csCFD_i.h"
#include "dllmain.h"

CcsCFDModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}

#endif
