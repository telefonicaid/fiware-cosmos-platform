// CFDMain.cpp : Implementation of CCFDMain

#include "stdafx.h"
#include "CFDMain.h"

//#define PROFILE_CSCFD

std::string ConvertBSTRToString(BSTR pSrc)
{
#ifdef _WIN32
	char *pszTemp	=	_com_util::ConvertBSTRToString(pSrc);
	std :: string strTemp	=	pszTemp;
	delete []pszTemp;
	return strTemp;
#else
	return pSrc ;
#endif
}

// CCFDMain

CCFDMain :: CCFDMain()
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
#ifdef _WIN32
	string strCOMServerBinary = Utils :: GetStringSetting ( HKEY_LOCAL_MACHINE , "software\\classes\\clsid\\{9E5821DB-777E-4D90-9319-23D09C9F27A3}\\InProcServer32" , NULL , "test" ) ;
	ostringstream objOssVersionMessage ;

	objOssVersionMessage << "COM Server: [" << strCOMServerBinary.c_str ( ) << "] Version: [" << Utils :: GetVersion ( strCOMServerBinary.c_str ( ) ).c_str ( ) << "]" ;
	objLogger.LogINFO ( objOssVersionMessage.str ( ).c_str ( ) ) ;
	objLogger.LogINFO ( "CCFDMain instance created" ) ;
	//objLogger.LogINFO ( "Setting Log level to: [" , ELogLevel :: LL_INFO , "]" ) ;
	objLogger.LogINFO ( "Setting Log level to: [2]" ) ;
#endif
#ifdef _WIN32
	objLogger.m_objCLoggerStaticInfo.SetLogLevel ( ELogLevel :: LL_INFO ) ;
#else
	objLogger.m_objCLoggerStaticInfo.SetLogLevel ( 2 ) ;
#endif
	try
	{
		m_pTermsInDomainMemBuffer = new C_MemBuffer ( ) ;
		//m_pTermsInDomain = new CTermsInDomain ( m_pTermsInDomainMemBuffer ) ;
		m_pObjCFDManager = NULL ;
#ifdef _WIN32
		m_strCachePath = "D:\\cs_logs" ;
		QueryPerformanceFrequency ( &m_liTimerFrequency ) ;
#else
		m_strCachePath = "/tmp/cs_logs/" ;
#endif
	}
	catch ( CCSCFDException& ex )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in CCFDMain constructor" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) ) ;
		throw ex ;
	}
	catch ( exception& ex )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in CCFDMain constructor" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) ) ;
		throw ex ;
	}
	catch ( ... )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in CCFDMain constructor" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) ) ;
		throw exception ( ) ;
	}
}

CCFDMain :: ~CCFDMain()
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;

#if defined(_WIN32) && defined(PROFILE_CSCFD)
	objLogger.LogINFO ( m_objpsApplyDictionary.m_ullCallCount , "ApplyDictionary calls performed in" , m_objpsApplyDictionary.m_ullTime , "micro sec" ) ;
	objLogger.LogINFO ( m_objpsApplyDictionaryUsingUrl.m_ullCallCount , "ApplyDictionaryUsingUrl calls performed in" , m_objpsApplyDictionaryUsingUrl.m_ullTime , "micro sec" ) ;
	objLogger.LogINFO ( m_objpsApplyDictionary2.m_ullCallCount , "ApplyDictionary2 calls performed in" , m_objpsApplyDictionary2.m_ullTime , "micro sec" ) ;
	objLogger.LogINFO ( m_objpsApplyDictionaryUsingUrl2.m_ullCallCount , "ApplyDictionaryUsingUrl2 calls performed in" , m_objpsApplyDictionaryUsingUrl2.m_ullTime , "micro sec" ) ;

	objLogger.LogINFO ( m_objpsApplyDictionaryAndComputeWeight.m_ullCallCount , "ApplyDictionaryAndComputeWeight calls performed in" , m_objpsApplyDictionaryAndComputeWeight.m_ullTime , "micro sec" ) ;
	objLogger.LogINFO ( m_objpsApplyDictionaryUsingUrlAndComputeWeight.m_ullCallCount , "ApplyDictionaryUsingUrlAndComputeWeight calls performed in" , m_objpsApplyDictionaryUsingUrlAndComputeWeight.m_ullTime , "micro sec" ) ;
	objLogger.LogINFO ( m_objpsApplyDictionaryAndComputeWeight2.m_ullCallCount , "ApplyDictionaryAndComputeWeight2 calls performed in" , m_objpsApplyDictionaryAndComputeWeight2.m_ullTime , "micro sec" ) ;
	objLogger.LogINFO ( m_objpsApplyDictionaryUsingUrlAndComputeWeight2.m_ullCallCount , "ApplyDictionaryUsingUrlAndComputeWeight2 calls performed in" , m_objpsApplyDictionaryUsingUrlAndComputeWeight2.m_ullTime , "micro sec" ) ;

	objLogger.LogINFO ( m_objpsApplyDictionary_AdNet.m_ullCallCount , "ApplyDictionary_AdNet calls performed in" , m_objpsApplyDictionary_AdNet.m_ullTime , "micro sec" ) ;
#endif

	if ( m_pObjCFDManager )
	{
		delete m_pObjCFDManager ;
		m_pObjCFDManager = NULL ;
	}

	if ( m_pTermsInDomain )
	{
		delete m_pTermsInDomain ;
		m_pTermsInDomain = NULL ;
	}

	if ( m_pTermsInDomainMemBuffer )
	{
		delete m_pTermsInDomainMemBuffer ;
		m_pTermsInDomainMemBuffer = NULL ;
	}
#ifdef _WIN32
	objLogger.LogINFO ( "CCFDMain instance released" ) ;
#endif
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::Init(INT lMode)
#else
bool CCFDMain :: Init ( INT lMode ) 
#endif
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
	try
	{
		objLogger.LogInfo ( "Cache path: [" , m_strCachePath.c_str ( ) , "]" ) ;
		m_pTermsInDomain = new CTermsInDomain ( m_pTermsInDomainMemBuffer , m_strCachePath.c_str ( ) ) ;
		m_pTermsInDomain->bLoadTable ( ) ;
		m_pObjCFDManager = new CCFDManager ( m_pTermsInDomain , lMode ) ;
		m_pObjCFDManager->SetCachePath ( m_strCachePath.c_str ( ) ) ;
		m_iMode = lMode ;
	}
	catch ( CCSCFDException& ex )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in Init" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in Init" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in Init" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::Cachepath(BSTR bstrCachePath)
#else
bool CCFDMain :: Cachepath ( BSTR bstrCachePath )
#endif
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;

	m_strCachePath = ConvertBSTRToString ( bstrCachePath ).c_str ( ) ;
	if ( m_pObjCFDManager )
	{
		m_pObjCFDManager->SetCachePath ( m_strCachePath.c_str ( ) ) ;
		objLogger.LogINFO ( "Cache Path changed to: [" , m_strCachePath.c_str ( ) , "]" ) ;
	}
	else
	{
		objLogger.LogINFO ( "Cache Path change to: [" , m_strCachePath.c_str ( ) , "] pending. To be done as soon as Init / InitFromTermsInDomainFlatFile gets called" ) ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::FilterHostList(BSTR bstrHosts)
#else
bool CCFDMain :: FilterHostList ( BSTR bstrHosts )
#endif
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
	try
	{
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
		m_pObjCFDManager->LoadFilterHostList ( ConvertBSTRToString ( bstrHosts ).c_str ( ) ) ;
	}
	catch ( CCSCFDException& ex )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in FilterHostList" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in FilterHostList" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in FilterHostList" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::FilterHostFile(BSTR bstrHostFileName)
#else
bool CCFDMain :: FilterHostFile ( BSTR bstrHostFileName )
#endif
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
	try
	{
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
		m_pObjCFDManager->LoadFilterHostFile ( ConvertBSTRToString ( bstrHostFileName ).c_str ( ) ) ;
	}
	catch ( CCSCFDException& ex )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in FilterHostFile" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in FilterHostFile" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in FilterHostFile" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::SetAppsMask(INT iAppMask)
#else
bool CCFDMain :: SetAppsMask ( INT iAppMask )
#endif
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;

	try
	{
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
		m_pObjCFDManager->SetAppsMask ( iAppMask ) ;
		objLogger.LogINFO ( "AppsMask set to: [" , iAppMask , "]" ) ;
	}
	catch ( CCSCFDException& ex )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in SetAppsMask" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in SetAppsMask" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in SetAppsMask" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::DisableNEDOM(void)
#else
bool CCFDMain :: DisableNEDOM ( void )
#endif
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;

	try
	{
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
		m_pObjCFDManager->DisableNEDOM ( ) ;
		objLogger.LogINFO ( "NEDOM entires won't be loaded" ) ;
	}
	catch ( CCSCFDException& ex )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in DisableNEDOM" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in DisableNEDOM" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in DisableNEDOM" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::ParseDomainFromHost(BSTR bstrHost, BSTR* bstrDomain)
#else
bool CCFDMain :: ParseDomainFromHost ( BSTR bstrHost , BSTR* bstrDomain )
#endif
{
	try
	{
		string strHostName = ConvertBSTRToString ( bstrHost ) ;
		string strDomain = m_pTermsInDomain->GetDomainFromHost ( strHostName ) ;
#ifdef _WIN32
		*bstrDomain = _com_util :: ConvertStringToBSTR ( strDomain.c_str ( ) ) ;
#else
		*bstrDomain = strDomain ;
#endif
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in ParseDomainFromHost" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in ParseDomainFromHost" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in ParseDomainFromHost" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::ParseURL(BSTR bstrURL, BSTR* bstrDomain, BSTR* bstrHost, BSTR* bstrDir, BSTR* bstrPage, BSTR* bstrQueryString)
#else
bool CCFDMain :: ParseURL ( BSTR bstrURL , BSTR* bstrDomain , BSTR* bstrHost , BSTR* bstrDir , BSTR* bstrPage , BSTR* bstrQueryString )
#endif
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
	try
	{
		string strDomain ;
		string strHost ;
		string strDirectory ;
		string strPage ;
		string strQueryString ;
		string strQueryData ;
		string strProtocol ;

		Utils :: ParseURL_Ver2 
			(
				ConvertBSTRToString ( bstrURL ) ,
				strDomain ,
				strHost ,
				strDirectory ,
				strPage ,
				strQueryString ,
				strQueryData ,
				strProtocol
			) ;

#ifdef _WIN32
		*bstrDomain = _com_util :: ConvertStringToBSTR ( ( m_pTermsInDomain->GetDomainFromHost ( strHost ) ).c_str ( ) ) ;
		*bstrHost = _com_util :: ConvertStringToBSTR ( strHost.c_str ( ) ) ;
		*bstrDir = _com_util :: ConvertStringToBSTR ( strDirectory.c_str ( ) ) ;
		*bstrPage = _com_util :: ConvertStringToBSTR ( strPage.c_str ( ) ) ;
		*bstrQueryString = _com_util :: ConvertStringToBSTR ( strQueryString.c_str ( ) ) ;
#else
		*bstrDomain = m_pTermsInDomain->GetDomainFromHost ( strHost ) ;
		*bstrHost = strHost ;
		*bstrDir = strDirectory ;
		*bstrPage = strPage ;
		*bstrQueryString = strQueryString ;
#endif
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in ParseURL" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in ParseURL" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in ParseURL" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::GetStats(LONG* plRecordsLoaded, LONG* plCacheDate, LONG* plNEDOMRecords)
#else
bool CCFDMain :: GetStats ( LONG* plRecordsLoaded , LONG* plCacheDate , LONG* plNEDOMRecords )
#endif
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
	try
	{
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
		m_pObjCFDManager->GetStats ( *plRecordsLoaded , *plCacheDate , *plNEDOMRecords ) ;
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in GetStats" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in GetStats" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in GetStats" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::LoadDictionary(BSTR bstrDictionaryName)
#else
bool CCFDMain :: LoadDictionary ( BSTR bstrDictionaryName )
#endif
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
	try
	{
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
		m_pObjCFDManager->LoadDictionary ( ConvertBSTRToString ( bstrDictionaryName ).c_str ( ) ) ;
		m_strDictionaryName = ConvertBSTRToString ( bstrDictionaryName ) ;
	}
	catch ( CCSCFDException& ex )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in LoadDictionary" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in LoadDictionary" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in LoadDictionary" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::ApplyDictionary(BSTR bstrHost, BSTR bstrQS, LONG* lPatternID)
#else
bool CCFDMain :: ApplyDictionary ( BSTR bstrHost , BSTR bstrQS , LONG* lPatternID )
#endif
{
	try
	{
		int iWeight = -1 ;
#if defined(_WIN32) && defined(PROFILE_CSCFD)
		LARGE_INTEGER liStartTime , liEndTime ;
		DWORD_PTR dwCurrentMask = SetThreadAffinityMask ( GetCurrentThread ( ) , 1 ) ;
		QueryPerformanceCounter ( &liStartTime ) ;
		SetThreadAffinityMask ( GetCurrentThread ( ) , dwCurrentMask ) ;
#endif

		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
		*lPatternID = m_pObjCFDManager->ApplyDictionary 
			(
				ConvertBSTRToString ( bstrHost ),
				ConvertBSTRToString ( bstrQS ) ,
				iWeight
			) ;
#if defined(_WIN32) && defined(PROFILE_CSCFD)
		dwCurrentMask = SetThreadAffinityMask ( GetCurrentThread ( ) , 1 ) ;
		QueryPerformanceCounter ( &liEndTime ) ;
		SetThreadAffinityMask ( GetCurrentThread ( ) , dwCurrentMask ) ;
		m_objpsApplyDictionary.m_ullTime += ( (liEndTime.QuadPart - liStartTime.QuadPart)*1000*1000 / m_liTimerFrequency.QuadPart ) ;
#endif

		m_objpsApplyDictionary.m_ullCallCount ++ ;
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in ApplyDictionary" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in ApplyDictionary" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in ApplyDictionary" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::ApplyDictionaryUsingUrl(BSTR bstrURL, LONG* lPatternID)
#else
bool CCFDMain :: ApplyDictionaryUsingUrl ( BSTR bstrURL , LONG* lPatternID )
#endif
{
	try
	{
#if defined(_WIN32) && defined(PROFILE_CSCFD)
		LARGE_INTEGER liStartTime , liEndTime ;
		DWORD_PTR dwCurrentMask = SetThreadAffinityMask ( GetCurrentThread ( ) , 1 ) ;
		QueryPerformanceCounter ( &liStartTime ) ;
		SetThreadAffinityMask ( GetCurrentThread ( ) , dwCurrentMask ) ;
#endif

		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
		string url ( ConvertBSTRToString ( bstrURL ) ) ;
		int iWeight = -1 ;

		// Remove prifix 
		// TODO

		// get host
		string host,qs;
		size_t pos = url.find("..");
		while(pos != string::npos)
		{
			url.erase(pos,1);
			pos = url.find("..");
		}

		
		pos = url.find("./");
		size_t pos1 = url.find("/");
		if(pos1  != string::npos)
		{
			if( pos !=string::npos && pos < pos1)
			{
				url.replace(pos,2,"/");
				pos1 = url.find("/");
			}

			size_t pos2 = url.find(":");
			if(pos2!= string::npos && pos2 < pos1)
			{
				url.erase(pos2,pos1-pos2);
				pos1 = url.find("/");
			}

			host = url.substr(0,pos1);	
			qs = url.substr(pos1,url.size());
		}
		else
		{
			pos1 = url.rfind(".");
			if( pos1 != string::npos && pos1 == url.size()-1)
				url.erase(pos1,1);

			pos1 = url.find(":");
			if(pos1!= string::npos)
			{
				url.erase(pos1,url.size());			
			}

			host = url;
			qs = "/";
		}

		*lPatternID = m_pObjCFDManager->ApplyDictionary
			(
				host ,
				qs ,
				iWeight
			) ;

#if defined (_WIN32) && defined(PROFILE_CSCFD)
		dwCurrentMask = SetThreadAffinityMask ( GetCurrentThread ( ) , 1 ) ;
		QueryPerformanceCounter ( &liEndTime ) ;
		SetThreadAffinityMask ( GetCurrentThread ( ) , dwCurrentMask ) ;
		m_objpsApplyDictionaryUsingUrl.m_ullTime += ( (liEndTime.QuadPart - liStartTime.QuadPart)*1000*1000 / m_liTimerFrequency.QuadPart ) ;
#endif

		m_objpsApplyDictionaryUsingUrl.m_ullCallCount ++ ;
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in ApplyDictionaryUsingUrl" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in ApplyDictionaryUsingUrl" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in ApplyDictionaryUsingUrl" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::ApplyDictionary2(BSTR bstrHost, BSTR bstrQS, BSTR bstrDomain, LONG* lPatternID)
#else
bool CCFDMain :: ApplyDictionary2 ( BSTR bstrHost , BSTR bstrQS , BSTR bstrDomain , LONG* lPatternID )
#endif
{
	try
	{
		int iWeight = -1 ;
#if defined(_WIN32) && defined(PROFILE_CSCFD)
		LARGE_INTEGER liStartTime , liEndTime ;
		DWORD_PTR dwCurrentMask = SetThreadAffinityMask ( GetCurrentThread ( ) , 1 ) ;
		QueryPerformanceCounter ( &liStartTime ) ;
		SetThreadAffinityMask ( GetCurrentThread ( ) , dwCurrentMask ) ;
#endif

		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
		*lPatternID = m_pObjCFDManager->ApplyDictionary
			(
				ConvertBSTRToString ( bstrHost ),
				ConvertBSTRToString ( bstrDomain ),
				ConvertBSTRToString ( bstrQS ) ,
				iWeight
			);
#if defined(_WIN32) && defined(PROFILE_CSCFD)
		dwCurrentMask = SetThreadAffinityMask ( GetCurrentThread ( ) , 1 ) ;
		QueryPerformanceCounter ( &liEndTime ) ;
		SetThreadAffinityMask ( GetCurrentThread ( ) , dwCurrentMask ) ;
		m_objpsApplyDictionary2.m_ullTime += ( (liEndTime.QuadPart - liStartTime.QuadPart)*1000*1000 / m_liTimerFrequency.QuadPart ) ;
#endif

		m_objpsApplyDictionary2.m_ullCallCount ++ ;
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in ApplyDictionary2" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in ApplyDictionary2" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in ApplyDictionary2" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::ApplyDictionaryUsingUrl2(BSTR bstrURL, BSTR bstrDomain, LONG* lPatternID)
#else
bool CCFDMain :: ApplyDictionaryUsingUrl2 ( BSTR bstrURL , BSTR bstrDomain , LONG* lPatternID )
#endif
{
	try
	{
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
#if defined(_WIN32) && defined(PROFILE_CSCFD)
		LARGE_INTEGER liStartTime , liEndTime ;
		DWORD_PTR dwCurrentMask = SetThreadAffinityMask ( GetCurrentThread ( ) , 1 ) ;
		QueryPerformanceCounter ( &liStartTime ) ;
		SetThreadAffinityMask ( GetCurrentThread ( ) , dwCurrentMask ) ;
#endif
		string url ( ConvertBSTRToString ( bstrURL ) ) ;
		int iWeight = -1 ;

		// Remove prifix 
		// get host
		string host,qs;
		size_t pos = url.find("..");
		while(pos != string::npos)
		{
			url.erase(pos,1);
			pos = url.find("..");
		}

		pos = url.find("./");
		size_t pos1 = url.find("/");
		if(pos1  != string::npos)
		{
			if( pos != string::npos && pos < pos1)
			{
				url.replace(pos,2,"/");
				pos1 = url.find("/");
			}

			size_t pos2 = url.find(":");
			if(pos2!= string::npos && pos2 < pos1)
			{
				url.erase(pos2,pos1-pos2);
				pos1 = url.find("/");
			}

			host = url.substr(0,pos1);	
			qs = url.substr(pos1,url.size());
		}
		else
		{
			pos1 = url.rfind(".");
			if( pos1 != string::npos && pos1 == url.size()-1)
				url.erase(pos1,1);

			host = url;
			//qs = "\/";
			//Nilesh - Apr 21, 2011 - Altered this expression to fix Compiler warning about unrecognized escape sequence
			qs = "/";
		}

		*lPatternID = m_pObjCFDManager->ApplyDictionary
			(
				host ,
				ConvertBSTRToString ( bstrDomain ) ,
				qs ,
				iWeight
			) ;
#if defined(_WIN32) && defined(PROFILE_CSCFD)
		dwCurrentMask = SetThreadAffinityMask ( GetCurrentThread ( ) , 1 ) ;
		QueryPerformanceCounter ( &liEndTime ) ;
		SetThreadAffinityMask ( GetCurrentThread ( ) , dwCurrentMask ) ;
		m_objpsApplyDictionaryUsingUrl2.m_ullTime += ( (liEndTime.QuadPart - liStartTime.QuadPart)*1000*1000 / m_liTimerFrequency.QuadPart ) ;
#endif

		m_objpsApplyDictionaryUsingUrl2.m_ullCallCount ++ ;
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in ApplyDictionaryUsingUrl2" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in ApplyDictionaryUsingUrl2" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in ApplyDictionaryUsingUrl2" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::CalculateDomainID(BSTR bstrInputString, BSTR* bstrDomainID, LONGLONG* pllDomainID)
#else
bool CCFDMain :: CalculateDomainID ( BSTR bstrInputString , BSTR* bstrDomainID , LONGLONG* pllDomainID )
#endif
{
	// TODO: Add your implementation code here
	string strDomainID ;

	try
	{
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
		*pllDomainID = m_pObjCFDManager->GetDomainID ( ConvertBSTRToString ( bstrInputString ).c_str ( ) , strDomainID ) ;
#ifdef _WIN32
		*bstrDomainID = _com_util :: ConvertStringToBSTR ( strDomainID.c_str ( ) ) ;
#else
		*bstrDomainID = strDomainID ;
#endif
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in CalculateDomainID" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in CalculateDomainID" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in CalculateDomainID" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}
	
	return S_OK;
}


#ifdef _WIN32
STDMETHODIMP CCFDMain::CalcMD5(BSTR bstrInputString, BSTR* bstrOutputHash)
#else
bool CCFDMain :: CalcMD5 ( BSTR bstrInputString , BSTR* bstrOutputHash )
#endif
{
	// TODO: Add your implementation code here

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::GetDictionaryList(BSTR* bstrDatabaseDictionaryTitles)
#else
bool CCFDMain :: GetDictionaryList ( BSTR* bstrDatabaseDictionaryTitles )
#endif
{
	// TODO: Add your implementation code here

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::ApplyDictionaryAndComputeWeight(BSTR bstrHost, BSTR bstrQS, INT* iWeight, LONG* lPatternID)
#else
bool CCFDMain :: ApplyDictionaryAndComputeWeight ( BSTR bstrHost , BSTR bstrQS , INT* iWeight , LONG* lPatternID )
#endif
{
	try
	{
		int iWt = -1 ;

		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
#if defined(_WIN32) && defined(PROFILE_CSCFD)
		LARGE_INTEGER liStartTime , liEndTime ;
		DWORD_PTR dwCurrentMask = SetThreadAffinityMask ( GetCurrentThread ( ) , 1 ) ;
		QueryPerformanceCounter ( &liStartTime ) ;
		SetThreadAffinityMask ( GetCurrentThread ( ) , dwCurrentMask ) ;
#endif
		*lPatternID = m_pObjCFDManager->ApplyDictionary 
			(
				ConvertBSTRToString ( bstrHost ),
				ConvertBSTRToString ( bstrQS ) ,
				iWt
			) ;

#if defined(_WIN32) && defined(PROFILE_CSCFD)
		dwCurrentMask = SetThreadAffinityMask ( GetCurrentThread ( ) , 1 ) ;
		QueryPerformanceCounter ( &liEndTime ) ;
		SetThreadAffinityMask ( GetCurrentThread ( ) , dwCurrentMask ) ;
		m_objpsApplyDictionaryAndComputeWeight.m_ullTime += ( (liEndTime.QuadPart - liStartTime.QuadPart)*1000*1000 / m_liTimerFrequency.QuadPart ) ;
#endif

		m_objpsApplyDictionaryAndComputeWeight.m_ullCallCount ++ ;
		*iWeight = iWt ;
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in ApplyDictionaryAndComputeWeight" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in ApplyDictionaryAndComputeWeight" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in ApplyDictionaryAndComputeWeight" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::ApplyDictionaryUsingUrlAndComputeWeight(BSTR bstrUrl, INT* iWeight, LONG* lPatternID)
#else
bool CCFDMain :: ApplyDictionaryUsingUrlAndComputeWeight ( BSTR bstrUrl , INT* iWeight , LONG* lPatternID )
#endif
{
	try
	{
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
#if defined(_WIN32) && defined(PROFILE_CSCFD)
		LARGE_INTEGER liStartTime , liEndTime ;
		DWORD_PTR dwCurrentMask = SetThreadAffinityMask ( GetCurrentThread ( ) , 1 ) ;
		QueryPerformanceCounter ( &liStartTime ) ;
		SetThreadAffinityMask ( GetCurrentThread ( ) , dwCurrentMask ) ;
#endif
		string url ( ConvertBSTRToString ( bstrUrl ) ) ;
		int iWt = -1 ;

		// Remove prifix 
		// TODO

		// get host
		string host,qs;
		size_t pos = url.find("..");
		while(pos != string::npos)
		{
			url.erase(pos,1);
			pos = url.find("..");
		}

		
		pos = url.find("./");
		size_t pos1 = url.find("/");
		if(pos1  != string::npos)
		{
			if( pos != string::npos && pos < pos1)
			{
				url.replace(pos,2,"/");
				pos1 = url.find("/");
			}

			size_t pos2 = url.find(":");
			if(pos2!= string::npos && pos2 < pos1)
			{
				url.erase(pos2,pos1-pos2);
				pos1 = url.find("/");
			}

			host = url.substr(0,pos1);	
			qs = url.substr(pos1,url.size());
		}
		else
		{
			pos1 = url.rfind(".");
			if( pos1 != string::npos && pos1 == url.size()-1)
				url.erase(pos1,1);

			pos1 = url.find(":");
			if(pos1!= string::npos)
			{
				url.erase(pos1,url.size());			
			}

			host = url;
			qs = "/";
		}

		*lPatternID = m_pObjCFDManager->ApplyDictionary
			(
				host ,
				qs ,
				iWt
			) ;

#if defined(_WIN32) && defined(PROFILE_CSCFD)
		dwCurrentMask = SetThreadAffinityMask ( GetCurrentThread ( ) , 1 ) ;
		QueryPerformanceCounter ( &liEndTime ) ;
		SetThreadAffinityMask ( GetCurrentThread ( ) , dwCurrentMask ) ;
		m_objpsApplyDictionaryUsingUrlAndComputeWeight.m_ullTime += ( (liEndTime.QuadPart - liStartTime.QuadPart)*1000*1000 / m_liTimerFrequency.QuadPart ) ;
#endif

		m_objpsApplyDictionaryUsingUrlAndComputeWeight.m_ullCallCount ++ ;
		*iWeight = iWt ;
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in ApplyDictionaryUsingUrlAndComputeWeight" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in ApplyDictionaryUsingUrlAndComputeWeight" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in ApplyDictionaryUsingUrlAndComputeWeight" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::ApplyDictionaryAndComputeWeight2(BSTR bstrHost, BSTR bstrQS, BSTR bstrDomainName, INT* iWeight, LONG* lPatternID)
#else
bool CCFDMain :: ApplyDictionaryAndComputeWeight2 ( BSTR bstrHost , BSTR bstrQS , BSTR bstrDomainName , INT* iWeight , LONG* lPatternID )
#endif
{
	try
	{
		int iWt = -1 ;

		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
#if defined(_WIN32) && defined(PROFILE_CSCFD)
		LARGE_INTEGER liStartTime , liEndTime ;
		DWORD_PTR dwCurrentMask = SetThreadAffinityMask ( GetCurrentThread ( ) , 1 ) ;
		QueryPerformanceCounter ( &liStartTime ) ;
		SetThreadAffinityMask ( GetCurrentThread ( ) , dwCurrentMask ) ;
#endif
		*lPatternID = m_pObjCFDManager->ApplyDictionary
			(
				ConvertBSTRToString ( bstrHost ),
				ConvertBSTRToString ( bstrDomainName ),
				ConvertBSTRToString ( bstrQS ) ,
				iWt
			);

#if defined(_WIN32) && defined(PROFILE_CSCFD)
		dwCurrentMask = SetThreadAffinityMask ( GetCurrentThread ( ) , 1 ) ;
		QueryPerformanceCounter ( &liEndTime ) ;
		SetThreadAffinityMask ( GetCurrentThread ( ) , dwCurrentMask ) ;
		m_objpsApplyDictionaryAndComputeWeight2.m_ullTime += ( (liEndTime.QuadPart - liStartTime.QuadPart)*1000*1000 / m_liTimerFrequency.QuadPart ) ;
#endif

		m_objpsApplyDictionaryAndComputeWeight2.m_ullCallCount ++ ;
		*iWeight = iWt ;
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in ApplyDictionaryAndComputeWeight2" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in ApplyDictionaryAndComputeWeight2" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in ApplyDictionaryAndComputeWeight2" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::ApplyDictionaryUsingUrlAndComputeWeight2(BSTR bstrUrl, BSTR bstrDomainName, INT* iWeight, LONG* lPatternID)
#else
bool CCFDMain :: ApplyDictionaryUsingUrlAndComputeWeight2 ( BSTR bstrUrl , BSTR bstrDomainName , INT* iWeight , LONG* lPatternID )
#endif
{
	try
	{
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
#if defined(_WIN32) && defined(PROFILE_CSCFD)
		LARGE_INTEGER liStartTime , liEndTime ;
		DWORD_PTR dwCurrentMask = SetThreadAffinityMask ( GetCurrentThread ( ) , 1 ) ;
		QueryPerformanceCounter ( &liStartTime ) ;
		SetThreadAffinityMask ( GetCurrentThread ( ) , dwCurrentMask ) ;
#endif
		string url ( ConvertBSTRToString ( bstrUrl ) ) ;
		int iWt = -1 ;

		// Remove prifix 
		// get host
		string host,qs;
		size_t pos = url.find("..");
		while(pos != string::npos)
		{
			url.erase(pos,1);
			pos = url.find("..");
		}

		pos = url.find("./");
		size_t pos1 = url.find("/");
		if(pos1  != string::npos)
		{
			if( pos != string::npos && pos < pos1)
			{
				url.replace(pos,2,"/");
				pos1 = url.find("/");
			}

			size_t pos2 = url.find(":");
			if(pos2!= string::npos && pos2 < pos1)
			{
				url.erase(pos2,pos1-pos2);
				pos1 = url.find("/");
			}

			host = url.substr(0,pos1);	
			qs = url.substr(pos1,url.size());
		}
		else
		{
			pos1 = url.rfind(".");
			if( pos1 != string::npos && pos1 == url.size()-1)
				url.erase(pos1,1);

			host = url;
			//qs = "\/";
			//Nilesh - Apr 21, 2011 - Altered this expression to fix Compiler warning about unrecognized escape sequence
			qs = "/";
		}

		*lPatternID = m_pObjCFDManager->ApplyDictionary
			(
				host ,
				ConvertBSTRToString ( bstrDomainName ) ,
				qs ,
				iWt
			) ;

#if defined(_WIN32) && defined(PROFILE_CSCFD)
		dwCurrentMask = SetThreadAffinityMask ( GetCurrentThread ( ) , 1 ) ;
		QueryPerformanceCounter ( &liEndTime ) ;
		SetThreadAffinityMask ( GetCurrentThread ( ) , dwCurrentMask ) ;
		m_objpsApplyDictionaryUsingUrlAndComputeWeight2.m_ullTime += ( (liEndTime.QuadPart - liStartTime.QuadPart)*1000*1000 / m_liTimerFrequency.QuadPart ) ;
#endif

		m_objpsApplyDictionaryUsingUrlAndComputeWeight2.m_ullCallCount ++ ;
		*iWeight = iWt ;
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in ApplyDictionaryUsingUrlAndComputeWeight2" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in ApplyDictionaryUsingUrlAndComputeWeight2" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in ApplyDictionaryUsingUrlAndComputeWeight2" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::EnableNonEssentialDomainExclusion(void)
#else
bool CCFDMain :: EnableNonEssentialDomainExclusion ( void )
#endif
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
	try
	{
#if !defined(_WIN32) && !defined(ENABLE_LINUX_DATABASE_LOAD)
		return E_FAIL ; 
#else
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
		m_pObjCFDManager->DisableNonEssentialDomainExclusion ( ) ;
		objLogger.LogINFO ( "Non Essential domain exclusion list loaded from database" ) ;
#endif
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in DisableNonEssentialDomainExclusion" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in DisableNonEssentialDomainExclusion" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in DisableNonEssentialDomainExclusion" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::InitFromTermsInDomainFlatFile(INT iMode, BSTR bstrTermsInDomainFlatFileName)
#else
bool CCFDMain::InitFromTermsInDomainFlatFile(INT iMode, BSTR bstrTermsInDomainFlatFileName)
#endif
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
	try
	{
		objLogger.LogInfo ( "Cache path: [" , m_strCachePath.c_str ( ) , "]" ) ;
		m_pTermsInDomain = new CTermsInDomain ( m_pTermsInDomainMemBuffer , m_strCachePath.c_str ( ) ) ;
		if ( ! m_pTermsInDomain->LoadTermInDomainTable ( ConvertBSTRToString ( bstrTermsInDomainFlatFileName ) ) )
		{
			ostringstream objOssErrorMessage ;

			objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
			objOssErrorMessage << "Error occurred: " ;
			objOssErrorMessage << "[" << strerror ( errno ) << "]" ;
			objOssErrorMessage << " while requesting Read-Write access on Terms In Domain flat file: " ;
			objOssErrorMessage << "[" << ConvertBSTRToString ( bstrTermsInDomainFlatFileName ).c_str ( ) << "]" ;

			throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
		}
		m_pObjCFDManager = new CCFDManager ( m_pTermsInDomain , iMode ) ;
		m_pObjCFDManager->SetCachePath ( m_strCachePath.c_str ( ) ) ;
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in InitFromTermsInDomainFlatFile" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in InitFromTermsInDomainFlatFile" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in InitFromTermsInDomainFlatFile" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::LoadDictionary_AdNet(BSTR bstrConfiguration, LONG* lRetVal)
#else
bool CCFDMain::LoadDictionary_AdNet(BSTR bstrConfiguration, LONG* lRetVal)
#endif
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;

	try
	{
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
		m_pObjCFDManager->LoadAdNetDictionary ( ConvertBSTRToString ( bstrConfiguration ).c_str ( ) ) ;
		m_strDictionaryName = ConvertBSTRToString ( bstrConfiguration ).c_str ( ) ;
		*lRetVal = 0 ;
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in LoadDictionary_AdNet" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		*lRetVal = -1 ;
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in LoadDictionary_AdNet" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		*lRetVal = -2 ;
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in LoadDictionary_AdNet" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		*lRetVal = -3 ;
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::ApplyDictionary_AdNet(BSTR bstrRequestURL, BSTR bstrReferURL, LONG* lPatternID)
#else
bool CCFDMain::ApplyDictionary_AdNet(BSTR bstrRequestURL, BSTR bstrReferURL, LONG* lPatternID)
#endif
{
	try
	{
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
#if defined(_WIN32) && defined(PROFILE_CSCFD)
		LARGE_INTEGER liStartTime , liEndTime ;
		DWORD_PTR dwCurrentMask = SetThreadAffinityMask ( GetCurrentThread ( ) , 1 ) ;
		QueryPerformanceCounter ( &liStartTime ) ;
		SetThreadAffinityMask ( GetCurrentThread ( ) , dwCurrentMask ) ;
#endif
		*lPatternID = m_pObjCFDManager->ApplyAdNetDictionary
										(
											ConvertBSTRToString ( bstrRequestURL ).c_str ( ) ,
											ConvertBSTRToString ( bstrReferURL ).c_str ( )
										) ;
#if defined(_WIN32) && defined(PROFILE_CSCFD)
		dwCurrentMask = SetThreadAffinityMask ( GetCurrentThread ( ) , 1 ) ;
		QueryPerformanceCounter ( &liEndTime ) ;
		SetThreadAffinityMask ( GetCurrentThread ( ) , dwCurrentMask ) ;
		m_objpsApplyDictionary_AdNet.m_ullTime += ( (liEndTime.QuadPart - liStartTime.QuadPart)*1000*1000 / m_liTimerFrequency.QuadPart ) ;
#endif

		m_objpsApplyDictionary_AdNet.m_ullCallCount ++ ;
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in ApplyDictionary_AdNet" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in ApplyDictionary_AdNet" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in ApplyDictionary_AdNet" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}

#if ( defined(_WIN32) ) || ( !defined(_WIN32) && defined (ENABLE_LINUX_DATABASE_LOAD) )
#ifdef _WIN32
STDMETHODIMP CCFDMain::IsDictionaryChangedInDB(BOOL* bResult)
#else
bool CCFDMain::IsDictionaryChangedInDB(bool* bResult)
#endif
{
	try
	{
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
		*bResult = m_pObjCFDManager->IsDictionaryInDBChanged ( ) ;
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in IsDictionaryChangedInDB" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in IsDictionaryChangedInDB" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in IsDictionaryChangedInDB" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}

	return S_OK;
}
#endif

#ifdef _WIN32
STDMETHODIMP CCFDMain::LoadDomainList_String(BSTR* bstrDomainList)
{
	try
	{
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
		*bstrDomainList = _com_util :: ConvertStringToBSTR ( m_pObjCFDManager->getDomainList ( ) ) ;
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in IsDictionaryChangedInDB" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in IsDictionaryChangedInDB" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in IsDictionaryChangedInDB" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}
	return S_OK;
}


STDMETHODIMP CCFDMain::LoadDomainList_Array(SAFEARRAY** arrayDomain)
{
	// TODO: Add your implementation code here
	try
	{
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
		set < string > setAdNetDomain ;
		m_pObjCFDManager->getDomainList ( setAdNetDomain ) ;

		SAFEARRAYBOUND bounds [] = {{setAdNetDomain.size ( ) , 0}} ;
		long lIndex [ 1 ] ;
		VARIANT var ;
		int iCount = 0 ;

		*arrayDomain = SafeArrayCreate ( VT_VARIANT , 1 , bounds ) ;

		for ( set < string > :: iterator itrSet = setAdNetDomain.begin ( ) ; itrSet != setAdNetDomain.end ( ) ; itrSet ++ )
		{
			OLECHAR* olecharAdNetDomain = new OLECHAR [ itrSet->size ( ) + 1 ] ;
			lIndex [ 0 ] = iCount ;
			var.vt = VT_BSTR ;

			mbstowcs ( olecharAdNetDomain , itrSet->c_str ( ) , itrSet->size ( ) ) ;
			var.bstrVal = ::SysAllocStringLen ( olecharAdNetDomain , itrSet->size ( ) ) ;
			SafeArrayPutElement ( *arrayDomain , lIndex , &var ) ;
			iCount ++ ;
		}
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in IsDictionaryChangedInDB" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in IsDictionaryChangedInDB" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in IsDictionaryChangedInDB" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}
	return S_OK;
}


STDMETHODIMP CCFDMain::LoadDomainList(LONG** handle)
{
	try
	{
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
		if ( ! m_setAdNetDomain.size ( ) )
			m_pObjCFDManager->getDomainList ( m_setAdNetDomain ) ;
		*handle = (LONG*) new set < string > :: iterator ( m_setAdNetDomain.begin ( ) ) ;
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in IsDictionaryChangedInDB" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in IsDictionaryChangedInDB" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in IsDictionaryChangedInDB" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}
	return S_OK;
}


STDMETHODIMP CCFDMain::GetNextDomain(LONG* handle, BSTR* bstrDomain)
{
	try
	{
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
		set < string > :: iterator* pItrSetAdNetDomain = reinterpret_cast < set < string > :: iterator* > (handle) ;
		if ( (*pItrSetAdNetDomain) != m_setAdNetDomain.end ( ) )
		{
			*bstrDomain = _com_util :: ConvertStringToBSTR ( (*pItrSetAdNetDomain)->c_str ( ) ) ;
			(*pItrSetAdNetDomain)++;
		}
		else
			*bstrDomain = _com_util :: ConvertStringToBSTR ( "" ) ;
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in IsDictionaryChangedInDB" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in IsDictionaryChangedInDB" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in IsDictionaryChangedInDB" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}
	return S_OK;
}


STDMETHODIMP CCFDMain::UnloadDomainList(LONG** handle)
{
	try
	{
		if ( ! m_pObjCFDManager )
			throw CCSCFDException ( "csCFD not initialized. Please call Init / InitTermsInDomainFromFlatFile first" , -1 ) ;
		set < string > :: iterator* pItrSetAdNetDomain = reinterpret_cast < set < string > :: iterator* > (*handle) ;
		delete pItrSetAdNetDomain ;
	}
	catch ( CCSCFDException& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "CCSCFDException: [" << ex.what ( ) << "] occurred in IsDictionaryChangedInDB" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( exception& ex )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "std::exception: [" << ex.what ( ) << "] occurred in IsDictionaryChangedInDB" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_FAIL ;
	}
	catch ( ... )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Unknown error occurred in IsDictionaryChangedInDB" ;
		objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
		fprintf ( stderr , "%s\n" , objOssErrorMessage.str ( ).c_str ( ) );
		return E_UNEXPECTED ;
	}
	return S_OK;
}
#endif

#ifdef _WIN32
STDMETHODIMP CCFDMain::GetMode(INT* iMode)
#else
bool CCFDMain::GetMode(INT* iMode)
#endif
{
	*iMode = m_iMode ;
	return S_OK;
}

#ifdef _WIN32
STDMETHODIMP CCFDMain::GetLoadDictionaryArg(BSTR* bstrDictionaryArg)
#else
bool CCFDMain::GetLoadDictionaryArg(BSTR* bstrDictionaryArg)
#endif
{
#ifdef _WIN32
	*bstrDictionaryArg = _com_util :: ConvertStringToBSTR ( m_strDictionaryName.c_str ( ) ) ;
#else
	*bstrDictionaryArg = m_strDictionaryName ;
#endif
	return S_OK;
}
