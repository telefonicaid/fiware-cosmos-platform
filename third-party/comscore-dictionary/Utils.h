#ifndef UTILS_H
#define UTILS_H

#include "stdafx.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

#include "UtilString.h"

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <direct.h>
#include <ShlObj.h>
#else
#define HANDLE int
#if defined(ENABLE_LINUX_DATABASE_LOAD)
#include <sql.h>
#include <sqlext.h>
#endif
#endif

#include <sstream>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

//#include <AtlBase.h>

#include "CSCFDException.h"
#include "ErrorCodes.h"
#ifdef _WIN32
#include "Logger.h"
#else
#include "Logger_Linux.h"
#endif

using namespace std ;

namespace Utils
{
	void CheckOutFilePrefixAndCreatePath( const string& strOutputFile );
	void ParseDomainFromHost(string strHost,string& strDomain/*, string strTopLevelDomains*/) ;
	void ParseURL_Ver2(string strURL, string& strDomain, string& strHost, string& strDir, string& strPage,
					string& strQueryString, string& strQueryData, string& strProtocol) ;
	void MergeFile ( const char* pszSourceFile , const char* pszTargetFile ) ;
	void MergeFile2 ( const char* pszSourceFile , const char* pszTargetFile ) ;
	void CheckBarrier ( const char* pszMutexName , HANDLE& hMutex ) ;
	void UnCheckBarrier ( HANDLE hMutex ) ;
#ifdef _WIN32
	std::string GetStringSetting(HKEY hKeyParent, LPCTSTR lpszKeyName, LPCTSTR szValueName, LPCTSTR szDefValue)  ;
	string GetVersion ( const char* pszFileName ) ;
#endif
} ;

#ifndef _WIN32
#if defined(ENABLE_LINUX_DATABASE_LOAD)
string extract_error ( const char* szFunctionName , SQLHANDLE handle , SQLSMALLINT type ) ;
#endif
#endif

#endif
