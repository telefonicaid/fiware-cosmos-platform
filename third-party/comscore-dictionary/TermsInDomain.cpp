#include "stdafx.h"
#include "TermsInDomain.h"
#include "column.h"
#include <iterator>
#include "DatabaseMang.h"
#include <algorithm>
#ifdef _WIN32
#include <atlbase.h>
#endif

#include "Utils.h"
#include "Logger.h"

CTermsInDomain::CTermsInDomain(C_MemBuffer *pMembuffer, const char* pszCachePath)
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
	m_pMembuffer = pMembuffer;
	m_pRoot = new CCFDTree(m_pMembuffer);
	
#ifdef _WIN32
	HRESULT hr = m_dbConn_Input.CreateInstance(__uuidof(Connection));	
	if ( S_OK != hr )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Failed to create instance of \"Connection\" class. HRESULT: [" << hex << hr << "]" ;
		throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_OS_ERROR ) ;
	}

	m_strMMXIDataSource = Utils :: GetStringSetting ( HKEY_LOCAL_MACHINE , "software\\comscore" , "MMXIDataSource" , "cfdapply.office.comscore.com" ) ;
#else
	m_strMMXIDataSource = "cfdapply.office.comscore.com" ;
	//m_strMMXIDataSource = "impetus-951" ;
#endif
#ifdef _WIN32
	objLogger.LogINFO ( "MMXIDataSource: [" , m_strMMXIDataSource.c_str ( ) , "]" ) ;
#endif

	if ( ! pszCachePath)
#ifdef _WIN32
		m_strCachePath = "D:\\cs_logs" ;
#else
		m_strCachePath = "/tmp/cs_logs" ;
#endif
	else
		m_strCachePath = pszCachePath ;

}


CTermsInDomain::~CTermsInDomain(void)
{
	if ( m_pRoot )
	{
		delete m_pRoot ;
		m_pRoot = NULL ;
	}
}

#ifdef _WIN32
bool CTermsInDomain::OpenDBConn_Input(string conn_string )
{
	bool db_open = false;
	try
	{
		printf("Opening Input DB Connection...\n");
		m_dbConn_Input->ConnectionTimeout = 300;		
		//Trying to acquire Connection
		unsigned int uiReconnect = 0;
		while(1)
		{
			try
			{
				if(uiReconnect == 40)
				{
					db_open = false;
					printf("\nDatabase Connection Failed after Retry count:40");	
					break;
				}				

				if(m_dbConn_Input->Open(conn_string.c_str(), "", "", NULL) == S_OK)
				{
					db_open = true;
					break;
				}
				else
				{
					printf("\nDatabase Connection failed count: %d",uiReconnect);
					++uiReconnect;
					Sleep(DBRETRYCONNECTTIME);
					printf("\nDatabase Connection Retry count: %d",uiReconnect);
					fflush(stdout);
				}
			}
			catch(_com_error &e)
			{
				++uiReconnect;
				ostringstream ErrStr;
				//ErrorHandler(e,ErrStr);
				printf("\n%s",ErrStr.str().c_str());	
				Sleep(DBRETRYCONNECTTIME);
			}
			catch(std::exception &e)
			{
				++uiReconnect;
				printf("\nUnknown Error in Database::Open()");
				Sleep(DBRETRYCONNECTTIME);
			}
			catch(...)
			{
				++uiReconnect;
				printf("\nUnknown Error in Database::Open()");
				fflush(stdout);
				Sleep(DBRETRYCONNECTTIME);
			}
		}

		if( db_open == true)
			printf("DB Input Connection Opened.\n");
		
		fflush(stdout);
	}
	catch(const _com_error& e)
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Error: (" << CS_ERROR_NO_DATABASE << "): " ;
		objOssErrorMessage << "DB Input Connection failed for " << (LPCSTR)e.Description ( ) << " on " << conn_string.c_str ( ) ;
		//Nilesh - TODO : Uncomment this Exception throwing logic after analysis
		//throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_NO_DATABASE ) ;
		//bstr_t errorInfo = e.Description;					
		printf("DB Input Connection failed for %s on %s\n", (LPCSTR) e.Description(), conn_string.c_str() );
		db_open=false;
	}
	catch(char *str)
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Error: (" << CS_ERROR_NO_DATABASE << "): " ;
		objOssErrorMessage << "DB Input connection failed for " << str ;
		//Nilesh - TODO : Uncomment this Exception throwing logic after analysis
		//throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_NO_DATABASE ) ;
		printf("DB Input Connection failed for %s.\n", str );
		db_open=false;
	}
	catch(...)
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Error: (" << CS_ERROR_NO_DATABASE << "): " ;
		objOssErrorMessage << "DB Input connection failed for something" ;
		//Nilesh - TODO : Uncomment this Exception throwing logic after analysis
		//throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_NO_DATABASE ) ;
		printf("DB Input Connection failed for something.\n" );
		db_open=false;
	}
	fflush(stdout);
	return db_open;

}

string CTermsInDomain::CheckString( _RecordsetPtr pRecordset, string column_name )
{
	string rc="";

	_variant_t column;
	
	column = pRecordset->GetCollect(column_name.c_str());
	
	if( column.vt != VT_NULL )
	{
		rc = (char *)_bstr_t(column);
	}

	return rc;

}
#endif


bool CTermsInDomain::bLoadTable()
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;

	//Nilesh - May 12, 2011 - Support for loading Terms In Domain from cache BEGIN
	string strTermsInDomainPermanentCacheFileName ( m_strCachePath ) ;

#ifdef _WIN32
	long lRowsLoaded = 0;
	strTermsInDomainPermanentCacheFileName += "\\cs_terms_in_domain_cache.bcp.gz" ;
#else
	strTermsInDomainPermanentCacheFileName += "/cs_terms_in_domain_cache.bcp.gz" ;
#endif

	if ( CTermsInDomain :: LoadTermInDomainTable ( strTermsInDomainPermanentCacheFileName ) )
		return true ;
	//Nilesh - May 12, 2011 - Support for loading Terms In Domain from cache END

#if ( defined(_WIN32) ) || ( !defined(_WIN32) && defined (ENABLE_LINUX_DATABASE_LOAD) )
	string strFileSuffix ;

	{
#ifdef _WIN32
		unsigned char* szGUIDString ;
		GUID objGUID ;

		CoCreateGuid ( &objGUID ) ;
		UuidToString ( &objGUID , &szGUIDString ) ;
#else
        char szGUIDString [ 40 ] = {0} ;
        uuid_t objUUID ;

		uuid_generate ( objUUID ) ;
        uuid_unparse ( objUUID , szGUIDString ) ;
        strFileSuffix = szGUIDString ;
#endif
		strFileSuffix = (char*) szGUIDString ;

#ifdef _WIN32
		RpcStringFree ( &szGUIDString ) ;
#endif
	}

	objLogger.LogINFO ( "Loading Terms In Domain from database" ) ;

	string strTermsInDomainTemporaryCacheFileName ( m_strCachePath ) ;
	gzFile foutTermsInDomain = NULL ;

#ifdef _WIN32
	strTermsInDomainTemporaryCacheFileName += "\\" ;
#else
	strTermsInDomainTemporaryCacheFileName += "/" ;
#endif
	strTermsInDomainTemporaryCacheFileName += "cs_terms_in_domain" ;
	strTermsInDomainTemporaryCacheFileName += "_" + strFileSuffix ;
	strTermsInDomainTemporaryCacheFileName += "_cache.bcp.gz" ;

	Utils :: CheckOutFilePrefixAndCreatePath ( strTermsInDomainTemporaryCacheFileName ) ;
	Utils :: CheckOutFilePrefixAndCreatePath ( strTermsInDomainPermanentCacheFileName ) ;

	if ( ! ( foutTermsInDomain = gzopen ( strTermsInDomainTemporaryCacheFileName.c_str ( ) , "wb" ) ) )
	{
		ostringstream objOssTemporaryCacheFileOpenError ;

		objOssTemporaryCacheFileOpenError << "Error: (" << CS_ERROR_FILE_CREATION_FAILED << "): " ;
		objOssTemporaryCacheFileOpenError << "Unable to open temporary cache file: " ;
		objOssTemporaryCacheFileOpenError << "[" << strTermsInDomainTemporaryCacheFileName.c_str ( ) << "]" ;
		objOssTemporaryCacheFileOpenError << " for compressed writing" ;
		objLogger.LogERROR ( objOssTemporaryCacheFileOpenError.str ( ).c_str ( ) ) ;
		throw CCSCFDException ( objOssTemporaryCacheFileOpenError.str ( ) , CS_ERROR_FILE_CREATION_FAILED ) ;
	}

	objLogger.LogINFO ( "Terms In Domain cache file name: [" , strTermsInDomainTemporaryCacheFileName.c_str ( ) , "]" ) ;

#ifdef _WIN32
	try
#endif
	{
#ifdef _WIN32
		string conn_string =  "Driver={SQL Server};Server=" + m_strMMXIDataSource + ";Database=AMW_CF1;Uid=seg_mmxi;Pwd=Segmmxi100";
		OpenDBConn_Input(conn_string );
#else
		string conn_string =  "Driver=FreeTDS;Server=" + m_strMMXIDataSource + ";Database=AMW_CF1;Uid=seg_mmxi;Pwd=Segmmxi100";
#endif
			
		string str_sql;	
		//str_sql = "SELECT LastTerm,	SecondLastTerm, TermCount, LastUpdatedAt FROM dbo.cs_terms_in_domain order by lastterm"; //where is_primary_c2_id = 1
		//Nilesh - June 07, 2011 - Order result of Terms in Domain retrieval query by Second Last Term
		//Change requested by Santosh
		//Done for fixing Pattern ID mismatch issue reported by Mita
		str_sql = "SELECT LastTerm,	SecondLastTerm, TermCount, LastUpdatedAt FROM dbo.cs_terms_in_domain order by secondlastterm"; //where is_primary_c2_id = 1

		printf("Executing %s\n", str_sql.c_str() );
		objLogger.LogINFO ( "Terms In Domain loading query: [" , str_sql.c_str ( ) , "]" ) ;
	
		string url;	
		long prec = 1000;
#ifdef _WIN32
		_CommandPtr pCommand;
		HRESULT hr=pCommand.CreateInstance(__uuidof(Command));
		if(SUCCEEDED(hr))
		{
			pCommand->ActiveConnection = m_dbConn_Input;
				
			pCommand->CommandText=str_sql.c_str();

			pCommand->CommandTimeout = 36000;

			_RecordsetPtr pRecordset;

			hr=pRecordset.CreateInstance(__uuidof(Recordset));
			if( SUCCEEDED(hr))
			{			
				FieldsPtr pFields=pRecordset->Fields;

				hr = pRecordset->Open((IDispatch *) pCommand,
					vtMissing, adOpenForwardOnly, adLockReadOnly, adCmdUnknown);			

				while( !pRecordset->EndOfFile )
				{
					std::string strSecondToLast;
					std::string strLast;
					std::string strCombined;
					lRowsLoaded++;

					std::string strTableLastTerm = CheckString( pRecordset, "LastTerm" );	
					std::string strTableSecondLastTerm = CheckString( pRecordset, "SecondLastTerm" );	
					std::string strCount = CheckString( pRecordset, "TermCount" );	
					int iCount = atoi(strCount.c_str());

					//Nilesh - May 12, 2011 - Support for creating Terms In Domain Cache - Dump the record to cache BEGIN
					gzprintf ( foutTermsInDomain , "%s\t%s\t%s\n" , strTableLastTerm.c_str ( ) , strTableSecondLastTerm.c_str ( ) , strCount.c_str ( ) ) ;
					//Nilesh - May 12, 2011 - Support for creating Terms In Domain Cache - Dump the record to cache END

					//strTableLastTerm = _strlwr((char *)strTableLastTerm.c_str());
					//strTableSecondLastTerm = _strlwr((char *)strTableSecondLastTerm.c_str());


					if ( !(strcmpi(strTableLastTerm.c_str(),"NULL") == 0 || strTableLastTerm.size() == 0))
					{
						strLast = strTableLastTerm;
					}
					else
					{
						strLast = "%";
					}

					//if ( strTableSecondLastTerm.size() > 0)
					if ( !(strcmpi(strTableSecondLastTerm.c_str(),"NULL") == 0 || strTableSecondLastTerm.size() == 0))
					{
						strSecondToLast = strTableSecondLastTerm;
					}
					else
					{
						strSecondToLast = "%";
					}

				
					strCombined = strSecondToLast;
					strCombined.append(".");
					strCombined.append(strLast);

				{
					//strCombined = string("%.")+strCombined;
					int ( *ptrFun )( int ) = toupper; 
					transform( strCombined.begin(), strCombined.end(), strCombined.begin(), ptrFun ); 
					//CMyTree::VALUE *value = (CMyTree::VALUE*) malloc(sizeof(CMyTree::VALUE));
					pair<unsigned __int16,unsigned __int16> value_pair;
					CCFDTree::VALUE *value = (CCFDTree::VALUE*) m_pMembuffer->pAllocBuffer(sizeof(CCFDTree::VALUE),value_pair.first,value_pair.second);
					value->pattern_id =  iCount;
					value->prec = prec;
					value->subprec = 0;
					prec--;
					m_pRoot->insert(strCombined.c_str(),value,value_pair);
					
				}		

					//mapMapping.insert(make_pair(name,plateform));									
					pRecordset->MoveNext();
				}

				// add com.com
				{
					pair<unsigned __int16,unsigned __int16> value_pair;
					CCFDTree::VALUE *value = (CCFDTree::VALUE*) m_pMembuffer->pAllocBuffer(sizeof(CCFDTree::VALUE),value_pair.first,value_pair.second);
					value->pattern_id =  2;
					value->prec = 1001;
					value->subprec = 0;
					prec++;
					m_pRoot->insert("COM.COM",value,value_pair);
				}
				
			}
		}
#else
        SQLHENV hEnvironment ;
        SQLHDBC hDatabaseConnection ;
        SQLHSTMT hStatement ;
        SQLCHAR sqlcharReturnedConnectionString [ 1024 ] ;
        SQLSMALLINT sqlsmallintReturnedConnectionStringLength ;
        SQLRETURN retVal ;

        SQLAllocHandle ( SQL_HANDLE_ENV , SQL_NULL_HANDLE , &hEnvironment ) ;
        SQLSetEnvAttr ( hEnvironment , SQL_ATTR_ODBC_VERSION , (void*) SQL_OV_ODBC3 , 0 ) ;
        SQLAllocHandle ( SQL_HANDLE_DBC , hEnvironment , &hDatabaseConnection ) ;
        retVal = SQLDriverConnect
                (
                        hDatabaseConnection ,
                        NULL ,
                        (SQLCHAR*) conn_string.c_str ( ) ,
                        SQL_NTS ,
                        sqlcharReturnedConnectionString ,
                        sizeof ( sqlcharReturnedConnectionString ) ,
                        &sqlsmallintReturnedConnectionStringLength ,
                        SQL_DRIVER_COMPLETE
                ) ;
        if ( SQL_SUCCEEDED ( retVal ) )
		{
	        fprintf ( stdout , "Returned connection string: [%s]\n" , sqlcharReturnedConnectionString ) ;
			objLogger.LogINFO ( "Returned connection string: [" , sqlcharReturnedConnectionString , "]" ) ;
            ostringstream objOssDictionaryLoadingQuery ;
            SQLAllocHandle ( SQL_HANDLE_STMT , hDatabaseConnection , &hStatement ) ;

			if ( ! SQL_SUCCEEDED ( SQLExecDirect ( hStatement , (SQLCHAR*) str_sql.c_str ( ) , str_sql.size ( ) ) ) )
            {
				ostringstream objOssErrorMessage ;

				objOssErrorMessage << "Error: (" << CS_ERROR_LOAD_FAILED << "): Error: " ;
				objOssErrorMessage << "[" << extract_error ( "bLoadTable" , hStatement , SQL_HANDLE_STMT ) << "]" ;
				objOssErrorMessage << " occurred while loading Terms In Domain from database" ;
				objLogger.LogERROR ( "Failed to load Terms In Domain from database." , objOssErrorMessage.str ( ).c_str ( ) ) ;
				throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_LOAD_FAILED ) ;
            }

			char szLastTerm [ 16 ] = {0} ;
			char szSecondLastTerm [ 16 ] = {0} ;
			int iTermCount = -1 ;
			SQLLEN indicator [ 3 ] ;
			SQLRETURN hRetVal ;

			SQLBindCol ( hStatement , 1 , SQL_C_CHAR , szLastTerm , 16 , &indicator [ 0 ] ) ;
			SQLBindCol ( hStatement , 2 , SQL_C_CHAR , szSecondLastTerm , 16 , &indicator [ 1 ] ) ;
			SQLBindCol ( hStatement , 3 , SQL_C_SLONG , &iTermCount , sizeof ( iTermCount ) , &indicator [ 2 ] ) ;

			while ( SQL_SUCCEEDED ( hRetVal = SQLFetch ( hStatement ) ) )
			{
				string strLast ;
				string strSecondLast ;
				string strCombined ;
				lRowsLoaded++;

				//Nilesh - May 17, 2011 - Fixed this for Linux - - Support for creating Terms In Domain Cache - Dump the record to cache BEGIN
				gzprintf ( foutTermsInDomain , "%s\t%s\t%i\n" , szLastTerm , szSecondLastTerm , iTermCount ) ;
				//Nilesh - May 17, 2011 - Fixed This for Linux - Support for creating Terms In Domain Cache - Dump the record to cache END

				if ( ( 0 >= strlen ( szLastTerm ) ) || ( SQL_NULL_DATA == indicator [ 0 ] ) )
					strLast = "%" ;
				else
					strLast = szLastTerm ;

				if ( ( 0 >= strlen ( szSecondLastTerm ) ) || ( SQL_NULL_DATA == indicator [ 0 ] ) )
					strSecondLast = "%" ;
				else
					strSecondLast = szSecondLastTerm ;

				strCombined = strSecondLast + "." + strLast ;

				//strCombined = string("%.")+strCombined;
				int ( *ptrFun )( int ) = toupper; 
				transform( strCombined.begin(), strCombined.end(), strCombined.begin(), ptrFun ); 
				//CMyTree::VALUE *value = (CMyTree::VALUE*) malloc(sizeof(CMyTree::VALUE));
				pair<unsigned __int16,unsigned __int16> value_pair;
				CCFDTree::VALUE *value = (CCFDTree::VALUE*) m_pMembuffer->pAllocBuffer(sizeof(CCFDTree::VALUE),value_pair.first,value_pair.second);
				value->pattern_id =  iTermCount;
				value->prec = prec;
				value->subprec = 0;
				prec--;
				m_pRoot->insert(strCombined.c_str(),value,value_pair);

				memset ( szLastTerm , 0 , 16 ) ;
				memset ( szSecondLastTerm , 0 , 16 ) ;
				iTermCount = 0 ;
			}

			// add com.com
			{
				pair<unsigned __int16,unsigned __int16> value_pair;
				CCFDTree::VALUE *value = (CCFDTree::VALUE*) m_pMembuffer->pAllocBuffer(sizeof(CCFDTree::VALUE),value_pair.first,value_pair.second);
				value->pattern_id =  2;
				value->prec = 1001;
				value->subprec = 0;
				prec++;
				m_pRoot->insert("COM.COM",value,value_pair);
			}
			SQLFreeHandle ( SQL_HANDLE_STMT , hStatement ) ;
		}
        else
        {
			ostringstream objOssErrorMessage ;

			objOssErrorMessage << "Error: (" << CS_ERROR_LOAD_FAILED << "): Error: " ;
			objOssErrorMessage << "[" << extract_error ( "bLoadTable" , hDatabaseConnection , SQL_HANDLE_DBC ).c_str ( ) << "]" ;
			objOssErrorMessage << " occurred while loading Terms In Domain from database" ;
			objLogger.LogERROR ( "Failed to load Terms in Domain from database." , objOssErrorMessage.str ( ).c_str ( ) ) ;
			throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_LOAD_FAILED ) ;
        }

        SQLDisconnect ( hDatabaseConnection ) ;
        SQLFreeHandle ( SQL_HANDLE_DBC , hDatabaseConnection ) ;
        SQLFreeHandle ( SQL_HANDLE_ENV , hEnvironment ) ;
#endif
	}
#ifdef _WIN32
	catch(_com_error error)
	{		
		//printf("error loading terms in domain %s\n");
		objLogger.LogERROR ( "Failed to load Terms In Domain from database" ) ;
		return false;
	}
	m_dbConn_Input->Close();
#endif

	gzclose ( foutTermsInDomain ) ;

	Utils::MergeFile ( strTermsInDomainTemporaryCacheFileName.c_str ( ) , strTermsInDomainPermanentCacheFileName.c_str ( ) ) ;
	unlink ( strTermsInDomainTemporaryCacheFileName.c_str ( ) ) ;

	if( lRowsLoaded != 0 )
	{
		printf("\n Loaded %d terms in domain rows", lRowsLoaded);
		objLogger.LogINFO ( "Loaded: [" , lRowsLoaded , "] Terms In Domain rows from database" ) ;
		return true;
	}
	else
	{
		printf("\n Unable to load Terms in domain");
		objLogger.LogERROR ( "Failed to load Terms In Domain from database" ) ;
		return false;
	}

	
#else
	return false ;
#endif
}

void CTermsInDomain :: SetCachePath ( const char* pszCachePath )
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;

	m_strCachePath = pszCachePath ;
	objLogger.LogINFO ( "Terms In Domain cache path set to: [" , pszCachePath , "]" ) ;
}

bool CTermsInDomain::LoadTermInDomainTable(string strTermsFile)
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
	objLogger.LogInfo ( "Loading Terms In Domain from flat file: [" , strTermsFile.c_str ( ) , "]" ) ;
//Nilesh - May 12, 2011 - Support for cache file synchronization BEGIN
	int lRowsLoaded = 0;

	//This *shouldn't* be done here
	//Utils :: CheckOutFilePrefixAndCreatePath ( strDictionaryCacheFileName ) ;

	//Nilesh - May 17, 2011 - Commented this because it was loeading to code crash in Linux
	//ColExtractor clxDictionary ;

#ifdef _WIN32
	if ( 0 != _access ( strTermsFile.c_str ( ) , 04 ) )
		return false ; //Since Permanent cache doesn't exist, it should be loaded from database first
#else
#ifdef ENABLE_LINUX_DATABASE_LOAD
	if ( 0 != access ( strTermsFile.c_str ( ) , 06 ) )
#else
	if ( 0 != access ( strTermsFile.c_str ( ) , 04 ) )
#endif
		return false ; //Since Permanent cache doesn't exist, it should be loaded from database first
#endif

	struct stat objCacheStat ;
	memset ( &objCacheStat , 0 , sizeof ( struct stat ) ) ;
	if ( 0 != stat ( strTermsFile.c_str ( ) , &objCacheStat ) )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
		objOssErrorMessage << "Error occurred: " ;
		objOssErrorMessage << "[" << strerror ( errno ) << "]" ;
		objOssErrorMessage << " while stat'ing Terms In Domain cache file: " ;
		objOssErrorMessage << "[" << strTermsFile.c_str ( ) << "]" ;

		objLogger.LogERROR ( "Terms In Domain load from flat file failed." , objOssErrorMessage.str ( ).c_str ( ) ) ;

		throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
	}

//#ifdef _WIN32
//	//Request Read rights on Permanent Cache file.
//	//In case this fails, it would mean that some other csCFD is loading the cache. Therefore, this csCFD should retry access after a small timeout
//	{
//	  HANDLE hAppend = INVALID_HANDLE_VALUE ;
//	  while ( true )
//	  {
//		  hAppend = CreateFile(TEXT(strTermsFile.c_str ( )), // open Two.txt
//					  GENERIC_READ,         // open for writing
//					  0,			          // do not share
//					  NULL,                     // no security
//					  OPEN_ALWAYS,              // open or create
//					  FILE_ATTRIBUTE_NORMAL,    // normal file
//					  NULL);                    // no attr. template
//		  if ( INVALID_HANDLE_VALUE == hAppend )
//		  {
//			  if ( ERROR_SHARING_VIOLATION == GetLastError ( ) )
//			  {
//				fprintf ( stderr, "Waiting for one sec and then checking for existence\n" ) ;
//				Sleep ( 100 ) ;
//
//				if ( 0 == _access ( strTermsFile.c_str ( ) , 0 ) )
//					continue ;	// ReApplying the checks because file existence doesn't mean that nobody else is accessing the file
//			  }
//			  else
//			  {
//				ostringstream objOssErrorMessage ;
//
//				objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
//				objOssErrorMessage << "Some unknown error: " ;
//				objOssErrorMessage << "[" << GetLastError ( ) << "]" ;
//				objOssErrorMessage << " while requesting unshared read on: " ;
//				objOssErrorMessage << "[" << strTermsFile.c_str ( ) << "]" ;
//
//				objLogger.LogERROR ( "Terms In Domain load from flat file failed." , objOssErrorMessage.str ( ).c_str ( ) ) ;
//
//				throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
//			  }
//		  }
//		  else
//		  {
//			  fprintf ( stderr , "Got access !! Ready To load from cache\n" ) ;
//			  break ;
//		  }
//	  }
//
//	  if (hAppend == INVALID_HANDLE_VALUE)
//	  {
//		  ostringstream objOssErrorMessage ;
//
//		  objOssErrorMessage << "Error: (" << CS_ERROR_FILE_NOT_FOUND << "): " ;
//		  objOssErrorMessage << "Could not open: " ;
//		  objOssErrorMessage << "[" << strTermsFile.c_str ( ) << "]" ;
//		  objOssErrorMessage << " , Error Code: " ;
//		  objOssErrorMessage << "[" << GetLastError ( ) << "]" ;
//
//		  objLogger.LogERROR ( "Terms In Domain load from flat file failed." , objOssErrorMessage.str ( ).c_str ( ) ) ;
//
//		  CloseHandle ( hAppend ) ;
//		  throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_FILE_NOT_FOUND ) ;
//	  }
//
//	  //In case control flow reaches this point, it is safe to load dictionary from cache
//	  CloseHandle ( hAppend ) ;
//	}
////#else
//#elif defined(ENABLE_LINUX_DATABASE_LOAD)
//	int fd = -1 ;
//	struct flock objFlock ;
//
//	fd = open ( strTermsFile.c_str ( ) , O_RDWR ) ;
//	if ( 0 > fd )
//	{
//		ostringstream objOssErrorMessage ;
//
//		objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
//		objOssErrorMessage << "Some unknown error: " ;
//		objOssErrorMessage << "[" << strerror ( errno ) << "]" ;
//		objOssErrorMessage << " while requesting write lock on: " ;
//		objOssErrorMessage << "[" << strTermsFile.c_str ( ) << "]" ;
//
//		objLogger.LogERROR ( "Terms In Domain load from flat file failed." , objOssErrorMessage.str ( ).c_str ( ) ) ;
//
//		throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
//	}
//	else
//	{
//		while ( true )
//		{
//			objFlock.l_type = F_WRLCK ;
//			objFlock.l_whence = SEEK_SET ;
//			objFlock.l_start = 0 ;
//			objFlock.l_len = 0 ;
//
//			if ( 0 > fcntl ( fd , F_SETLK , &objFlock ) )
//			{
//				if ( ( EACCES == errno ) || ( EAGAIN == errno ) )
//				{
//						fprintf ( stderr , "Waiting for one sec and then checking for existence\n" ) ;
//						sleep ( 1 ) ;
//						continue ;
//				}
//				else
//				{
//					ostringstream objOssErrorMessage ;
//
//					objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
//					objOssErrorMessage << "Some unknown error: " ;
//					objOssErrorMessage << "[" << strerror ( errno ) << "]" ;
//					objOssErrorMessage << " while requesting write lock on: " ;
//					objOssErrorMessage << "[" << strTermsFile.c_str ( ) << "]" ;
//
//					objLogger.LogERROR ( "Terms In Domain load from flat file failed." , objOssErrorMessage.str ( ).c_str ( ) ) ;
//
//					throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
//				}
//			}
//			else
//			{
//				fprintf ( stderr , "Got access !! Ready To load from cache\n" ) ;
//				break ;
//			}
//		}
//	}
////#endif
//
//	//{
//	//	struct tm objTmJan012000 = { 0 , 0 , 0 , 1 , 0 , 100 } ;
//	//	time_t objTimeJan012000 = mktime ( &objTmJan012000 ) - 86400 ;
//	//	m_lCacheDate = objCacheStat.st_mtime - objTimeJan012000 ;
//	//}
////#ifndef _WIN32
//		objFlock.l_type = F_UNLCK ;
//		objFlock.l_whence = SEEK_SET ;
//		objFlock.l_start = 0 ;
//		objFlock.l_len = 0 ;
//		if ( 0 > fcntl ( fd , F_SETLK , &objFlock ) )
//		{
//			ostringstream objOssErrorMessage ;
//
//			objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
//			objOssErrorMessage << "Some unknown error: " ;
//			objOssErrorMessage << "[" << strerror ( errno ) << "]" ;
//			objOssErrorMessage << " while requesting unlocking: " ;
//			objOssErrorMessage << "[" << strTermsFile.c_str ( ) << "]" ;
//
//			objLogger.LogERROR ( "Terms In Domain load from flat file failed." , objOssErrorMessage.str ( ).c_str ( ) ) ;
//
//			throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
//		}
//		close ( fd ) ;
//#endif
////Nilesh - May 12, 2011 - Support for cache file synchronization END

	//Nilesh - May 31, 2011 - New support for cache synchronization BEGIN

#ifdef _WIN32
	HANDLE hMutex ;
	Utils :: CheckBarrier ( "/csCFD/CacheSynchronizationMutex-ReadOnly" , hMutex ) ;
#else
#ifdef ENABLE_LINUX_DATABASE_LOAD
	HANDLE hMutex ;
	Utils :: CheckBarrier ( "/tmp/csCFD-CacheSynchronizationMutext-ReadOnly" , hMutex ) ;
#endif 
#endif
	//Nilesh - May 31, 2011 - New support for cache synchronization END
		ColExtractor oColExtractor;
		long prec = 1000;
		if(oColExtractor.ExtractFrom(strTermsFile.c_str()))
		{

			COLUMNS oCOLUMNS;

			printf("\n Loading - %s",strTermsFile.c_str());
			
			while (oColExtractor.Extract(oCOLUMNS))
			{
				lRowsLoaded++;
				std::string strSecondToLast;
				std::string strLast;
				std::string strCombined;

				std::string strTableLastTerm = oCOLUMNS[0];
				std::string strTableSecondLastTerm = oCOLUMNS[1];
				/*
#ifdef _WIN32
				strTableLastTerm = _strlwr((char *)strTableLastTerm.c_str());
				strTableSecondLastTerm = _strlwr((char *)strTableSecondLastTerm.c_str());
#else
				for ( size_t stLoop = 0 ; stLoop < strTableLastTerm.size ( ) ; stLoop ++ )
					strTableLastTerm [ stLoop ] = tolower ( strTableLastTerm [ stLoop ] ) ;
				for ( size_t stLoop = 0 ; stLoop < strTableSecondLastTerm.size ( ) ; stLoop ++ )
					strTableSecondLastTerm [ stLoop ] = tolower ( strTableSecondLastTerm [ stLoop ] ) ;
#endif*/

#ifdef _WIN32
				if ( !(strcmpi(strTableLastTerm.c_str(),"NULL") == 0 || strTableLastTerm.size() == 0))
#else
				if ( !(strcasecmp(strTableLastTerm.c_str(),"NULL") == 0 || strTableLastTerm.size() == 0))
#endif
				{
					strLast = strTableLastTerm;
				}
				else
				{
					strLast = "%";
				}

				//if ( strTableSecondLastTerm.size() > 0)
#ifdef _WIN32
				if ( !(strcmpi(strTableSecondLastTerm.c_str(),"NULL") == 0 || strTableSecondLastTerm.size() == 0))
#else
				if ( !(strcasecmp(strTableSecondLastTerm.c_str(),"NULL") == 0 || strTableSecondLastTerm.size() == 0))
#endif
				{
					strSecondToLast = strTableSecondLastTerm;
				}
				else
				{
					strSecondToLast = "%";
				}

				
				strCombined = strSecondToLast;
				strCombined.append(".");
				strCombined.append(strLast);


				{
					int ( *ptrFun )( int ) = toupper; 
					transform( strCombined.begin(), strCombined.end(), strCombined.begin(), ptrFun ); 

					pair<unsigned __int16,unsigned __int16> value_pair;
					CCFDTree::VALUE *value = (CCFDTree::VALUE*) m_pMembuffer->pAllocBuffer(sizeof(CCFDTree::VALUE),value_pair.first,value_pair.second);
#ifdef _WIN32
					value->pattern_id =  atol(oCOLUMNS[2].c_str());
#else
					value->pattern_id =  atol(oCOLUMNS[2]);
#endif
					value->prec = prec;
					value->subprec = 0;
					prec--;
					m_pRoot->insert(strCombined.c_str(),value,value_pair);
					
				}			
			}

				// add com.com
				{
					pair<unsigned __int16,unsigned __int16> value_pair;
					CCFDTree::VALUE *value = (CCFDTree::VALUE*) m_pMembuffer->pAllocBuffer(sizeof(CCFDTree::VALUE),value_pair.first,value_pair.second);
					value->pattern_id =  2;
					value->prec = 1001;
					value->subprec = 0;					
					m_pRoot->insert("COM.COM",value,value_pair);
				}
		}else
		{
			printf("\n Unable to Open ");
			return -1;
		}

#if ( defined(_WIN32) ) || ( !defined(_WIN32) && defined (ENABLE_LINUX_DATABASE_LOAD) )
		Utils :: UnCheckBarrier ( hMutex ) ;
#endif 
		// TO DO .. check if terms in Domain loaded properly
	if( lRowsLoaded != 0 )
	{
		printf("\n Loaded %i terms in domain rows\n", lRowsLoaded);
		objLogger.LogINFO ( "Loaded: [" , lRowsLoaded , "] Terms In Domain Rows from flat file" ) ;
		return true;
	}
	else
	{
		printf("\n Unable to load Terms in domain");
		objLogger.LogERROR ( "Unable to load Terms In Domain from flat file" ) ;
		return false;
	}
}

string CTermsInDomain::GetDomainFromHost(std::string &strHost)
{
	long NbrDomainTerms = 2;
	std::string strEntity = strHost;

	long lHostLength = strHost.size();
	char DomainType = 'i';

	for (int Index = 0; Index < lHostLength; Index++)
	{
		int lValue = (int)(strHost[Index]);

		if ((lValue != 46) &&                   // not "."
		    ((lValue < 48) || (lValue > 57)))   // not Between 0 and 9
		{
			DomainType = 'w';
			break; // Exit while loop
		}
	}

	std::vector<std::string> fields;
	fields.reserve(4);
	split_string(strHost, '.', std::back_inserter(fields));


	if(DomainType == 'w')
	{		
		if (fields.size() > 2)
		{
			long iLastField = fields.size() - 1;
			//NbrDomainTerms = Lookup(fields[iLastField].c_str(), fields[iLastField-1].c_str(), mpTermList);
			char szTemp[500];

			//Nilesh - Sep 09, 2011 - Fixed csCFD stuck up reported by Sahin BEGIN
			if ( fields [ iLastField - 1 ].size ( ) + fields [ iLastField ].size ( ) > 498 )
			{
				CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
				ostringstream objOssErrorMessage ;
				objOssErrorMessage << "Failed to extract Domain name from Host name" ;
				objOssErrorMessage << "Host name: [" << strHost.c_str ( ) << "]\n" ;
				objOssErrorMessage << "Last but one field: [" << fields [ iLastField - 1 ].c_str ( ) << "] Size: [" << fields [ iLastField - 1 ].size ( ) << "]\n" ;
				objOssErrorMessage << "Last field: [" << fields [ iLastField ].c_str ( ) << "] Size: [" << fields [ iLastField ].size ( ) << "]\n" ;
				objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
				return "" ;
			}
			//Nilesh - Sep 09, 2011 - Fixed csCFD stuck up reported by Sahin END

			sprintf(szTemp,"%s.%s",fields[iLastField-1].c_str(),fields[iLastField].c_str());

			//Nilesh - May 03, 2011 - Added support for bringing only the required thing to upper case BEGIN
			for ( unsigned int i = 0 ; i < strlen ( szTemp ) ; i ++ )
				szTemp [ i ] = toupper ( szTemp [ i ] ) ;
			//Nilesh - May 03, 2011 - Added support for bringing only the required thing to upper case END

			CCFDTree::VALUE *pValue = m_pRoot->MatchURL(szTemp);
			if( pValue != NULL)
			{
				NbrDomainTerms = pValue->pattern_id;				

			}

			long lIndex = fields.size() - NbrDomainTerms;

			strEntity = fields[lIndex];
			lIndex++;

			while (lIndex < (int)fields.size())
			{
				strEntity.append(".");
				strEntity.append(fields[lIndex]);
				lIndex++;
			}
		}				
	}
/*	else
	{
		if(fields.size()==4)
			strEntity = string(fields[0]) +  string(".") +  string(fields[1]) +  string(".") +  string(fields[2]);

	}*/

	return strEntity ;
}
