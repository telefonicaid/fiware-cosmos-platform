#include "stdafx.h"
#include "CFDManager.h"

CCFDManager::CCFDManager(CTermsInDomain* pTermsInDomain, int iMode)
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
	//The Passed terms in domain pointer is already loaded !!
	m_pTermsInDomain = pTermsInDomain ;

	m_lRowsLoaded = -1 ;
	m_lCacheDate = -1 ;
	m_lNEDOMRecords = -1 ;

	//A true value in here will indicate that NEDOMs have to be loaded into dictionary
	m_bNEDOM = true ;
	m_iAppsMask = 1 ;
	m_pDictionaryMemBuffer = new C_MemBuffer ( ) ;

#ifdef _WIN32
	m_strCachePath = "D:\\cs_logs" ;
#else
	m_strCachePath = "/tmp/cs_logs" ;
#endif

	m_iMode = iMode ;

	m_iCFDWildcardCount = 0 ;
	m_iCFDTreeCount = 0 ;

#ifdef _WIN32
	if ( TIME_OPTIMIZED_MODE == m_iMode )
		objLogger.LogINFO ( "Mode: [" , iMode , "] (Speed Optimized Mode)" ) ;
	else if ( SPACE_OPTIMIZED_MODE == m_iMode )
		objLogger.LogINFO ( "Mode: [" , iMode , "] (Space Optimized Mode)" ) ;
	else if ( 0 >= m_iMode )
	{
		ostringstream objOssInvalidModeErrorMessage ;

		objOssInvalidModeErrorMessage << "Invalid mode: [" << m_iMode << "] specified. Value must be greater than 0" ;
		throw CCSCFDException ( objOssInvalidModeErrorMessage.str ( ) , -1 ) ;
	}
	else
		objLogger.LogINFO ( "Mix Mode. Record threshold: [" , m_iMode , "]" ) ;
#endif

	m_bDictionaryNameFlatFile = false ;

	m_pAdNetTree = NULL ;
	m_arrayCCFDTree = NULL ;
	m_iPercentEntryCount = 0 ;

	m_bLoadAdNetDictionary = false ;

	m_pszAdNetDomainList = NULL ;
}


CCFDManager::~CCFDManager(void)
{
	if ( m_pDictionaryMemBuffer )
	{
		delete m_pDictionaryMemBuffer ;
		m_pDictionaryMemBuffer = NULL ;
	}

	for ( vector < const char* > :: iterator vItr = m_vecHostList.begin ( ) ; vItr != m_vecHostList.end ( ) ; vItr ++ )
		free ( (void*) *vItr ) ;

	if ( m_arrayCCFDTree )
	{
		for ( int i = 0 ; i < m_iPercentEntryCount ; i ++ )
			free ( m_arrayCCFDTree [ i ].first ) ;

		delete[] m_arrayCCFDTree ;
		m_arrayCCFDTree = NULL ;
	}

	if ( m_pszAdNetDomainList )
	{
		free ( m_pszAdNetDomainList ) ;
		m_pszAdNetDomainList = NULL ;
	}

	for( map < string , CCFDBase* >::iterator it = m_mapDictionary.begin(); it!= m_mapDictionary.end(); it++)
	{
			if( it->second != NULL)
				delete it->second;
	}

	for( map < string , CCFDBase* >::iterator it = m_mapWildcardDictionary.begin(); it!= m_mapWildcardDictionary.end(); it++)
	{
			if( it->second != NULL)
				delete it->second;
	}

	
}

void CCFDManager :: SetCachePath ( const char* pszCachePath )
{
	m_strCachePath = pszCachePath ;
}

void CCFDManager :: LoadDictionary ( const char* pszDictionaryName )
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
	m_strTableName = pszDictionaryName ;

#ifdef _WIN32
	if ( ( strchr ( pszDictionaryName , '\\' ) ) )
#else
	if ( ( strchr ( pszDictionaryName , '/' ) ) )
#endif
	{
#ifdef _WIN32
		objLogger.LogINFO ( "Considering supplied argument: [" , pszDictionaryName , "] as Flat file name" ) ;
#endif
		m_strPermanentCacheFileName = pszDictionaryName ;
		m_bDictionaryNameFlatFile = true ;
		if ( ! CCFDManager :: loadDictionaryFromCache ( ) )
		{
			ostringstream objOssErrorMessage ;

			objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
			objOssErrorMessage << "Error occurred: " ;
			objOssErrorMessage << "[" << strerror ( errno ) << "]" ;
			objOssErrorMessage << " while requesting Read-Write access on Dictionary Cache flat file: " ;
			objOssErrorMessage << "[" << m_strPermanentCacheFileName.c_str ( ) << "]" ;

			throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
		}

		return ;
	}
#ifdef _WIN32
	else
		objLogger.LogINFO ( "Considering supplied argument: [" , pszDictionaryName , "] as Dictionary name" ) ;
#endif

#if ( defined(_WIN32) ) || ( !defined(_WIN32) && defined (ENABLE_LINUX_DATABASE_LOAD) )
	CCFDManager :: loadConfiguration ( ) ;
#endif

#ifdef _WIN32
	m_strPermanentCacheFileName = m_strCachePath + "\\" + m_strTableName + "_cache.bcp.gz" ;
#else
	m_strPermanentCacheFileName = m_strCachePath + "/" + m_strTableName + "_cache.bcp.gz" ;
#endif

	if ( ! CCFDManager :: loadDictionaryFromCache ( ) )
	{
#if ( defined(_WIN32) ) || ( !defined(_WIN32) && defined (ENABLE_LINUX_DATABASE_LOAD) )
		CCFDManager :: loadDictionaryFromDatabase ( ) ;
		Utils :: MergeFile ( m_strTemporaryCacheFileName.c_str ( ) , m_strPermanentCacheFileName.c_str ( ) ) ;
		unlink ( m_strTemporaryCacheFileName.c_str ( ) ) ;
		if ( ! CCFDManager :: loadDictionaryFromCache ( ) )
#endif
		{
			ostringstream objOssErrorMessage ;

			objOssErrorMessage << "Error: (" << CS_ERROR_EXCEPTION << "): " ;
			objOssErrorMessage << "Dictionary loading from cache: [" << m_strPermanentCacheFileName.c_str ( ) << "] failed" ;
#if ( defined(_WIN32) ) || ( !defined(_WIN32) && defined (ENABLE_LINUX_DATABASE_LOAD) )
			objOssErrorMessage << " after caching from database" ;
#endif
			throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_EXCEPTION ) ;
		}
	}
}

void CCFDManager :: LoadAdNetDictionary ( const char* pszConfiguration )
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;

	m_bLoadAdNetDictionary = true ;

	CCFDManager :: LoadDictionary ( pszConfiguration ) ;

	return ;
#if 0
#ifdef _WIN32
	try
#endif
	{
#ifdef _WIN32
		string strConnectionString =  "Driver={SQL Server};Server=" + m_pTermsInDomain->m_strMMXIDataSource + ";Database=AMW_CF1;Uid=seg_mmxi;Pwd=Segmmxi100";
#else
		string strConnectionString =  "Driver=FreeTDS;Server=" + m_pTermsInDomain->m_strMMXIDataSource + ";Database=AMW_CF1;Uid=seg_mmxi;Pwd=Segmmxi100";
#endif
		ostringstream objOssDictionaryLoadingQuery ;

		objLogger.LogINFO ( "Before opening db connection" ) ;
#ifdef _WIN32
		m_pTermsInDomain->OpenDBConn_Input ( strConnectionString ) ;
#endif
		objOssDictionaryLoadingQuery << "SELECT pattern_id, request_url, refer_url FROM "  << pszConfiguration
			//<< " WHERE pattern_id=1003080403"
			//<< " WHERE refer_url='%' AND request_url LIKE '%msn.com%'" 
			<< " ORDER BY len(request_url) DESC, len(refer_url) DESC" ;
			//<< " WHERE refer_url='%' AND request_url LIKE '%msn.com%'" ;
			//<< " WHERE request_url LIKE '%btrll.com%'" ;

		fprintf ( stderr , "Now running Ad Net dictionary loading query: [%s]\n" , objOssDictionaryLoadingQuery.str ( ).c_str ( ) ) ;
		objLogger.LogINFO ( "Ad Net Dictionary loading query: [" , objOssDictionaryLoadingQuery.str ( ).c_str ( ) , "]" ) ;

#ifdef _WIN32
		_CommandPtr pCommand ;
		if ( SUCCEEDED ( pCommand.CreateInstance ( __uuidof ( Command ) ) ) )
		{
			pCommand->ActiveConnection = m_pTermsInDomain->m_dbConn_Input ;
			pCommand->CommandText = objOssDictionaryLoadingQuery.str ( ).c_str ( ) ;
			pCommand->CommandTimeout = 36000 ;

			_RecordsetPtr pRecordSet ;
			if ( SUCCEEDED ( pRecordSet.CreateInstance ( __uuidof ( Recordset ) ) ) )
			{
				FieldsPtr pFields = pRecordSet->Fields ;

				if ( SUCCEEDED ( pRecordSet->Open ( (IDispatch*) pCommand , vtMissing , adOpenForwardOnly , adLockReadOnly , adCmdUnknown ) ) )
				{
					//map < string , CCFDBase* > :: iterator itrMapSearchResult = m_mapDictionary.end ( ) ;
					CCFDBase* pCFDBase = NULL ;
					long lPrecedence = 2147483647 ;
					int ( *ptrFun )( int ) = toupper; 
					m_lRowsLoaded = 0 ;
					m_lNEDOMRecords = 0 ;

					//Pushing all Domain names to Upper case
					for ( vector < string > :: iterator vItr = m_vecKeepDomainList.begin ( ) ; vItr != m_vecKeepDomainList.end ( ) ; vItr ++ )
						transform ( vItr->begin ( ) , vItr->end ( ) , vItr->begin ( ) , ptrFun ) ;

					while ( !pRecordSet->EndOfFile )
					{
						bool bIsNEDOM = false ;
						//Nilesh - TODO - Discuss cs_digapp / DatasetType = A with Santosh and do needful modifications
						string strPatternID = m_pTermsInDomain->CheckString ( pRecordSet , "pattern_id" ) ;
						string strRequestURL = m_pTermsInDomain->CheckString ( pRecordSet , "request_url" ) ;
						string strReferURL = m_pTermsInDomain->CheckString ( pRecordSet , "refer_url" ) ;

						m_lRowsLoaded ++ ;
						//transform( strEntityID.begin(), strEntityID.end(), strEntityID.begin(), ptrFun );
						transform( strRequestURL.begin(), strRequestURL.end(), strRequestURL.begin(), ptrFun );
						transform( strReferURL.begin(), strReferURL.end(), strReferURL.begin(), ptrFun );

						if ( 0 == ( m_lRowsLoaded % 1000000 ) )
							fprintf ( stdout , "Ad Net Rows loaded: %i\n" , m_lRowsLoaded ) ;

						//Nilesh - TODO AT this point add logic to load the AdNet row
#if 0
						if ( '%' != strRequestURL [ strRequestURL.size ( ) - 1 ] )
							strRequestURL.append ( "%" ) ;
						if ( '%' != strReferURL [ strReferURL.size ( ) - 1 ] )
							strReferURL.append ( "%" ) ;

						pair < unsigned __int16 , unsigned __int16 > value_pair ;
						CCFDTree :: VALUE* pValue = (CCFDTree :: VALUE*) m_pDictionaryMemBuffer->pAllocBuffer ( sizeof ( CCFDTree :: VALUE) , value_pair.first , value_pair.second ) ;

						pValue->pattern_id = atoi ( strPatternID.c_str ( ) ) ;
						pValue->prec = lPrecedence ++ ;
						pValue->subprec = 0 ;

						m_pAdNetTree->insert ( ( strRequestURL + "\t" + strReferURL ).c_str ( ) , pValue , value_pair ) ;
#endif
						string strMapKey ;
						string strRequestDomain ;
						string strRequestKey ;
						string strReferDomain ;
						string strReferKey ;
						map < string , CCFDTree* > :: iterator mapItr = m_mapAdNet.end ( ) ;
						CCFDTree* pTargetTree = NULL ;

						if
						(
							( ! CCFDManager :: prepareKey ( strRequestURL , strRequestDomain , strRequestKey ) ) ||
							( ! CCFDManager :: prepareKey ( strReferURL , strReferDomain , strReferKey ) )
						)
						{
							fprintf ( stderr , "Skipping: %s\t%s\t%s\n" , strPatternID.c_str ( ) , strRequestURL.c_str ( ) , strReferURL.c_str ( ) ) ;
							pRecordSet->MoveNext ( ) ;
							continue ;
						}

						strMapKey = strRequestDomain + "\t" + strReferDomain ;

						if ( m_mapAdNet.end ( ) == ( mapItr = m_mapAdNet.find ( strMapKey ) ) )
						{
							pTargetTree = new CCFDTree ( m_pDictionaryMemBuffer ) ;
							m_mapAdNet.insert ( make_pair ( strMapKey , pTargetTree ) ) ;
						}
						else
							pTargetTree = mapItr->second ;

						pair < unsigned __int16 , unsigned __int16 > value_pair ;
						CCFDTree :: VALUE* pValue = (CCFDTree :: VALUE*) m_pDictionaryMemBuffer->pAllocBuffer ( sizeof ( CCFDTree :: VALUE) , value_pair.first , value_pair.second ) ;

						pValue->pattern_id = atoi ( strPatternID.c_str ( ) ) ;
						pValue->prec = lPrecedence -- ;
						pValue->subprec = 0 ;

						if ( strRequestKey [ strRequestKey.size ( ) - 1 ] != '%' )
							strRequestKey.append ( "%" ) ;
						if ( strReferKey [ strReferKey.size ( ) - 1 ] != '%' )
							strReferKey.append ( "%" ) ;
						//fprintf ( stderr , "[" ) ;
						//fwrite ( strMapKey.c_str ( ) , 1 , strMapKey.size ( ), stderr ) ;
						//fprintf ( stderr , "]" ) ;
						//fprintf ( stderr , "[" ) ;
						//fwrite ( (strRequestKey + "\t" + strReferKey).c_str ( ) , 1 , (strRequestKey + "\t" + strReferKey).size ( ), stderr ) ;
						//fprintf ( stderr , "]\n" ) ;

						pTargetTree->insert ( ( strRequestKey + "\t" + strReferKey ).c_str ( ) , pValue , value_pair ) ;

						pRecordSet->MoveNext ( ) ;
					}
					fprintf ( stdout , "Ad Net Rows loaded: %i\n" , m_lRowsLoaded ) ;
					objLogger.LogINFO ( "Loaded: [" , m_lRowsLoaded , "] dictionary rows from database" ) ;

					fprintf ( stdout , "Counting number of domain name entries having a %\n" ) ;
					int iPercentEntryCount = 0 ;
					vector < map < string , CCFDTree* > :: iterator > vecMapItrPercentLocation ;
					for ( map < string , CCFDTree* > :: iterator mapItr = m_mapAdNet.begin ( ) ; mapItr != m_mapAdNet.end ( ) ; mapItr ++ )
					{
						if ( string :: npos != mapItr->first.find ( "%" ) )
						{
							iPercentEntryCount ++ ;
							vecMapItrPercentLocation.push_back ( mapItr ) ;
						}
					}
					fprintf ( stdout , "Found: [%i] entries having a % in them\n" , iPercentEntryCount ) ;

					m_arrayCCFDTree = new pair < char* , CCFDTree* > [ iPercentEntryCount ] ;
					m_iPercentEntryCount = iPercentEntryCount ;
					int iLoop  = 0 ;
					for ( vector < map < string , CCFDTree* > :: iterator > :: iterator vItr = vecMapItrPercentLocation.begin ( ) ; vItr != vecMapItrPercentLocation.end ( ) ; vItr ++ )
					{
						m_arrayCCFDTree [ iLoop ].first = strdup ( (*vItr)->first.c_str ( ) ) ;
						m_arrayCCFDTree [ iLoop ].second = (*vItr)->second ;
						m_mapAdNet.erase ( (*vItr) ) ;
						iLoop ++ ;
					}

					sort ( m_arrayCCFDTree , m_arrayCCFDTree + m_iPercentEntryCount , AdNetPredicate ) ;

					FILE* fout = fopen ( "Dump.bcp" , "wb" ) ;
					for ( int i = 0 ; i < m_iPercentEntryCount ; i ++ )
					{
						fwrite ( m_arrayCCFDTree [ i ].first , 1 , strlen ( m_arrayCCFDTree [ i ].first ) , fout ) ;
						fprintf ( fout , "\n" ) ;
					}
					fclose ( fout ) ;

					fprintf ( stdout , "Converted percent entries to array\n" ) ;
				}
			}
		}
#else
#error "AdNet dictionary loading not supported on Linux"
#endif
	}
#ifdef _WIN32
	catch(_com_error& error)
	{		
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Error: (" << CS_ERROR_LOAD_FAILED << "): COM Error: " ;
		objOssErrorMessage << "[" << error.Error ( ) << "]=[" << error.ErrorMessage ( ) << "]" ;
		objOssErrorMessage << " occurred while loading Ad Net dictionary from database" ;
		throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_LOAD_FAILED ) ;
	}

	m_pTermsInDomain->m_dbConn_Input->Close();
#endif
#endif

}

bool CCFDManager :: prepareKey ( const string& strURL , string& strDomain , string& strHostName , string& strKey )
{
	bool bRetVal = true ;

	string strHost ;
	size_t stDomainIndex = string :: npos ;
	string strDir ;
	string strPage ;
	string strQueryString ;

	CCFDManager :: ParseURL ( strURL.c_str ( ) , strDomain , strHost , strDir , strPage , strQueryString ) ;

	strHostName = strHost ;

	strDomain = m_pTermsInDomain->GetDomainFromHost ( strHost ) ;

	//if ( strDomain == strHost )
	if ( strDomain == strURL ) // Nilesh - Aug 26, 2011 - Handled case of 1004756072 1001271520 in cs_adnet_2009_jul
		strKey = "%" ;
	else
	{
		stDomainIndex = strURL.rfind ( strDomain ) ;

		if ( string :: npos == stDomainIndex )
			bRetVal = false ;
		else
		{
			strKey = strURL ;

			strKey.erase ( stDomainIndex , strDomain.size ( ) ) ;
			strKey.insert ( stDomainIndex , "\t" ) ;
			if ( strKey [ 0 ] == '.' )
				//strKey [ 0 ] = '%' ; //Nilesh - Handled case when dictionary contains entries for a1.interclick.com and .a1.interclick.com and the URL to be matched is a1.interclick.com/Segment.aspx?sid=0c686121-74b2-488c-af2b-07ff755196ec
				strKey.insert ( 0 , 1 , '%' ) ; //Nilesh (Sep 06, 2011) - Reverted back this one to support case of ad.reduxmedia.com/st  Above case also supported

			if ( string :: npos == strURL.find ( "/" ) )
			{
				strKey.insert ( 0 , 1 , '%' ) ; //Nilesh - July 21, 2011 - Added support for handling case of an-imp.bid.ace.advertising.com where dcnsubdomain returns a valid Pattern ID. Per Nilesh's understanding even csCFD should go for substring matches
					strKey += "/%" ;
			}
		}
	}

	return bRetVal ;
}

long CCFDManager :: ApplyDictionary ( string sHost , string sQueryString , int& iWeight )
{
	
	int ( *ptrFun ) ( int ) = toupper ;

	// Convert the input URL to Upper Case 
	transform ( sHost.begin ( ) , sHost.end ( ) , sHost.begin ( ) , ptrFun ) ;

	// Get Domain name 
	string strDomainName = m_pTermsInDomain->GetDomainFromHost ( sHost ) ;
	long lPatternID = 1 ;
	
	map < string , CCFDBase* > :: iterator itrMapSearchResult ;

	// Search the domain name in collection to get root node for tree for that domain 
	if ( m_mapDictionary.end ( ) != ( itrMapSearchResult = m_mapDictionary.find ( strDomainName ) ) )
		lPatternID = itrMapSearchResult->second->Apply ( sHost.c_str() , strDomainName.c_str ( ) , sQueryString.c_str() , iWeight ) ;
	else
	{
		// do a wildcard serch if we dont find a domain in collection.

		for ( map < string , CCFDBase* >::iterator it = m_mapWildcardDictionary.begin(); it != m_mapWildcardDictionary.end(); it++)
		{
			if(DsciWildCardSearch ( it->first.c_str() , strDomainName.c_str ( ) , '%' , '\0' ) )
			{
				lPatternID = it->second->Apply ( sHost.c_str() , strDomainName.c_str ( ) , sQueryString.c_str() , iWeight ) ;
			}
		}
		// m_mapWildcardDictionary
	}

	return lPatternID ;
}

long CCFDManager :: ApplyDictionary ( string sHost , string sDomain , string sQueryString , int& iWeight )
{
	int ( *ptrFun ) ( int ) = toupper ;
	// Convert the input URL to Upper Case 
	transform ( sHost.begin ( ) , sHost.end ( ) , sHost.begin ( ) , ptrFun ) ;
	transform ( sDomain.begin ( ) , sDomain.end ( ) , sDomain.begin ( ) , ptrFun ) ;

	// Get Domain name 
	string strDomainName = m_pTermsInDomain->GetDomainFromHost ( sHost ) ;
	long lPatternID = 1 ;
	
	map < string , CCFDBase* > :: iterator itrMapSearchResult ;

	if(sDomain == string("AOL.COM-PROP") )
	{
		if(sQueryString.size() > 1 && sQueryString.at(0) == '/')
			sQueryString.erase(0,1);

	}

	//if( sDomain == string("SECURESTUDIES.COM-STREAM"))
	//{
	//	transform ( sQueryString.begin ( ) , sQueryString.end ( ) , sQueryString.begin ( ) , ptrFun ) ;
	//	int pos = sQueryString.find("C2=");
	//	if(pos != -1)
	//	{
	//		long pos2 = sQueryString.find("&",pos);
	//		if(pos2 != -1)
	//		{
	//			sDomain = sDomain + sQueryString.substr(pos+3,pos2-(pos+3));
	//			//printf("\n Modified entity name = %s",sDomain.c_str());
	//		}
	//	}

	//}

	// this is domain override case so use original domain name to look in collection
	if ( m_mapDictionary.end ( ) != ( itrMapSearchResult = m_mapDictionary.find ( sDomain ) ) )
		lPatternID = itrMapSearchResult->second->Apply ( sHost.c_str() , strDomainName.c_str() , sQueryString.c_str() , iWeight ) ;
	else
	{
		// do a wildcard serch if we dont find a domain in collection.

		for ( map < string , CCFDBase* >::iterator it = m_mapWildcardDictionary.begin(); it != m_mapWildcardDictionary.end(); it++)
		{
			if(DsciWildCardSearch ( it->first.c_str() , sDomain.c_str ( ) , '%' , '\0' ) )
			{
				lPatternID = it->second->Apply ( sHost.c_str() , strDomainName.c_str ( ) , sQueryString.c_str() , iWeight ) ;
			}
		}
	}

	return lPatternID ;
}

long CCFDManager :: ApplyAdNetDictionary ( string strRequestURL , string strReferURL )
{
	if ( !m_bLoadAdNetDictionary )
		throw CCSCFDException ( "ApplyAdNetDictionary called without loading AdNet dictionary" , -1 ) ;

	//Nilesh - Aug 26, 2011 - Added support for handling cases where yahoo.com and mail.yahoo.com both are present in dictionary and Request / Refer URLs do not contain any slash
	//if ( string :: npos == strRequestURL.find ( '/' ) )
	if ( '/' != strRequestURL [ strRequestURL.size ( ) - 1 ] )
		strRequestURL.append ( "/" ) ;
	if ( '/' != strReferURL [ strReferURL.size ( ) - 1 ] )
		strReferURL.append ( "/" ) ;

	string strRequestDomain ;
	string strReferDomain ;
	string strRequestHost ;
	string strReferHost ;
	string strDir ;
	string strPage ;
	string strQueryString ;
	//map < string , CCFDTree* > :: iterator mapItr ;
	map < string , CCFDBase* > :: iterator mapItr ;
	long lRetVal = 1 ;
	int ( *ptrFun )( int ) = toupper; 

	transform ( strRequestURL.begin ( ) , strRequestURL.end ( ) , strRequestURL.begin ( ) , ptrFun ) ;
	transform ( strReferURL.begin ( ) , strReferURL.end ( ) , strReferURL.begin ( ) , ptrFun ) ;

#ifdef _WIN32
	CCFDManager ::ParseURLA ( strRequestURL.c_str ( ) , strRequestDomain , strRequestHost , strDir , strPage , strQueryString ) ;
	CCFDManager ::ParseURLA ( strReferURL.c_str ( ) , strReferDomain , strReferHost , strDir , strPage , strQueryString ) ;
#else
	CCFDManager ::ParseURL ( strRequestURL.c_str ( ) , strRequestDomain , strRequestHost , strDir , strPage , strQueryString ) ;
	CCFDManager ::ParseURL ( strReferURL.c_str ( ) , strReferDomain , strReferHost , strDir , strPage , strQueryString ) ;
#endif

	strRequestDomain = m_pTermsInDomain->GetDomainFromHost ( strRequestHost ) ;
	strReferDomain = m_pTermsInDomain->GetDomainFromHost ( strReferHost ) ;

	//CCFDTree::VALUE* pResult ;
	size_t stIndexOfRequestDomain ;
	size_t stIndexOfReferDomain ;

	stIndexOfRequestDomain = strRequestURL.find ( strRequestDomain ) ;
	stIndexOfReferDomain = strReferURL.find ( strReferDomain ) ;

	if ( string :: npos == stIndexOfRequestDomain )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		objLogger.LogTrace ( "Failed to find Domain in RequestURL:" , strRequestURL ) ;
		return lRetVal ;
	}
	if ( string :: npos == stIndexOfReferDomain )
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		objLogger.LogTrace ( "Failed to find Domain in ReferURL:" , strReferURL ) ;
		return lRetVal ;
	}
	strRequestURL.erase ( stIndexOfRequestDomain , strRequestDomain.size ( ) ) ;
	strRequestURL.insert ( stIndexOfRequestDomain , "\t" ) ;
	strReferURL.erase ( stIndexOfReferDomain , strReferDomain.size ( ) ) ;
	strReferURL.insert ( stIndexOfReferDomain , "\t" ) ;

	if ( m_mapAdNet.end ( ) != ( mapItr = m_mapAdNet.find ( strRequestDomain + "\t" + strReferDomain ) ) )
	{
		//pResult = mapItr->second->MatchURL ( (char*)(strRequestURL + "\t" + strReferURL).c_str ( ) ) ;

		//if ( pResult )
		//	lRetVal = pResult->pattern_id ;
		lRetVal = mapItr->second->ApplyAdNet ( strRequestURL.c_str ( ) , strReferURL.c_str ( ) ) ;
	}
	//else
	if ( 1 == lRetVal )
	{
		for ( int i = 0 ; i < m_iPercentEntryCount ; i ++ )
		{
			//fprintf ( stderr , "[" ) ;
			//fwrite ( m_arrayCCFDTree [ i ].first , strlen ( m_arrayCCFDTree [ i ].first ) , 1 , stderr ) ;
			//fprintf ( stderr , "]\nvs\n[" ) ;
			//fwrite ( ( strRequestDomain + "\t" + strReferDomain ).c_str ( ) , ( strRequestDomain + "\t" + strReferDomain ).size ( ) , 1 , stderr )  ;
			//fprintf ( stderr , "]" ) ;
			if ( DsciWildCardSearch ( m_arrayCCFDTree [ i ].first , ( strRequestDomain + "\t" + strReferDomain ).c_str ( ) , '%' , '\n' ) )
			{
				//CCFDTree::VALUE* pResult ;

				//if ( ( pResult = m_arrayCCFDTree [ i ].second->MatchURL ( (char*) ( strRequestURL + "\t" + strReferURL ).c_str ( ) ) ) )
				//{
				//	lRetVal = pResult->pattern_id ;
				//	break ;
				//}

				if ( 1 < ( lRetVal = m_arrayCCFDTree [ i ].second->ApplyAdNet ( strRequestURL.c_str ( ) , strReferURL.c_str ( ) ) ) )
					break ;
			}
		}
	}
	//long lRetVal = 1 ;
	//CCFDTree :: VALUE* pVal = NULL ;
	//string strKey ( strRequestURL ) ;

	//strKey += "\t" + strReferURL ;

	//if ( ( pVal = m_pAdNetTree->MatchURL ( (char*)strKey.c_str ( ) ) ) )
	//	lRetVal = pVal->pattern_id ;

	return lRetVal ;
}

string CCFDManager :: GetDomain ( const char* pszHostName )
{
	return "" ;
}

void CCFDManager :: ParseURL ( const char* pszURL , string& strDomain , string& strHost , string& strDir , string& strPage , string& strQueryString )
{
		string url ( pszURL ) ;

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

		strHost = host ;
		strQueryString = qs ;
}

string CCFDManager :: GetMD5Hash ( const char* pszInputString )
{
	return "" ;
}

long long CCFDManager :: GetDomainID ( const char* pszInputString , string& strDomainID )
{
	return -1 ;
}

void CCFDManager :: GetStats ( long& lRowsLoaded , long& lCacheDate , long& lNEDOMRecords ) 
{
	lRowsLoaded = m_lRowsLoaded ;
	lCacheDate = m_lCacheDate ;
	lNEDOMRecords = m_lNEDOMRecords ;

	m_lDictionaryDate = -1 ;
}

#if ( defined(_WIN32) ) || ( !defined(_WIN32) && defined (ENABLE_LINUX_DATABASE_LOAD) )
void CCFDManager :: loadConfiguration ( )
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
	string strDictionaryDate ;

	objLogger.LogINFO ( "Searching \"Dictionary\" for \"Title\": [" , m_strTableName.c_str ( ) , "]" ) ;
	string strGivenDictionaryName ( m_strTableName ) ;
	m_strGivenDictionaryName = m_strTableName ;
#ifdef _WIN32
	try
#endif
	{
		if ( ! m_bLoadAdNetDictionary )
		{
#ifdef _WIN32
				string conn_string =  "Driver={SQL Server};Server=" + m_pTermsInDomain->m_strMMXIDataSource + ";Database=AMW_CF1;Uid=seg_mmxi;Pwd=Segmmxi100";
				m_pTermsInDomain->OpenDBConn_Input(conn_string );
			
				string str_sql;	
				//str_sql = "SELECT LastTerm,	SecondLastTerm, TermCount, LastUpdatedAt FROM dbo.cs_terms_in_domain order by lastterm"; //where is_primary_c2_id = 1
				/*stringstream ssCommand;
					ssCommand << "SELECT Idx, Time_stamp, Dictionary, Stored_proc, Title "
				  << "FROM dbo.cs_dictionary_config "*/
				stringstream ssCommand;
#define USE_DATEPART
#ifdef USE_DATEPART
				ssCommand << "SELECT Idx, "
						<< "DATEPART(yyyy, crdate) AS Time_stamp_YR, "
						<< "DATEPART(m, crdate) AS Time_stamp_MON, "
						<< "DATEPART(d, crdate) AS Time_stamp_DAY, "
						<< "DATEPART(hh, crdate) AS Time_stamp_HR, "
						<< "DATEPART(n, crdate) AS Time_stamp_MIN, "
						<< "DATEPART(s, crdate) AS Time_stamp_SEC, "
						<< "Dictionary, Stored_proc, Title FROM dbo.cs_dictionary_config "
#else
				ssCommand << "SELECT Idx, crdate AS Time_stamp, Dictionary, Stored_proc, Title FROM dbo.cs_dictionary_config "
#endif
						<< "join sysobjects on name = Dictionary "
						 << "WHERE Title = '" << m_strTableName << "'";

				str_sql = ssCommand.str().c_str();

				printf("Executing %s\n", str_sql.c_str() );
	
				string url;	
				_CommandPtr pCommand;
				HRESULT hr=pCommand.CreateInstance(__uuidof(Command));
				if(SUCCEEDED(hr))
				{
					pCommand->ActiveConnection = m_pTermsInDomain->m_dbConn_Input;
				
					pCommand->CommandText=str_sql.c_str();

					pCommand->CommandTimeout = 36000;

					_RecordsetPtr pRecordset;

					hr=pRecordset.CreateInstance(__uuidof(Recordset));
					if( SUCCEEDED(hr))
					{			
						FieldsPtr pFields=pRecordset->Fields;

						hr = pRecordset->Open((IDispatch *) pCommand,
							vtMissing, adOpenForwardOnly, adLockReadOnly, adCmdUnknown);			

						//Nilesh - May 11, 2011 - Added support for blanking this out so that case of invalid dictionary name can be handled
						m_strTableName = "" ;

						while( !pRecordset->EndOfFile )
						{								
							// todo // get the exact logic from dcnSub....
							m_strTableName = m_pTermsInDomain->CheckString( pRecordset, "Dictionary" );							
							//mapMapping.insert(make_pair(name,plateform));	
							{
								struct tm objTm ;
								int iVal = -1 ;

								//fprintf ( stderr , "[%s]\n" , strDictionaryDate.c_str ( ) ) ;

								memset ( &objTm , 0 , sizeof ( struct tm ) ) ;
#if !defined(USE_DATEPART)
							strDictionaryDate = m_pTermsInDomain->CheckString ( pRecordset , "Time_stamp" ) ;
								if 
									(
										/*(iVal = sscanf
										( strDictionaryDate.c_str ( ) , "%i/%i/%i %i:%i:%i" ,
											&(objTm.tm_mday) ,
											&(objTm.tm_mon) ,
											&(objTm.tm_year) ,
											&(objTm.tm_hour) ,
											&(objTm.tm_min) ,
											&(objTm.tm_sec) 
										)) >= 3 */
										(iVal = sscanf
										( strDictionaryDate.c_str ( ) , "%i/%i/%i %i:%i:%i" ,
											&(objTm.tm_mon) ,
											&(objTm.tm_mday) ,
											&(objTm.tm_year) ,
											&(objTm.tm_hour) ,
											&(objTm.tm_min) ,
											&(objTm.tm_sec) 
										)) >= 3 

									)
#endif
								{
#ifdef USE_DATEPART
									objTm.tm_year = atoi ( m_pTermsInDomain->CheckString ( pRecordset , "Time_stamp_YR" ).c_str ( ) ) ;
									objTm.tm_mon = atoi ( m_pTermsInDomain->CheckString ( pRecordset , "Time_stamp_MON" ).c_str ( ) ) ;
									objTm.tm_mday = atoi ( m_pTermsInDomain->CheckString ( pRecordset, "Time_stamp_DAY" ).c_str ( ) ) ;
									objTm.tm_hour = atoi ( m_pTermsInDomain->CheckString ( pRecordset , "Time_stamp_HR" ).c_str ( ) ) ;
									objTm.tm_min = atoi ( m_pTermsInDomain->CheckString ( pRecordset , "Time_stamp_MIN" ).c_str ( ) ) ;
									objTm.tm_sec = atoi ( m_pTermsInDomain->CheckString ( pRecordset , "Time_stamp_SEC" ).c_str ( ) ) ;
#else
									if( strDictionaryDate.find("PM") != -1)
										objTm.tm_hour+=12;
#endif

									struct tm objTmJan012000 = { 0 , 0 , 0 , 1 , 0 , 100 } ;
									time_t objTime ;

									objTm.tm_mon -- ;
									objTm.tm_year -= 1900 ;

									objTime = mktime ( &objTm ) ;

									time_t objTimeJan012000 = mktime ( &objTmJan012000 ) - 86400 ;
									m_lDictionaryDate = objTime - objTimeJan012000 ;
									fprintf ( stderr , "[Dictioany ss2k - %d]\n" , m_lDictionaryDate) ;
								}
#if !defined(USE_DATEPART)
								else
								{
									ostringstream objOssTimestampParsingError ;

									objOssTimestampParsingError << "Unable to Parse dictionary timestamp: [" << strDictionaryDate.c_str ( ) << "] read from database[" << iVal << "]" ;
									throw CCSCFDException ( objOssTimestampParsingError.str ( ) , CS_ERROR_OS_ERROR ) ;
								}
#endif
							}
							pRecordset->MoveNext();
						}
				
					}
				}
#else
		string conn_string =  "Driver=FreeTDS;Server=" + m_pTermsInDomain->m_strMMXIDataSource + ";Database=AMW_CF1;Uid=seg_mmxi;Pwd=Segmmxi100";
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
						fprintf ( stdout , "Returned conection string: [%s]\n" , sqlcharReturnedConnectionString ) ;
						ostringstream objOssTableNameDeterminationQuery ;
						char szTableName [ 128 ] = {0} ;
						SQLLEN sqlintegerIndicator ;

						SQLAllocHandle ( SQL_HANDLE_STMT , hDatabaseConnection , &hStatement ) ;
						objOssTableNameDeterminationQuery << "SELECT dictionary FROM dbo.cs_dictionary_config " ;
						objOssTableNameDeterminationQuery << "WHERE title='" << m_strTableName.c_str ( ) << "'" ;

						fprintf ( stderr , "Executing: [%s]\n" , objOssTableNameDeterminationQuery.str ( ).c_str ( ) ) ;

						SQLAllocHandle ( SQL_HANDLE_STMT , hDatabaseConnection , &hStatement ) ;

						if ( ! SQL_SUCCEEDED ( SQLExecDirect ( hStatement , (SQLCHAR*) objOssTableNameDeterminationQuery.str ( ).c_str ( ) , objOssTableNameDeterminationQuery.str ( ).size ( )  ) ) )
						{
							ostringstream objOssErrorMessage ;

							objOssErrorMessage << "Error: (" << CS_ERROR_LOAD_FAILED << "): Error: " ;
							objOssErrorMessage << "[" << extract_error ( "loadConfiguration" , hStatement , SQL_HANDLE_STMT ) << "]" ;
							objOssErrorMessage << " occurred while loading configuration from database" ;
							throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_LOAD_FAILED ) ;
						}

						while ( SQL_SUCCEEDED ( retVal = SQLFetch ( hStatement ) ) )
						{
								memset ( szTableName , 0 , 128 ) ;

								if ( SQL_SUCCEEDED ( SQLGetData ( hStatement , 1 , SQL_C_CHAR , szTableName , 127 , (SQLLEN*) &sqlintegerIndicator ) ) )
										;
								else
								{
										//TODO - Nilesh add support for handling this failure
								}
						}

						SQLFreeHandle ( SQL_HANDLE_STMT , hStatement ) ;

						m_strTableName = szTableName ;
				}
				else
				{
					ostringstream objOssErrorMessage ;

					objOssErrorMessage << "Error: (" << CS_ERROR_LOAD_FAILED << "): Error: " ;
					objOssErrorMessage << "[" << extract_error ( "loadDictionaryFromDatabase" , hDatabaseConnection , SQL_HANDLE_DBC ) << "]" ;
					objOssErrorMessage << " occurred while loading Dictionary from database" ;
					throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_LOAD_FAILED ) ;
				}

				SQLFreeHandle ( SQL_HANDLE_DBC , hDatabaseConnection ) ;
				SQLFreeHandle ( SQL_HANDLE_ENV , hEnvironment ) ;
#endif
				ostringstream objOssDictionaryLoadMessage ;

				objOssDictionaryLoadMessage << "\"Dictionary\": [" << m_strTableName.c_str ( ) << "]" ;
				objOssDictionaryLoadMessage << "Dictionary timestamp in database: [" << strDictionaryDate.c_str ( ) << "] SS2K: [" << m_lDictionaryDate << "]" ;
				objLogger.LogINFO ( objOssDictionaryLoadMessage.str ( ).c_str ( ) ) ;
				if ( ! m_strTableName.size ( ) )
				{
					ostringstream objOssErrorMessage ;

					objOssErrorMessage << "Error: (" << CS_ERROR_NO_DATA << "): Error: " ;
					objOssErrorMessage << "Unable to retrieve Title for Dictionary name: [" << strGivenDictionaryName.c_str ( ) << "]" ;
					objOssErrorMessage << " occurred while retrieving Dictionary title from database" ;
					throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_NO_DATA ) ;
				}
			}
			else
			{
				string conn_string =  "Driver={SQL Server};Server=" + m_pTermsInDomain->m_strMMXIDataSource + ";Database=AMW_CF1;Uid=seg_mmxi;Pwd=Segmmxi100";
				ostringstream objOssAdNetConfigurationRetrievalQuery ;

				objOssAdNetConfigurationRetrievalQuery << "SELECT " 
						<< "DATEPART(yyyy, crdate) AS Time_stamp_YR, "
						<< "DATEPART(m, crdate) AS Time_stamp_MON, "
						<< "DATEPART(d, crdate) AS Time_stamp_DAY, "
						<< "DATEPART(hh, crdate) AS Time_stamp_HR, "
						<< "DATEPART(n, crdate) AS Time_stamp_MIN, "
						<< "DATEPART(s, crdate) AS Time_stamp_SEC "
					<< "FROM sysobjects WHERE name='" << m_strTableName.c_str ( ) << "'" ;
				printf("Executing %s\n", objOssAdNetConfigurationRetrievalQuery.str ( ).c_str() );

#ifdef _WIN32
				m_pTermsInDomain->OpenDBConn_Input(conn_string );
				_CommandPtr pCommand;
				HRESULT hr=pCommand.CreateInstance(__uuidof(Command));
				if(SUCCEEDED(hr))
				{
					pCommand->ActiveConnection = m_pTermsInDomain->m_dbConn_Input;
				
					pCommand->CommandText=objOssAdNetConfigurationRetrievalQuery.str ( ).c_str() ;

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
							struct tm objTm ;

							objTm.tm_year = atoi ( m_pTermsInDomain->CheckString ( pRecordset , "Time_stamp_YR" ).c_str ( ) ) ;
							objTm.tm_mon = atoi ( m_pTermsInDomain->CheckString ( pRecordset , "Time_stamp_MON" ).c_str ( ) ) ;
							objTm.tm_mday = atoi ( m_pTermsInDomain->CheckString ( pRecordset, "Time_stamp_DAY" ).c_str ( ) ) ;
							objTm.tm_hour = atoi ( m_pTermsInDomain->CheckString ( pRecordset , "Time_stamp_HR" ).c_str ( ) ) ;
							objTm.tm_min = atoi ( m_pTermsInDomain->CheckString ( pRecordset , "Time_stamp_MIN" ).c_str ( ) ) ;
							objTm.tm_sec = atoi ( m_pTermsInDomain->CheckString ( pRecordset , "Time_stamp_SEC" ).c_str ( ) ) ;

							struct tm objTmJan012000 = { 0 , 0 , 0 , 1 , 0 , 100 } ;
							time_t objTime ;

							objTm.tm_mon -- ;
							objTm.tm_year -= 1900 ;

							objTime = mktime ( &objTm ) ;

							time_t objTimeJan012000 = mktime ( &objTmJan012000 ) - 86400 ;
							m_lDictionaryDate = objTime - objTimeJan012000 ;
							fprintf ( stderr , "[Ad Net Dictionary ss2k - %d]\n" , m_lDictionaryDate) ;

							pRecordset->MoveNext ( ) ;
						}
					}
					else
					{
						ostringstream objOssErrorMessage ;

						objOssErrorMessage << "Failed to create instance of Recordset class. HRESULT: [" << hex << hr << "]" ;
						throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
					}
				}
				else
				{
					ostringstream objOssErrorMessage ;

					objOssErrorMessage << "Failed to create instance of Command class. HRESULT: [" << hex << "]" ;
					throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_OS_ERROR ) ;
				}
#else
				fprintf ( stderr , "Ad Net configuration loading not implemented on Linux\n" ) ;
#if 0
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
						fprintf ( stdout , "Returned conection string: [%s]\n" , sqlcharReturnedConnectionString ) ;
						ostringstream objOssTableNameDeterminationQuery ;
						char szTableName [ 128 ] = {0} ;
						SQLLEN sqlintegerIndicator ;

						SQLAllocHandle ( SQL_HANDLE_STMT , hDatabaseConnection , &hStatement ) ;

						SQLAllocHandle ( SQL_HANDLE_STMT , hDatabaseConnection , &hStatement ) ;

						if ( ! SQL_SUCCEEDED ( SQLExecDirect ( hStatement , (SQLCHAR*) objOssAdNetConfigurationRetrievalQuery.str ( ).c_str ( ) , objOssAdNetConfigurationRetrievalQuery.str ( ).size ( )  ) ) )
						{
							ostringstream objOssErrorMessage ;

							objOssErrorMessage << "Error: (" << CS_ERROR_LOAD_FAILED << "): Error: " ;
							objOssErrorMessage << "[" << extract_error ( "loadConfiguration" , hStatement , SQL_HANDLE_STMT ) << "]" ;
							objOssErrorMessage << " occurred while loading configuration from database" ;
							throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_LOAD_FAILED ) ;
						}

						while ( SQL_SUCCEEDED ( retVal = SQLFetch ( hStatement ) ) )
						{
								memset ( szTableName , 0 , 128 ) ;

								if ( SQL_SUCCEEDED ( SQLGetData ( hStatement , 1 , SQL_C_CHAR , szTableName , 127 , (SQLLEN*) &sqlintegerIndicator ) ) )
										;
								else
								{
										//TODO - Nilesh add support for handling this failure
								}
						}

						SQLFreeHandle ( SQL_HANDLE_STMT , hStatement ) ;

						m_strTableName = szTableName ;
				}
				else
				{
					ostringstream objOssErrorMessage ;

					objOssErrorMessage << "Error: (" << CS_ERROR_LOAD_FAILED << "): Error: " ;
					objOssErrorMessage << "[" << extract_error ( "loadDictionaryFromDatabase" , hDatabaseConnection , SQL_HANDLE_DBC ) << "]" ;
					objOssErrorMessage << " occurred while loading Dictionary from database" ;
					throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_LOAD_FAILED ) ;
				}

				SQLFreeHandle ( SQL_HANDLE_DBC , hDatabaseConnection ) ;
				SQLFreeHandle ( SQL_HANDLE_ENV , hEnvironment ) ;
#endif
#endif

				if ( -1 == m_lDictionaryDate )
				{
					ostringstream objOssAdNetDictionaryTimestampRetrievalError ;

					objOssAdNetDictionaryTimestampRetrievalError << "Failed to determine timestamp for Ad Net dictionary: " ;
					objOssAdNetDictionaryTimestampRetrievalError << "[" << m_strTableName.c_str ( ) << "]" ;

					throw CCSCFDException ( objOssAdNetDictionaryTimestampRetrievalError.str ( ).c_str ( ) , CS_ERROR_NO_DATA ) ;
				}
			}
		}
#ifdef _WIN32
		catch(_com_error& error)
		{		
			ostringstream objOssErrorMessage ;

			objOssErrorMessage << "Error: (" << CS_ERROR_LOAD_FAILED << "): COM Error: " ;
			objOssErrorMessage << "[" << error.Error ( ) << "]=[" << error.ErrorMessage ( ) << "]" ;
			objOssErrorMessage << " occurred while loading Terms In Domain from database" ;
			throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_LOAD_FAILED ) ;
		}
		m_pTermsInDomain->m_dbConn_Input->Close();
#endif
}
#endif

bool CCFDManager::IsNumericIPAddress(string sHost)
{
	long pos = sHost.find("-stream");
	if(pos != -1)
	{
		sHost.erase(pos,7);
	}
	else
	{
		pos = sHost.find("-rstream");
		if(pos != -1)
		{
			sHost.erase(pos,8);
		}
	}

	long lHostLength = sHost.size();

	for (int Index = 0; Index < lHostLength; Index++)
	{
		int lValue = (int)(sHost[Index]);

		if ((lValue != 46) &&                   // not "."
			((lValue < 48) || (lValue > 57)))   // not Between 0 and 9
		{
			return false;
		}
	}

	return true;
}

#if ( defined(_WIN32) ) || ( !defined(_WIN32) && defined (ENABLE_LINUX_DATABASE_LOAD) )
void CCFDManager :: loadDictionaryFromDatabase ( )
{
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;

#ifdef _WIN32
	try
#endif
	{
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
#ifdef _WIN32
		string strConnectionString =  "Driver={SQL Server};Server=" + m_pTermsInDomain->m_strMMXIDataSource + ";Database=AMW_CF1;Uid=seg_mmxi;Pwd=Segmmxi100";
#else
		string strConnectionString =  "Driver=FreeTDS;Server=" + m_pTermsInDomain->m_strMMXIDataSource + ";Database=AMW_CF1;Uid=seg_mmxi;Pwd=Segmmxi100";
#endif
		ostringstream objOssDictionaryLoadingQuery ;

#ifdef _WIN32
		m_pTermsInDomain->OpenDBConn_Input ( strConnectionString ) ;
#endif
		if ( ! m_bLoadAdNetDictionary )
		{
			objOssDictionaryLoadingQuery << "SELECT DDEntityID, Prefix, EntityName, IPSuffixMin, IPSuffixMax, PageMask," 
											<< "SubchannelPrecedence, Precedence, DatasetType, PropertyName, apps_mask" 
											<< " FROM " << m_strTableName.c_str ( ) << " WITH (nolock) "
											//<< "WHERE (DatasetType='W' OR DatasetType='A') " //and Prefix = '' or  ( IPSuffixMin > 0 or IPSuffixMax > 0 )" /*<< " AND EntityName='GOOGLE.COM' " */
											<< "WHERE (DatasetType='W' OR DatasetType='A') " /*<< " AND EntityName='GOOGLE.COM' " */
											//<< "WHERE (DatasetType='W' OR DatasetType='A') " << " AND EntityName IN ('GOOGLE.COM','MSN.COM') "
											<< "ORDER BY SubchannelPrecedence DESC, Precedence DESC, DDEntityID" ;
		}
		else
		{
			objOssDictionaryLoadingQuery << "SELECT pattern_id, request_url, refer_url FROM "  << m_strTableName.c_str ( ) 
				<< " ORDER BY len(request_url) DESC, len(refer_url) DESC" ;
		}


		fprintf ( stderr , "Now running dictionary loading query: [%s]\n" , objOssDictionaryLoadingQuery.str ( ).c_str ( ) ) ;
		objLogger.LogINFO ( "Dictionary loading query: [" , objOssDictionaryLoadingQuery.str ( ).c_str ( ) , "]" ) ;

		string strDictionaryCacheFileName ( m_strCachePath ) ;
		gzFile foutDictionary = NULL ;
		long lPrecedence = 2147483647 ;

#ifdef _WIN32
		strDictionaryCacheFileName += "\\" ;
#else
		strDictionaryCacheFileName += "/" ;
#endif
		strDictionaryCacheFileName += m_strTableName ;
		strDictionaryCacheFileName += "_" + strFileSuffix ;
		strDictionaryCacheFileName += "_cache.bcp.gz" ;

		Utils :: CheckOutFilePrefixAndCreatePath ( strDictionaryCacheFileName ) ;
		Utils :: CheckOutFilePrefixAndCreatePath ( m_strPermanentCacheFileName ) ;

		if ( ! ( foutDictionary = gzopen ( strDictionaryCacheFileName.c_str ( ) , "wb" ) ) )
		{
			ostringstream objOssTemporaryCacheFileOpenError ;

			objOssTemporaryCacheFileOpenError << "Error: (" << CS_ERROR_FILE_CREATION_FAILED << "): " ;
			objOssTemporaryCacheFileOpenError << "Unable to open temporary cache file: " ;
			objOssTemporaryCacheFileOpenError << "[" << strDictionaryCacheFileName.c_str ( ) << "]" ;
			objOssTemporaryCacheFileOpenError << " for compressed writing" ;
			objLogger.LogERROR ( "Failed to cache dictionary from database." , objOssTemporaryCacheFileOpenError.str ( ).c_str ( ) ) ;
			throw CCSCFDException ( objOssTemporaryCacheFileOpenError.str ( ) , CS_ERROR_FILE_CREATION_FAILED ) ;
		}

		m_strTemporaryCacheFileName = strDictionaryCacheFileName ;
		objLogger.LogINFO ( "Caching dictionary from database into: [" , strDictionaryCacheFileName.c_str ( ) , "]" ) ;
#ifdef _WIN32
		_CommandPtr pCommand ;
		if ( SUCCEEDED ( pCommand.CreateInstance ( __uuidof ( Command ) ) ) )
		{
			pCommand->ActiveConnection = m_pTermsInDomain->m_dbConn_Input ;
			pCommand->CommandText = objOssDictionaryLoadingQuery.str ( ).c_str ( ) ;
			pCommand->CommandTimeout = 36000 ;

			_RecordsetPtr pRecordSet ;
			if ( SUCCEEDED ( pRecordSet.CreateInstance ( __uuidof ( Recordset ) ) ) )
			{
				FieldsPtr pFields = pRecordSet->Fields ;

				if ( SUCCEEDED ( pRecordSet->Open ( (IDispatch*) pCommand , vtMissing , adOpenForwardOnly , adLockReadOnly , adCmdUnknown ) ) )
				{
					//map < string , CCFDBase* > :: iterator itrMapSearchResult = m_mapDictionary.end ( ) ;
					CCFDBase* pCFDBase = NULL ;
					int ( *ptrFun )( int ) = toupper; 
					m_lRowsLoaded = 0 ;
					m_lNEDOMRecords = 0 ;

					while ( !pRecordSet->EndOfFile )
					{
						string strOutputRow ;
						m_lRowsLoaded ++ ;
						if ( ! m_bLoadAdNetDictionary )
						{
							bool bIsNEDOM = false ;
							//Nilesh - TODO - Discuss cs_digapp / DatasetType = A with Santosh and do needful modifications
							string strEntityID = m_pTermsInDomain->CheckString ( pRecordSet , "DDEntityID" ) ;
							string strPrefix = m_pTermsInDomain->CheckString ( pRecordSet , "Prefix" ) ;
							string strEntityName = m_pTermsInDomain->CheckString ( pRecordSet , "EntityName" ) ;
							string strIPSuffixMin = m_pTermsInDomain->CheckString ( pRecordSet , "IPSuffixMin" ) ;
							string strIPSuffixMax = m_pTermsInDomain->CheckString ( pRecordSet , "IPSuffixMax" ) ;
							string strPageMask = m_pTermsInDomain->CheckString ( pRecordSet , "PageMask" ) ;
							string strSubChannelPrecedence = m_pTermsInDomain->CheckString ( pRecordSet , "SubchannelPrecedence" ) ;
							string strPrecedence = m_pTermsInDomain->CheckString ( pRecordSet , "Precedence" ) ;
							string strDatasetType = m_pTermsInDomain->CheckString ( pRecordSet , "DatasetType" ) ;
							string strPropertyName = m_pTermsInDomain->CheckString ( pRecordSet , "PropertyName" ) ;
							string strAppsMask = m_pTermsInDomain->CheckString ( pRecordSet , "apps_mask" ) ;

							//transform( strEntityID.begin(), strEntityID.end(), strEntityID.begin(), ptrFun );
							transform( strEntityName.begin(), strEntityName.end(), strEntityName.begin(), ptrFun );

							strOutputRow = strEntityID ;
							strOutputRow += "\t" + strPrefix ;
							strOutputRow += "\t" + strEntityName ;
							strOutputRow += "\t" + strIPSuffixMin ;
							strOutputRow += "\t" + strIPSuffixMax ;
							strOutputRow += "\t" + strPageMask ;
							strOutputRow += "\t" + strSubChannelPrecedence ;
							strOutputRow += "\t" + strPrecedence ;
							strOutputRow += "\t" + strDatasetType ;
							strOutputRow += "\t" + strPropertyName ;
							strOutputRow += "\t" + strAppsMask + "\n" ;
						}
						else
						{
							ostringstream objOssOutputRow ;

							objOssOutputRow << m_pTermsInDomain->CheckString ( pRecordSet , "pattern_id" ) ;
							objOssOutputRow << "\t" << m_pTermsInDomain->CheckString ( pRecordSet , "request_url" ) ;
							objOssOutputRow << "\t" << m_pTermsInDomain->CheckString ( pRecordSet , "refer_url" ) ;
							objOssOutputRow << "\t" << lPrecedence -- << "\n" ;

							transform( strOutputRow.begin(), strOutputRow.end(), strOutputRow.begin(), ptrFun );

							strOutputRow = objOssOutputRow.str ( ) ;
						}

						gzwrite ( foutDictionary , strOutputRow.c_str ( ) , strOutputRow.size ( ) ) ;

						if ( 0 == ( m_lRowsLoaded % 1000000 ) )
							fprintf ( stdout , "Rows cached: %i\n" , m_lRowsLoaded ) ;

						pRecordSet->MoveNext ( ) ;
					}
					fprintf ( stdout , "Rows cached: %i\n" , m_lRowsLoaded ) ;
					objLogger.LogINFO ( "Cached: [" , m_lRowsLoaded , "] dictionary rows from database" ) ;
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
                        (SQLCHAR*) strConnectionString.c_str ( ) ,
                        SQL_NTS ,
                        sqlcharReturnedConnectionString ,
                        sizeof ( sqlcharReturnedConnectionString ) ,
                        &sqlsmallintReturnedConnectionStringLength ,
                        SQL_DRIVER_COMPLETE
                ) ;
        if ( SQL_SUCCEEDED ( retVal ) )
        {
	        fprintf ( stdout , "Returned connection string: [%s]\n" , sqlcharReturnedConnectionString ) ;
                //ostringstream objOssDictionaryLoadingQuery ;
                SQLAllocHandle ( SQL_HANDLE_STMT , hDatabaseConnection , &hStatement ) ;
#if 0
				if ( m_bLoadAdNetDictionary )
				{
					objOssDictionaryLoadingQuery << "SELECT DDEntityID, Prefix, EntityName, IPSuffixMin, IPSuffixMax, PageMask,"
																					<< "SubchannelPrecedence, Precedence, DatasetType, PropertyName, apps_mask"
																					<< " FROM " << m_strTableName.c_str ( ) << " WITH (nolock) "
																					//<< "WHERE (DatasetType='W' OR DatasetType='A') " //and Prefix = '' or  ( IPSuffixMin > 0 or IPSuffixMax > 0 )" /*<< " AND EntityName='GOOGLE.COM' " */
																					<< "WHERE (DatasetType='W' OR DatasetType='A') " /*<< " AND EntityName='GOOGLE.COM' " */
																					<< "ORDER BY SubchannelPrecedence DESC, Precedence DESC, DDEntityID" ;
				}
				else
				{
					objOssDictionaryLoadingQuery << "SELECT pattern_id, request_url, refer_url FROM "  << m_strTableName.c_str ( ) 
						<< " ORDER BY len(request_url) DESC, len(refer_url) DESC" ;
				}
#endif
				if ( ! SQL_SUCCEEDED ( SQLExecDirect ( hStatement , (SQLCHAR*) objOssDictionaryLoadingQuery.str ( ).c_str ( ) , objOssDictionaryLoadingQuery.str ( ).size ( ) ) ) )
                {
					ostringstream objOssErrorMessage ;

					objOssErrorMessage << "Error: (" << CS_ERROR_LOAD_FAILED << "): Error: " ;
					objOssErrorMessage << "[" << extract_error ( "loadDictionaryFromDatabase" , hStatement , SQL_HANDLE_STMT ) << "]" ;
					objOssErrorMessage << " occurred while loading Dictionary from database" ;
					throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_LOAD_FAILED ) ;
                }

                int iPatternID = -1 ;
                char szPrefix [ 33 ] = {0} ;
                char szEntityName [ 65 ] = {0} ;
                unsigned int uiIPSuffixMin = 0 ;
                unsigned int uiIPSuffixMax = -1 ;
                char szPageMask [ 256 ] = {0} ;
                unsigned int uiSubchannelPrecedence = 0 ;
                unsigned int uiPrecedence = 0 ;
                char szDatasetType [ 256 ] = {0} ;
                char szPropertyName [ 256 ] = {0} ;
                int iAppsMask = -1 ;
                SQLLEN indicator [ 11 ] = {0} ;

				int iAdNetPatternID = -1 ;
				char szRequestURL [ 257 ] = {0} ;
				char szReferURL [ 257 ] = {0} ;

				if ( ! m_bLoadAdNetDictionary )
				{
					SQLBindCol ( hStatement , 1 , SQL_C_SLONG , &iPatternID , sizeof ( iPatternID ) , &indicator [ 0 ]  ) ;
					SQLBindCol ( hStatement , 2 , SQL_C_CHAR , szPrefix , 32 , &indicator [ 1 ]  ) ;
					SQLBindCol ( hStatement , 3 , SQL_C_CHAR , szEntityName , 64 , &indicator [ 2 ]  ) ;
					SQLBindCol ( hStatement , 4 , SQL_C_ULONG , &uiIPSuffixMin , sizeof ( uiIPSuffixMin ) , &indicator [ 3 ]  ) ;
					SQLBindCol ( hStatement , 5 , SQL_C_ULONG , &uiIPSuffixMax , sizeof ( uiIPSuffixMax ) , &indicator [ 4 ]  ) ;
					SQLBindCol ( hStatement , 6 , SQL_C_CHAR , szPageMask , 255 , &indicator [ 5 ]  ) ;
					SQLBindCol ( hStatement , 7 , SQL_C_ULONG , &uiSubchannelPrecedence , sizeof ( uiSubchannelPrecedence ) , &indicator [ 6 ]  ) ;
					SQLBindCol ( hStatement , 8 , SQL_C_ULONG , &uiPrecedence , sizeof ( uiPrecedence ) , &indicator [ 7 ]  ) ;
					SQLBindCol ( hStatement , 9 , SQL_C_CHAR , szDatasetType , 255 , &indicator [ 8 ]  ) ;
					SQLBindCol ( hStatement , 10 , SQL_C_CHAR , szPropertyName , 255 , &indicator [ 9 ]  ) ;
					SQLBindCol ( hStatement , 11 , SQL_C_SLONG , &iAppsMask , sizeof ( iAppsMask ) , &indicator [ 10 ]  ) ;
				}
				else
				{
					SQLBindCol ( hStatement , 1 , SQL_C_SLONG , &iAdNetPatternID , sizeof ( iAdNetPatternID ) , &indicator [ 0 ]  ) ;
					SQLBindCol ( hStatement , 2 , SQL_C_CHAR , szRequestURL , 256 , &indicator [ 1 ]  ) ;
					SQLBindCol ( hStatement , 3 , SQL_C_CHAR , szReferURL , 256 , &indicator [ 2 ]  ) ;
				}

                while ( SQL_SUCCEEDED ( retVal = SQLFetch ( hStatement ) ) )
                {
					ostringstream objOssCacheRecord ;

					if ( ! m_bLoadAdNetDictionary )
					{
						objOssCacheRecord << iPatternID << "\t" ;
						objOssCacheRecord << szPrefix << "\t" ;
						objOssCacheRecord << szEntityName << "\t" ;
						objOssCacheRecord << uiIPSuffixMin << "\t" ;
						objOssCacheRecord << uiIPSuffixMax << "\t" ;
						objOssCacheRecord << szPageMask << "\t" ;
						objOssCacheRecord << uiSubchannelPrecedence << "\t" ;
						objOssCacheRecord << uiPrecedence << "\t" ;
						objOssCacheRecord << szDatasetType << "\t" ;
						objOssCacheRecord << szPropertyName << "\t" ;
						objOssCacheRecord << iAppsMask << "\n" ;

						iPatternID = iAppsMask = -1 ;
						uiIPSuffixMin = uiIPSuffixMax = uiSubchannelPrecedence = uiPrecedence = 0 ;
						memset ( szPrefix , 0 , 33 ) ;
						memset ( szEntityName , 0 , 65 ) ;
						memset ( szPageMask , 0 , 256 ) ;
						memset ( szDatasetType , 0 , 256 ) ;
						memset ( szPropertyName , 0 , 256 ) ;
					}
					else
					{
						objOssCacheRecord << iAdNetPatternID << "\t" ;
						objOssCacheRecord << szRequestURL << "\t" ;
						objOssCacheRecord << szReferURL << "\n" ;

						iAdNetPatternID = -1 ;
						memset ( szRequestURL , 0 , 257 ) ;
						memset ( szReferURL , 0 , 257 ) ;
					}

					gzwrite ( foutDictionary , objOssCacheRecord.str ( ).c_str ( ) , objOssCacheRecord.str ( ).size ( ) ) ;

					if ( 0 == ( m_lRowsLoaded % 1000000 ) )
						fprintf ( stdout , "Rows cached: %i\n" , m_lRowsLoaded ) ;
                }

				fprintf ( stdout , "Rows cached: %i\n" , m_lRowsLoaded ) ;
				objLogger.LogINFO ( "Cached: [" , m_lRowsLoaded , "] dictionary rows from database" ) ;
                SQLFreeHandle ( SQL_HANDLE_STMT , hDatabaseConnection ) ;
        }
        else
        {
			ostringstream objOssErrorMessage ;

			objOssErrorMessage << "Error: (" << CS_ERROR_LOAD_FAILED << "): Error: " ;
			objOssErrorMessage << "[" << extract_error ( "loadDictionaryFromDatabase" , hDatabaseConnection , SQL_HANDLE_DBC ).c_str ( ) << "]" ;
			objOssErrorMessage << " occurred while loading Dictionary from database" ;
			throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_LOAD_FAILED ) ;
        }

        SQLDisconnect ( hDatabaseConnection ) ;
        SQLFreeHandle ( SQL_HANDLE_DBC , hDatabaseConnection ) ;
        SQLFreeHandle ( SQL_HANDLE_ENV , hEnvironment ) ;
        SQLFreeHandle ( SQL_HANDLE_STMT , hStatement ) ;
#endif
		gzclose ( foutDictionary ) ;
	}
#ifdef _WIN32
	catch(_com_error& error)
	{		
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Error: (" << CS_ERROR_LOAD_FAILED << "): COM Error: " ;
		objOssErrorMessage << "[" << error.Error ( ) << "]=[" << error.ErrorMessage ( ) << "]" ;
		objOssErrorMessage << " occurred while loading Dictionary from database" ;
		objLogger.LogERROR ( "Dictionary load from database failed." , objOssErrorMessage.str ( ).c_str ( ) ) ;
		throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_LOAD_FAILED ) ;
	}
#endif
}
#endif

void CCFDManager :: DisableNEDOM ( )
{
	m_bNEDOM = false ;
}

void CCFDManager :: SetAppsMask ( int iAppsMask )
{
	m_iAppsMask = iAppsMask ;
}

bool CCFDManager :: LoadFilterHostList ( const char* pszFilterHostList )
{
	bool bRetVal = false ;
	char* pIndexOfComma = NULL ;
	char* szFilterHostList = strdup ( pszFilterHostList ) ;
	char* pHostStringIterator = szFilterHostList ;

	while ( NULL != ( pIndexOfComma = strchr ( pHostStringIterator , ',' ) ) )
	{
		*pIndexOfComma = '\0' ;
		m_vecHostList.push_back ( strdup ( pHostStringIterator ) ) ;
		string strHostName ( pHostStringIterator ) ;
		m_vecKeepDomainList.push_back ( m_pTermsInDomain->GetDomainFromHost ( strHostName ) ) ;
		pHostStringIterator = pIndexOfComma + 1 ;
	}
	m_vecHostList.push_back ( strdup ( pHostStringIterator ) ) ;
	string strHostName ( pHostStringIterator ) ;
	m_vecKeepDomainList.push_back ( m_pTermsInDomain->GetDomainFromHost ( strHostName ) ) ;
	bRetVal = true ;

	free ( szFilterHostList ) ;
	return bRetVal ;
}

bool CCFDManager :: LoadFilterHostFile ( const char* pszFilterHostFile )
{
	bool bRetVal = false ;
	ColExtractor clx ;

	if ( clx.ExtractFrom ( pszFilterHostFile ) )
	{
		COLUMNS cols ;
		while ( clx.Extract ( cols ) )
		{
			string strHostName ( cols [ 0 ] ) ;
			m_vecHostList.push_back ( strdup ( strHostName.c_str ( ) ) ) ;
			m_vecKeepDomainList.push_back ( m_pTermsInDomain->GetDomainFromHost ( strHostName ) ) ;
			bRetVal = true ;
		}
	}
	else
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Error: (" << CS_ERROR_FILE_NOT_FOUND << "): " ;
		objOssErrorMessage << "Unable to open Filter host file: " ;
		objOssErrorMessage << "[" << pszFilterHostFile << "]" ;
		objOssErrorMessage << " for reading" ;
		throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_FILE_NOT_FOUND ) ;
	}

	return bRetVal ;
}

bool CCFDManager :: loadDictionaryFromCache ( )
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
	bool bRetVal = false ;

	objLogger.LogInfo ( "Loading Dictionary from cache file: [" , m_strPermanentCacheFileName.c_str ( ) , "]" ) ;

	//This *shouldn't* be done here
	//Utils :: CheckOutFilePrefixAndCreatePath ( strDictionaryCacheFileName ) ;
	

#ifdef _WIN32
	if ( 0 != _access ( m_strPermanentCacheFileName.c_str ( ) , 04 ) )
	{
		objLogger.LogERROR ( "Failed. Unable to _access it: [" , strerror ( errno ) , "]" ) ;
		return false ; //Since Permanent cache doesn't exist, it should be loaded from database first
	}
#else
	if ( 0 != access ( m_strPermanentCacheFileName.c_str ( ) , 04 ) )
	{
		objLogger.LogERROR ( "Failed. Unable to access it: [" , strerror ( errno ) , "]" ) ;
		return false ; //Since Permanent cache doesn't exist, it should be loaded from database first
	}
#endif
	
	struct stat objCacheStat ;
	memset ( &objCacheStat , 0 , sizeof ( struct stat ) ) ;
	if ( 0 != stat ( m_strPermanentCacheFileName.c_str ( ) , &objCacheStat ) )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
		objOssErrorMessage << "Error occurred: " ;
		objOssErrorMessage << "[" << strerror ( errno ) << "]" ;
		objOssErrorMessage << " while stat'ing dictionary cache file: " ;
		objOssErrorMessage << "[" << m_strPermanentCacheFileName.c_str ( ) << "]" ;

		objLogger.LogERROR ( "Failed. " , objOssErrorMessage.str ( ).c_str ( ) ) ;

		throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
	}
	
	{
		struct tm objTmJan012000 = { 0 , 0 , 0 , 1 , 0 , 100 } ;
		time_t objTimeJan012000 = mktime ( &objTmJan012000 ) - 86400 ;
		m_lCacheDate = objCacheStat.st_mtime - objTimeJan012000 ;
	}
	
#if !defined(_WIN32) && !defined(ENABLE_LINUX_DATABASE_LOAD)
	m_lDictionaryDate = m_lCacheDate ; //Handle case when database loading is disabled in Linux
#ifdef _WIN32
	objLogger.LogINFO ( "Assigning Cache file timestamp to Dictionary date read from database because Linux Database loading is disabled" ) ;
#endif
#endif

	ostringstream objOssTimestampCheckMessage ;
	objOssTimestampCheckMessage << "Dictionary cache file timestamp: [" << m_lCacheDate << "]" ;
	if ( ! m_bDictionaryNameFlatFile )
	{
		if ( m_lCacheDate < m_lDictionaryDate )
		{
			objOssTimestampCheckMessage << " less than dictionary timestamp in database: [" << m_lDictionaryDate << "]" ;
			objOssTimestampCheckMessage << "\nReloading Dictionary from database\n" ;

			printf("\n%s",objOssTimestampCheckMessage.str().c_str());
			
#ifdef _WIN32
			HANDLE hReaderMutex ;
			HANDLE hWriterMutex ;

			Utils :: CheckBarrier ( "/csCFD/CacheSynchronizationMutex-ReadOnly" , hReaderMutex ) ;
			Utils :: CheckBarrier ( "/csCFD/CacheSynchronizationMutex-WriteOnly" , hWriterMutex ) ;
#else
#ifdef ENABLE_LINUX_DATABASE_LOAD
			
			HANDLE hReaderMutex ;
			HANDLE hWriterMutex ;
			Utils :: CheckBarrier ( "/tmp/csCFD-CacheSynchronizationMutex-ReadOnly" , hReaderMutex ) ;
			Utils :: CheckBarrier ( "/tmp/csCFD-CacheSynchronizationMutex-WriteOnly" , hWriterMutex ) ;
#endif 
#endif

#if ( defined(_WIN32) ) || ( !defined(_WIN32) && defined (ENABLE_LINUX_DATABASE_LOAD) )
			CCFDManager :: loadDictionaryFromDatabase ( ) ;
			unlink ( m_strPermanentCacheFileName.c_str ( ) ) ;

			Utils :: MergeFile ( m_strTemporaryCacheFileName.c_str ( ) , m_strPermanentCacheFileName.c_str ( ) ) ;

			Utils :: UnCheckBarrier ( hWriterMutex ) ;
			Utils :: UnCheckBarrier ( hReaderMutex ) ;

			unlink ( m_strTemporaryCacheFileName.c_str ( ) ) ;
#endif
		}
		else
		{
			objOssTimestampCheckMessage << " greater than or equal to dictionary timestamp in database: [" << m_lDictionaryDate << "]" ;
			objOssTimestampCheckMessage << "\nNot reloading Dictionary from database\n" ;
			printf("\n%s",objOssTimestampCheckMessage.str().c_str());
		}
		objLogger.LogINFO ( objOssTimestampCheckMessage.str ( ).c_str ( ) ) ;
	}
#ifdef _WIN32
	else
		objLogger.LogINFO ( "Skipping dictionary timestamp check because dictionary is being loaded from a flat file" ) ;
#endif

#ifdef _WIN32
	HANDLE hMutex ;
	Utils :: CheckBarrier ( "/csCFD/CacheSynchronizationMutex-ReadOnly" , hMutex ) ;
#else
#ifdef ENABLE_LINUX_DATABASE_LOAD
	HANDLE hMutex ;
	Utils :: CheckBarrier ( "/tmp/csCFD-CacheSynchronizationMutex-ReadOnly" , hMutex ) ;
#endif 
#endif
#if !defined(__WIN32) && defined(ENABLE_LINUX_DATABASE_LOAD)
		struct flock objFlock ;
		int fd = -1 ;
		objFlock.l_type = F_UNLCK ;
		objFlock.l_whence = SEEK_SET ;
		objFlock.l_start = 0 ;
		objFlock.l_len = 0 ;
		if ( 0 > fcntl ( fd , F_SETLK , &objFlock ) )
		{
			ostringstream objOssErrorMessage ;

			objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
			objOssErrorMessage << "Some unknown error: " ;
			objOssErrorMessage << "[" << strerror ( errno ) << "]" ;
			objOssErrorMessage << " while requesting unlocking: " ;
			objOssErrorMessage << "[" << m_strPermanentCacheFileName.c_str ( ) << "]" ;

			objLogger.LogERROR ( "Failed." , objOssErrorMessage.str ( ).c_str ( ) ) ;
			throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
		}
		close ( fd ) ;
#endif

	map < const char* , int , SConstCharComparator > mapDomain_EntryCount ;
	map < const char* , int , SConstCharComparator > :: iterator itrDomain_EntryCount = mapDomain_EntryCount.end ( ) ;

	//if ( ! m_bLoadAdNetDictionary )
	{
		//Analyze complete dictionary to determine count of records per domain
		if ( ( TIME_OPTIMIZED_MODE != m_iMode ) && ( SPACE_OPTIMIZED_MODE != m_iMode ) )
		{
			objLogger.LogINFO ( "Counting number of records per domain" ) ;
			ColExtractor clxAnalyzer ;
			if ( clxAnalyzer.ExtractFrom ( m_strPermanentCacheFileName ) )
			{
				COLUMNS colsDictionary ;
				int iEntityNameIndex			= 2 ;
				int iRequestURLIndex			= 1 ;
				int iReferURLIndex				= 2 ;
				map < const char* , int , SConstCharComparator > :: iterator mapItrSearchResult = mapDomain_EntryCount.end ( ) ;

				if ( ! m_bLoadAdNetDictionary )
				{
					while ( clxAnalyzer.Extract ( colsDictionary ) )
					{
						string strEntityName = colsDictionary [ iEntityNameIndex ] ;
						for ( unsigned int i = 0 ; i < strEntityName.size ( ) ; i ++ )
							strEntityName [ i ] = toupper ( strEntityName [ i ] ) ;

						if ( mapDomain_EntryCount.end ( ) == ( mapItrSearchResult = mapDomain_EntryCount.find ( strEntityName.c_str ( ) ) ) )
							mapDomain_EntryCount.insert ( make_pair ( strdup ( strEntityName.c_str ( ) ) , 1 ) ) ;
						else
							mapItrSearchResult->second ++ ;
					}
				}
				else
				{
					string strDir , strPage , strQueryString ;

					while ( clxAnalyzer.Extract ( colsDictionary ) )
					{
						string strRequestURL = colsDictionary [ iRequestURLIndex ] ;
						string strReferURL = colsDictionary [ iReferURLIndex ] ;
						string strRequestDomain ;
						string strRequestHost ;
						string strReferDomain ;
						string strReferHost ;

						CCFDManager :: ParseURL ( strRequestURL.c_str ( ) , strRequestDomain , strRequestHost , strDir , strPage , strQueryString ) ;
						strRequestDomain = m_pTermsInDomain->GetDomainFromHost ( strRequestHost ) ;
						CCFDManager :: ParseURL ( strReferURL.c_str ( ) , strReferDomain , strReferHost , strDir , strPage , strQueryString ) ;
						strReferDomain = m_pTermsInDomain->GetDomainFromHost ( strReferHost ) ;

						string strKey ( strRequestDomain ) ;
						strKey += "\t" ;
						strKey += strReferDomain ;

						for (unsigned int i = 0 ; i < strKey.size ( ) ; i ++ )
							strKey [ i ] = toupper ( strKey [ i ] ) ;

						if ( mapDomain_EntryCount.end ( ) == ( mapItrSearchResult = mapDomain_EntryCount.find ( strKey.c_str ( ) ) ) )
							mapDomain_EntryCount.insert ( make_pair ( strdup ( strKey.c_str ( ) ) , 1 ) ) ;
						else
							mapItrSearchResult->second ++ ;
					}
				}
			}
			objLogger.LogINFO ( "Done" ) ;
			ostringstream objOssDomainAnalysisFileName ;
			objOssDomainAnalysisFileName << m_strCachePath << "DomainAnalysis_" << hex 
#ifdef _WIN32
				<< GetCurrentProcessId ( )
#else
				<< getpid ( ) 
#endif
				<< ".txt" ;
			FILE* fout = fopen ( objOssDomainAnalysisFileName.str ( ).c_str ( ) , "wb" ) ;
			if ( fout )
			{
				for ( map < const char* , int , SConstCharComparator > :: iterator mapItr = mapDomain_EntryCount.begin ( ) ; mapItr != mapDomain_EntryCount.end ( ) ; mapItr ++ )
					fprintf ( fout , "%s\t%i\n" , mapItr->first , mapItr->second ) ;
				fclose ( fout ) ;
				objLogger.LogINFO ( "Domain analysis file:" , objOssDomainAnalysisFileName.str ( ).c_str ( ) , "created" ) ;
			}
			else
				objLogger.LogWARN ( "Domain analysis file:" , objOssDomainAnalysisFileName.str ( ).c_str ( ) , "creation failed" ) ;
		}
	}

	ColExtractor clxDictionary ;
	if ( clxDictionary.ExtractFrom ( m_strPermanentCacheFileName ) )
	{
		bRetVal = true ;

		COLUMNS colsDictionary ;
		int iEntityIDIndex				= 0 ;
		int iPrefixIndex				= 1 ;
		int iEntityNameIndex			= 2 ;
		int iIPSuffixMinIndex			= 3 ;
		int iIPSuffixMaxIndex			= 4 ;
		int iPageMaskIndex				= 5 ;
		int iSubChannelPrecedenceIndex	= 6 ;
		int iPrecedenceIndex			= 7 ;
		int iDatasetTypeIndex			= 8 ;
		int iPropertyNameIndex			= 9 ;
		int iAppsMaskIndex				= 10;
		map < string , CCFDBase* > :: iterator itrMapSearchResult ;
		CCFDBase* pCFDBase = NULL ;
		bool bIsNEDOM = false ;
		m_lRowsLoaded = 0 ;
		m_lNEDOMRecords = 0 ;

		int iPatternIDIndex				= 0 ;
		int iRequestURLIndex			= 1 ;
		int iReferURLIndex				= 2 ;
		int iAdNetPrecedenceIndex		= 3 ;

		int ( *ptrFun )( int ) = toupper;

		bool m_bRowLoaded = false ;

		//Pushing all Domain names to Upper case - Nilesh - Aug 23, 2011 - Moved this piece from loadDictionaryFromDatabase to here
		for ( vector < string > :: iterator vItr = m_vecKeepDomainList.begin ( ) ; vItr != m_vecKeepDomainList.end ( ) ; vItr ++ )
			transform ( vItr->begin ( ) , vItr->end ( ) , vItr->begin ( ) , ptrFun ) ;

		while ( clxDictionary.Extract ( colsDictionary ) )
		{
			m_bRowLoaded = false ;
			if ( ! m_bLoadAdNetDictionary )
			{
				string strEntityID = colsDictionary [ iEntityIDIndex ] ;
				string strPrefix = colsDictionary [ iPrefixIndex ] ;
				string strEntityName = colsDictionary [ iEntityNameIndex ] ;
				string strIPSuffixMin = colsDictionary [ iIPSuffixMinIndex ] ;
				string strIPSuffixMax = colsDictionary [ iIPSuffixMaxIndex ] ;
				string strPageMask = colsDictionary [ iPageMaskIndex ] ;
				string strSubChannelPrecedence = colsDictionary [ iSubChannelPrecedenceIndex ] ;
				string strPrecedence = colsDictionary [ iPrecedenceIndex ] ;
				string strDatasetType = colsDictionary [ iDatasetTypeIndex ] ;
				string strPropertyName = colsDictionary [ iPropertyNameIndex ] ;
				string strAppsMask = colsDictionary [ iAppsMaskIndex ] ;
				bIsNEDOM = false ;

				//if( strEntityName == string("SECURESTUDIES.COM-STREAM"))
				//{
				//	transform ( strPageMask.begin ( ) , strPageMask.end ( ) , strPageMask.begin ( ) , ptrFun ) ;
				//	int pos = strPageMask.find("C2=");
				//	if(pos != -1)
				//	{
				//		long pos2 = strPageMask.find("%",pos);
				//		if(pos2 != -1)
				//		{
				//			strEntityName = strEntityName + strPageMask.substr(pos+3,pos2-(pos+3));
				//			//printf("\n Modified entity name = %s",strEntityName.c_str());
				//		}
				//	}

				//}


				if(IsNumericIPAddress(strEntityName))
				{
					int iIPSuffixMin  = atoi(strIPSuffixMin.c_str());	
					int iIPSuffixMax  = atoi(strIPSuffixMax.c_str());	
			
					for(int ip = iIPSuffixMin; ip <= iIPSuffixMax; ip++)
					{
						char szTemp[10];
						sprintf(szTemp,".%d",ip);
						string strEntityNameInner ( strEntityName ) ;
						strEntityNameInner += szTemp;
						if
							(
							(
								( ! m_vecKeepDomainList.size ( ) ) ||
								( ( m_vecKeepDomainList.size ( ) ) && ( m_vecKeepDomainList.end ( ) != find ( m_vecKeepDomainList.begin ( ) , m_vecKeepDomainList.end ( ) , strEntityNameInner ) ) )
							) &&
							(
								( ! m_vecDiscDomainList.size ( ) ) ||
								( ( m_vecDiscDomainList.size ( ) ) && ( m_vecDiscDomainList.end ( ) == find ( m_vecDiscDomainList.begin ( ) , m_vecDiscDomainList.end ( ) , strEntityNameInner ) ) )
							)
							)
						{
							//Load appropriate dictionary object
							if ( m_mapDictionary.end ( ) != ( itrMapSearchResult = m_mapDictionary.find ( strEntityNameInner ) ) )
								pCFDBase = itrMapSearchResult->second ;
							else
							{
								if ( TIME_OPTIMIZED_MODE == m_iMode )
								{
									pCFDBase = new CCFDTree ( m_pDictionaryMemBuffer , m_bNEDOM , m_iAppsMask ) ;
									m_iCFDTreeCount ++ ;
								}
								else if ( SPACE_OPTIMIZED_MODE == m_iMode )
								{
									pCFDBase = new CCFDWildcard ( m_pDictionaryMemBuffer , m_bNEDOM , m_iAppsMask ) ;
									m_iCFDWildcardCount ++ ;
								}
								else if
								(
									( mapDomain_EntryCount.end ( ) != ( itrDomain_EntryCount = mapDomain_EntryCount.find ( strEntityName.c_str ( ) ) ) ) &&
									( itrDomain_EntryCount->second >= m_iMode )
								)
								//else if ( mapDomain_EntryCount [ strEntityName.c_str ( ) ] >= m_iMode )
								{
									pCFDBase = new CCFDTree ( m_pDictionaryMemBuffer , m_bNEDOM , m_iAppsMask ) ;
									m_iCFDTreeCount ++ ;
								}
								else
								{
									pCFDBase = new CCFDWildcard ( m_pDictionaryMemBuffer , m_bNEDOM , m_iAppsMask ) ;
									m_iCFDWildcardCount ++ ;
								}
								m_mapDictionary.insert ( make_pair ( strEntityNameInner , pCFDBase ) ) ;
							}

							//Nilesh - TODO - Add support for checking return value of below call and abort loading
							if
								(
									pCFDBase->LoadRow
													(
														strPrefix.c_str ( ) ,
														strEntityNameInner.c_str() ,
														strPageMask.c_str ( ) ,
														strPropertyName.c_str ( ) ,
														strAppsMask.c_str ( ) ,
														atol ( strEntityID.c_str ( ) ) ,
														atoi ( strPrecedence.c_str ( ) ) ,
														atoi ( strSubChannelPrecedence.c_str ( ) ) ,
														bIsNEDOM
													)
								)
								m_lRowsLoaded ++ ;

							m_bRowLoaded = true ;
						}

						if ( bIsNEDOM )
							m_lNEDOMRecords ++ ;
					}
				}
				else
				{
					if
						(
						(
							( ! m_vecKeepDomainList.size ( ) ) ||
							( ( m_vecKeepDomainList.size ( ) ) && ( m_vecKeepDomainList.end ( ) != find ( m_vecKeepDomainList.begin ( ) , m_vecKeepDomainList.end ( ) , strEntityName ) ) )
						) &&
						(
							( ! m_vecDiscDomainList.size ( ) ) ||
							( ( m_vecDiscDomainList.size ( ) ) && ( m_vecDiscDomainList.end ( ) == find ( m_vecDiscDomainList.begin ( ) , m_vecDiscDomainList.end ( ) , strEntityName ) ) )
						)
						)
					{
						//Load appropriate dictionary object
						if ( string :: npos == strEntityName.find ( '%' ) )
						{
							if ( m_mapDictionary.end ( ) != ( itrMapSearchResult = m_mapDictionary.find ( strEntityName ) ) )
								pCFDBase = itrMapSearchResult->second ;
							else
							{
								if ( TIME_OPTIMIZED_MODE == m_iMode )
								{
									pCFDBase = new CCFDTree ( m_pDictionaryMemBuffer , m_bNEDOM , m_iAppsMask ) ;
									m_iCFDTreeCount ++ ;
								}
								else if ( SPACE_OPTIMIZED_MODE == m_iMode )
								{
									pCFDBase = new CCFDWildcard ( m_pDictionaryMemBuffer , m_bNEDOM , m_iAppsMask ) ;
									m_iCFDWildcardCount ++ ;
								}
								else if
								(
									( mapDomain_EntryCount.end ( ) != ( itrDomain_EntryCount = mapDomain_EntryCount.find ( strEntityName.c_str ( ) ) ) ) &&
									( itrDomain_EntryCount->second >= m_iMode )
								)
								//else if ( mapDomain_EntryCount [ strEntityName.c_str ( ) ] >= m_iMode )
								{
									pCFDBase = new CCFDTree ( m_pDictionaryMemBuffer , m_bNEDOM , m_iAppsMask ) ;
									m_iCFDTreeCount ++ ;
								}
								else
								{
									pCFDBase = new CCFDWildcard ( m_pDictionaryMemBuffer , m_bNEDOM , m_iAppsMask ) ;
									m_iCFDWildcardCount ++ ;
								}
								m_mapDictionary.insert ( make_pair ( strEntityName , pCFDBase ) ) ;
							}
						}
						else
						{
							if ( m_mapWildcardDictionary.end ( ) != ( itrMapSearchResult = m_mapWildcardDictionary.find ( strEntityName ) ) )
								pCFDBase = itrMapSearchResult->second ;
							else
							{
								if ( TIME_OPTIMIZED_MODE == m_iMode )
								{
									pCFDBase = new CCFDTree ( m_pDictionaryMemBuffer , m_bNEDOM , m_iAppsMask ) ;
									m_iCFDTreeCount ++ ;
								}
								else if ( SPACE_OPTIMIZED_MODE == m_iMode )
								{
									pCFDBase = new CCFDWildcard ( m_pDictionaryMemBuffer , m_bNEDOM , m_iAppsMask ) ;
									m_iCFDWildcardCount ++ ;
								}
								else if
								(
									( mapDomain_EntryCount.end ( ) != ( itrDomain_EntryCount = mapDomain_EntryCount.find ( strEntityName.c_str ( ) ) ) ) &&
									( itrDomain_EntryCount->second >= m_iMode )
								)
								//else if ( mapDomain_EntryCount [ strEntityName.c_str ( ) ] >= m_iMode )
								{
									pCFDBase = new CCFDTree ( m_pDictionaryMemBuffer , m_bNEDOM , m_iAppsMask ) ;
									m_iCFDTreeCount ++ ;
								}
								else
								{
									pCFDBase = new CCFDWildcard ( m_pDictionaryMemBuffer , m_bNEDOM , m_iAppsMask ) ;
									m_iCFDWildcardCount ++ ;
								}
								m_mapWildcardDictionary.insert ( make_pair ( strEntityName , pCFDBase ) ) ;
							}
						}

						// trim space 
						int pos = strDatasetType.find(" ");

						while(pos != -1)
						{
							strDatasetType.erase(pos,1);
							pos = strDatasetType.find(" ");
						}

						// Santosh Added on 07/14/2011 to handle IM patterns vs Digapp issue
						if
						(
							( strDatasetType == string("W") ) &&
#ifdef _WIN32
							(strcmpi(strPageMask.c_str(),"/%3") == 0 ) &&
#else
							(strcasecmp(strPageMask.c_str(),"/%3") == 0 ) &&
#endif
							( strEntityName.find(".EXE") != string::npos)  &&
							( atoi(strPrecedence.c_str()) == 0 )
						)
						{
							//printf("\n %s",colsDictionary [ iPrecedenceIndex ].c_str());
							strPrecedence = "1"; 
							//printf("\n %s",colsDictionary [ iPrecedenceIndex ].c_str());
						}

						//Nilesh - TODO - Add support for checking return value of below call and abort loading
						if
							(
								pCFDBase->LoadRow
											(
												strPrefix.c_str ( ) ,
												strEntityName.c_str ( ) ,
												strPageMask.c_str ( ) ,
												strPropertyName.c_str ( ) ,
												strAppsMask.c_str ( ) ,
												atol ( strEntityID.c_str ( ) ) ,
												atoi ( strPrecedence.c_str ( ) ) ,
												atoi ( strSubChannelPrecedence.c_str ( ) ) ,
												bIsNEDOM
											) 
							)
							m_lRowsLoaded ++ ;
						
						m_bRowLoaded = true ;
					}

					if ( bIsNEDOM )
						m_lNEDOMRecords ++ ;
				}

				if
				(
					( m_bRowLoaded ) &&
					( 0 == ( m_lRowsLoaded % 500000 ) )
				)
					fprintf ( stdout , "Rows loaded: %i\n" , m_lRowsLoaded ) ;
			}
			else
			{
				//Add Ad Net loading logic in here

				string strPatternID = colsDictionary [ iPatternIDIndex ] ;
				string strRequestURL = colsDictionary [ iRequestURLIndex ] ;
				string strReferURL = colsDictionary [ iReferURLIndex ] ;
				string strPrecedence = colsDictionary [ iAdNetPrecedenceIndex ] ; 
				long lPrecedence = atoi ( strPrecedence.c_str ( ) ) ;
				size_t stLocation = -1 ;

				m_lRowsLoaded ++ ;

	if ( string :: npos != ( stLocation = strRequestURL.find ( '\r' ) ) )
		strRequestURL.erase ( stLocation , 1 ) ;
	if ( string :: npos != ( stLocation = strReferURL.find ( '\r' ) ) )
		strReferURL.erase ( stLocation , 1 ) ;

				transform( strRequestURL.begin(), strRequestURL.end(), strRequestURL.begin(), ptrFun );
				transform( strReferURL.begin(), strReferURL.end(), strReferURL.begin(), ptrFun );

				//if ( string :: npos == strRequestURL.find ( "DOUBLECLICK.NET" ) )
				//	continue ;

				if ( 0 == ( m_lRowsLoaded % 1000000 ) )
					fprintf ( stdout , "Ad Net Rows loaded: %i\n" , m_lRowsLoaded ) ;

				string strMapKey ;
				string strRequestDomain ;
				string strRequestHost ;
				string strRequestKey ;
				string strReferDomain ;
				string strReferHost ;
				string strReferKey ;
				//map < string , CCFDTree* > :: iterator mapItr = m_mapAdNet.end ( ) ;
				map < string , CCFDBase* > :: iterator mapItr = m_mapAdNet.end ( ) ;
				//CCFDTree* pTargetTree = NULL ;
				CCFDBase* pTargetTree = NULL ;

				if
				(
					( ! CCFDManager :: prepareKey ( strRequestURL , strRequestDomain , strRequestHost , strRequestKey ) ) ||
					( ! CCFDManager :: prepareKey ( strReferURL , strReferDomain , strReferHost , strReferKey ) )
				)
				{
					fprintf ( stderr , "Skipping: %s\t%s\t%s\n" , strPatternID.c_str ( ) , strRequestURL.c_str ( ) , strReferURL.c_str ( ) ) ;
					continue ;
				}

				m_setAdNetDomain.insert ( strRequestDomain ) ;
				//m_setAdNetDomain.insert ( strReferDomain ) ; //Nilesh - Aug 08, 2011 - This isn't required because dcnsubdomainsearch loads only Request domains in this collection

				if
					(
						!(
						(
							( ! m_vecKeepDomainList.size ( ) ) ||
							( ( m_vecKeepDomainList.size ( ) ) && ( m_vecKeepDomainList.end ( ) != find ( m_vecKeepDomainList.begin ( ) , m_vecKeepDomainList.end ( ) , strRequestDomain ) ) )
						) &&
						(
							( ! m_vecDiscDomainList.size ( ) ) ||
							( ( m_vecDiscDomainList.size ( ) ) && ( m_vecDiscDomainList.end ( ) == find ( m_vecDiscDomainList.begin ( ) , m_vecDiscDomainList.end ( ) , strRequestDomain ) ) )
						)
						)
					)
					continue ;

				strMapKey = strRequestDomain + "\t" + strReferDomain ;

				//if ( m_mapAdNet.end ( ) == ( mapItr = m_mapAdNet.find ( strMapKey ) ) )
				//{
				//	pTargetTree = new CCFDTree ( m_pDictionaryMemBuffer ) ;
				//	m_mapAdNet.insert ( make_pair ( strMapKey , pTargetTree ) ) ;
				//}
				//else
				//	pTargetTree = mapItr->second ;

				//pair < unsigned __int16 , unsigned __int16 > value_pair ;
				//CCFDTree :: VALUE* pValue = (CCFDTree :: VALUE*) m_pDictionaryMemBuffer->pAllocBuffer ( sizeof ( CCFDTree :: VALUE) , value_pair.first , value_pair.second ) ;

				//pValue->pattern_id = atoi ( strPatternID.c_str ( ) ) ;
				//pValue->prec = lPrecedence ;
				//pValue->subprec = 0 ;

				if ( strRequestKey [ strRequestKey.size ( ) - 1 ] != '%' )
					strRequestKey.append ( "%" ) ;
				if ( strReferKey [ strReferKey.size ( ) - 1 ] != '%' )
					strReferKey.append ( "%" ) ;

				//pTargetTree->insert ( ( strRequestKey + "\t" + strReferKey ).c_str ( ) , pValue , value_pair ) ;

				if ( m_mapAdNet.end ( ) == ( mapItr = m_mapAdNet.find ( strMapKey ) ) )
				{
					if ( TIME_OPTIMIZED_MODE == m_iMode )
					{
						pTargetTree = new CCFDTree ( m_pDictionaryMemBuffer ) ;
						m_iCFDTreeCount ++ ;
					}
					else if ( SPACE_OPTIMIZED_MODE == m_iMode )
					{
						pTargetTree = new CCFDWildcard ( m_pDictionaryMemBuffer ) ;
						m_iCFDWildcardCount ++ ;
					}
					else if
					(
						( mapDomain_EntryCount.end ( ) != ( itrDomain_EntryCount = mapDomain_EntryCount.find ( strMapKey.c_str ( ) ) ) ) &&
						( itrDomain_EntryCount->second >= m_iMode )
					)
					{
						pTargetTree = new CCFDTree ( m_pDictionaryMemBuffer ) ;
						m_iCFDTreeCount ++ ;
					}
					else
					{
						pTargetTree = new CCFDWildcard ( m_pDictionaryMemBuffer ) ;
						m_iCFDWildcardCount ++ ;
					}

					m_mapAdNet.insert ( make_pair ( strMapKey , pTargetTree ) ) ;
				}
				else
					pTargetTree = mapItr->second ;

				pTargetTree->LoadAdNetRow ( strRequestKey.c_str ( ) , strReferKey.c_str ( ) , atoi ( strPatternID.c_str ( ) ) , lPrecedence , (strRequestHost.size ( ) * 10000) + strReferHost.size ( ) ) ;
			}
		}

		fprintf ( stdout , "Rows loaded: %i\n" , m_lRowsLoaded ) ;

		if ( m_bLoadAdNetDictionary )
		{
			fprintf ( stdout , "Counting number of domain name entries having a %%\n" ) ;
			int iPercentEntryCount = 0 ;
			//vector < map < string , CCFDTree* > :: iterator > vecMapItrPercentLocation ;
			//for ( map < string , CCFDTree* > :: iterator mapItr = m_mapAdNet.begin ( ) ; mapItr != m_mapAdNet.end ( ) ; mapItr ++ )
			vector < map < string , CCFDBase* > :: iterator > vecMapItrPercentLocation ;
			for ( map < string , CCFDBase* > :: iterator mapItr = m_mapAdNet.begin ( ) ; mapItr != m_mapAdNet.end ( ) ; mapItr ++ )
			{
				if ( string :: npos != mapItr->first.find ( "%" ) )
				{
					iPercentEntryCount ++ ;
					vecMapItrPercentLocation.push_back ( mapItr ) ;
				}
			}
			fprintf ( stdout , "Found: [%i] entries having a %% in them\n" , iPercentEntryCount ) ;
			objLogger.LogINFO ( "Found:[" , m_iPercentEntryCount , "] entries having a % in them" ) ;

			//m_arrayCCFDTree = new pair < char* , CCFDTree* > [ iPercentEntryCount ] ;
			m_arrayCCFDTree = new pair < char* , CCFDBase* > [ iPercentEntryCount ] ;
			m_iPercentEntryCount = iPercentEntryCount ;
			int iLoop  = 0 ;
			//for ( vector < map < string , CCFDTree* > :: iterator > :: iterator vItr = vecMapItrPercentLocation.begin ( ) ; vItr != vecMapItrPercentLocation.end ( ) ; vItr ++ )
			for ( vector < map < string , CCFDBase* > :: iterator > :: iterator vItr = vecMapItrPercentLocation.begin ( ) ; vItr != vecMapItrPercentLocation.end ( ) ; vItr ++ )
			{
				m_arrayCCFDTree [ iLoop ].first = strdup ( (*vItr)->first.c_str ( ) ) ;
				m_arrayCCFDTree [ iLoop ].second = (*vItr)->second ;
				m_mapAdNet.erase ( (*vItr) ) ;
				iLoop ++ ;
			}

			sort ( m_arrayCCFDTree , m_arrayCCFDTree + m_iPercentEntryCount , AdNetPredicate ) ;
		}

		string strAdNetDomain ;
		for ( set < string > :: iterator sItr = m_setAdNetDomain.begin ( ) ; sItr != m_setAdNetDomain.end ( ) ; sItr ++ )
		{
			strAdNetDomain += *sItr ;
			strAdNetDomain += "," ;
		}

#ifdef _WIN32
		if ( m_bLoadAdNetDictionary )
			objLogger.LogINFO ( "AdNet domain count:" , m_setAdNetDomain.size ( ) ) ;
#endif
		m_pszAdNetDomainList = strdup ( strAdNetDomain.c_str ( ) ) ;
		ostringstream objOssDictionaryLoadSuccess ;

		objOssDictionaryLoadSuccess << "Succeeded." ;
		objOssDictionaryLoadSuccess << "Rows loaded: [" << m_lRowsLoaded << "] " ;
		objOssDictionaryLoadSuccess << "NEDOM rows: [" << m_lNEDOMRecords << "] " ;
		objOssDictionaryLoadSuccess << "Cache date: [" << m_lCacheDate << "]" ;
		objOssDictionaryLoadSuccess << "CCFDTree count: [" << m_iCFDTreeCount << "]" ;
		objOssDictionaryLoadSuccess << "CCFDWildcard count: [" << m_iCFDWildcardCount << "]" ;
		objLogger.LogINFO ( objOssDictionaryLoadSuccess.str ( ).c_str ( ) ) ;

		fprintf ( stderr , "Cleaning the Domain_EntryCount" ) ;
		int iClean = 0 ;
		for ( map < const char* , int , SConstCharComparator > :: iterator mapItr = mapDomain_EntryCount.begin ( ) ; mapItr != mapDomain_EntryCount.end ( ) ; mapItr ++ )
		{
			iClean ++ ;
			free ( (void*) mapItr->first ) ;
			if ( 0 == ( iClean % 500 ) )
				fprintf ( stderr , "Cleaned: [%i]\n" , iClean ) ;
		}
		fprintf ( stderr , "Cleaned: [%i]\n" , iClean ) ;
	}
	else
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Error: (" << CS_ERROR_FILE_NOT_FOUND << "): " ;
		objOssErrorMessage << "Unable to open Dictionary cache file: " ;
		objOssErrorMessage << "[" << m_strPermanentCacheFileName.c_str ( ) << "]" ;
		objOssErrorMessage << " for reading" ;
		objLogger.LogERROR ( "Failed." , objOssErrorMessage.str ( ).c_str ( ) ) ;

#if ( defined(_WIN32) ) || ( !defined(_WIN32) && defined (ENABLE_LINUX_DATABASE_LOAD) )
		Utils :: UnCheckBarrier ( hMutex ) ;
#endif 

		throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_FILE_NOT_FOUND ) ;
	}

#if ( defined(_WIN32) ) || ( !defined(_WIN32) && defined (ENABLE_LINUX_DATABASE_LOAD) )
	Utils :: UnCheckBarrier ( hMutex ) ;
#endif 
	return bRetVal ;
}

bool CCFDManager :: DsciWildCardSearch(const char *pszWildCardString, const char *pszOriginalString, char cMuitiCharWildCard, char cSingleCharWildCard)
{
	//NS :: Commented out logging code as logger is not present
	//CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;

	//--Start--Comparing upto first cMuitiCharWildCard.
	while ((*pszOriginalString) && (*pszWildCardString != cMuitiCharWildCard))
	{
		if ((tolower(*pszWildCardString) != tolower(*pszOriginalString)) && (*pszWildCardString != cSingleCharWildCard))
			return false;

		pszWildCardString++;
		pszOriginalString++;
	}
	//--End--

	const char *pNextToCurrentPositionInOriginalString	=	NULL;
	const char *pCurrentPositionInWildCardString		=	NULL;
	while(*pszOriginalString)
	{
		//Iterate all continous cMuitiCharWildCard.
		if(*pszWildCardString == cMuitiCharWildCard)
		{
			++pszWildCardString;
			//If no character after cMuitiCharWildCard then return true.
			if(!*pszWildCardString)
				return true;

			pCurrentPositionInWildCardString	=	pszWildCardString;
			pNextToCurrentPositionInOriginalString	=	pszOriginalString + 1;
		}
		//Comparing upto next cMuitiCharWildCard
		else if((tolower(*pszWildCardString) == tolower(*pszOriginalString)) || (*pszWildCardString == cSingleCharWildCard))
		{
			++pszWildCardString;
			++pszOriginalString;
		}
		//If all continous cMuitiCharWildCard iterated and after iterating one or more continous cMuitiCharWildCard
		//if next character of OriginalString is not as in WildCardString then this character is neglectde due to
		//previous cMuitiCharWildCard.
		else
		{
			pszWildCardString	=	pCurrentPositionInWildCardString;
			pszOriginalString	=	pNextToCurrentPositionInOriginalString++;
		}
	}

	//--Start--OriginalString completed so if WildCardString is also finished then return true else if
	//remaining WildCardString is having any character other then cMuitiCharWildCard character then return false.
	while(true)
	{
		if(*pszWildCardString == cMuitiCharWildCard)
		{
			++pszWildCardString;
			continue;
		}
		else if('\0' == *pszWildCardString)
			return true;
		else
			return false;
	}
	//--End--

	//Can't reach here. Unknown case so returning false.
	return false;
}

//NS :: Wildcard function defintion complete

#if ( defined(_WIN32) ) || ( !defined(_WIN32) && defined (ENABLE_LINUX_DATABASE_LOAD) )
void CCFDManager :: DisableNonEssentialDomainExclusion ( )
{
#ifdef _WIN32
	try
#endif
	{
		CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
		ostringstream objOssDomainExclusionListLoadingQuery ;

		objOssDomainExclusionListLoadingQuery << "SELECT Domain_Override FROM cs_domain_exclusion_list" ;

		fprintf ( stderr , "Now running domain exclusion list loading query: [%s]\n" , objOssDomainExclusionListLoadingQuery.str ( ).c_str ( ) ) ;

#ifdef _WIN32
		string strConnectionString =  "Driver={SQL Server};Server=" + m_pTermsInDomain->m_strMMXIDataSource + ";Database=AMW_CF1;Uid=seg_mmxi;Pwd=Segmmxi100";
		m_pTermsInDomain->OpenDBConn_Input ( strConnectionString ) ;
		_CommandPtr pCommand ;
		if ( SUCCEEDED ( pCommand.CreateInstance ( __uuidof ( Command ) ) ) )
		{
			pCommand->ActiveConnection = m_pTermsInDomain->m_dbConn_Input ;
			pCommand->CommandText = objOssDomainExclusionListLoadingQuery .str ( ).c_str ( ) ;
			pCommand->CommandTimeout = 36000 ;

			_RecordsetPtr pRecordSet ;
			if ( SUCCEEDED ( pRecordSet.CreateInstance ( __uuidof ( Recordset ) ) ) )
			{
				FieldsPtr pFields = pRecordSet->Fields ;

				if ( SUCCEEDED ( pRecordSet->Open ( (IDispatch*) pCommand , vtMissing , adOpenForwardOnly , adLockReadOnly , adCmdUnknown ) ) )
				{
					while ( ! pRecordSet->EndOfFile )
					{
						m_vecDiscDomainList.push_back ( m_pTermsInDomain->CheckString ( pRecordSet , "Domain_Override" ) ) ;
						pRecordSet->MoveNext ( ) ;
					}
				}
			}
		}
#else
	string strConnectionString =  "Driver=FreeTDS;Server=" + m_pTermsInDomain->m_strMMXIDataSource + ";Database=AMW_CF1;Uid=seg_mmxi;Pwd=Segmmxi100";
        SQLHENV hEnvironment ;
        SQLHDBC hDatabaseConnection ;
        SQLHSTMT hStatement ;
        SQLCHAR sqlcharReturnedConnectionString [ 1024 ] ;
        SQLSMALLINT sqlsmallintReturnedConnectionStringLength ;
        SQLRETURN retVal ;
		char szNonEssentialDomainName [ 128 ] = {0} ;

        SQLAllocHandle ( SQL_HANDLE_ENV , SQL_NULL_HANDLE , &hEnvironment ) ;
        SQLSetEnvAttr ( hEnvironment , SQL_ATTR_ODBC_VERSION , (void*) SQL_OV_ODBC3 , 0 ) ;
        SQLAllocHandle ( SQL_HANDLE_DBC , hEnvironment , &hDatabaseConnection ) ;
        retVal = SQLDriverConnect
                                (
                                        hDatabaseConnection ,
                                        NULL ,
                                        (SQLCHAR*) strConnectionString.c_str ( ) ,
                                        SQL_NTS ,
                                        sqlcharReturnedConnectionString ,
                                        sizeof ( sqlcharReturnedConnectionString ) ,
                                        &sqlsmallintReturnedConnectionStringLength ,
                                        SQL_DRIVER_COMPLETE
                                ) ;
        if ( SQL_SUCCEEDED ( retVal ) )
        {
            fprintf ( stdout , "Returned conection string: [%s]\n" , sqlcharReturnedConnectionString ) ;
            SQLLEN sqlintegerIndicator ;

            SQLAllocHandle ( SQL_HANDLE_STMT , hDatabaseConnection , &hStatement ) ;

            fprintf ( stderr , "Executing: [%s]\n" , objOssDomainExclusionListLoadingQuery.str ( ).c_str ( ) ) ;

            SQLAllocHandle ( SQL_HANDLE_STMT , hDatabaseConnection , &hStatement ) ;

            if ( ! SQL_SUCCEEDED ( SQLExecDirect ( hStatement , (SQLCHAR*) objOssDomainExclusionListLoadingQuery.str ( ).c_str ( ) , objOssDomainExclusionListLoadingQuery.str ( ).size ( )  ) ) )
            {
				ostringstream objOssErrorMessage ;

				objOssErrorMessage << "Error: (" << CS_ERROR_LOAD_FAILED << "): Error occurred: " ;
				objOssErrorMessage << "[" << extract_error ( "DisableNonEssentialDomainExclusion" , hStatement , SQL_HANDLE_STMT ) << "]" ;
				objOssErrorMessage << " occurred while loading domain exclusion list from database" ;
				throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_LOAD_FAILED ) ;
            }

            while ( SQL_SUCCEEDED ( retVal = SQLFetch ( hStatement ) ) )
            {
                    if ( SQL_SUCCEEDED ( SQLGetData ( hStatement , 1 , SQL_C_CHAR , szNonEssentialDomainName , 127 , (SQLLEN*) &sqlintegerIndicator ) ) )
						m_vecDiscDomainList.push_back ( szNonEssentialDomainName ) ;
                    else
                    {
                            //TODO - Nilesh add support for handling this failure
                    }

                    memset ( szNonEssentialDomainName , 0 , 128 ) ;
            }

            SQLFreeHandle ( SQL_HANDLE_STMT , hStatement ) ;
		}
		else
		{
			ostringstream objOssErrorMessage ;

			objOssErrorMessage << "Error: (" << CS_ERROR_LOAD_FAILED << "): Error: " ;
			objOssErrorMessage << "[" << extract_error ( "DisableNonEssentialDomainExclusion" , hDatabaseConnection , SQL_HANDLE_DBC ) << "]" ;
			objOssErrorMessage << " occurred while loading Non-Essential Domain exclusion list from database" ;
			throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_LOAD_FAILED ) ;
		}
#endif

		objLogger.LogINFO ( "Domains loaded from cs_domain_exclusion_list:[" , m_vecDiscDomainList.size ( ) , "]" ) ;
	}
#ifdef _WIN32
	catch(_com_error& error)
	{		
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Error: (" << CS_ERROR_LOAD_FAILED << "): COM Error: " ;
		objOssErrorMessage << "[" << error.Error ( ) << "]=[" << error.ErrorMessage ( ) << "]" ;
		objOssErrorMessage << " occurred while loading domain exclusion list from database" ;
		throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_LOAD_FAILED ) ;
	}
#endif
}
#endif

#if ( defined(_WIN32) ) || ( !defined(_WIN32) && defined (ENABLE_LINUX_DATABASE_LOAD) )
bool CCFDManager :: IsDictionaryInDBChanged ( )
{
	bool bRetVal = false ;
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
	string strDictionaryDate ;
	time_t objDictionaryTimestampDatabase = -1 ;

	if ( ! m_strGivenDictionaryName.size ( ) )
		throw CCSCFDException ( "IsDictionaryInDBChanged called without loading dictionary. Please call LoadDictionary first" , -1 ) ;

	objLogger.LogINFO ( "Checking \"Dictionary\" in database modifications for \"Title\": [" , m_strTableName.c_str ( ) , "]" ) ;
	string strGivenDictionaryName ( m_strTableName ) ;
#ifdef _WIN32
	try
#endif
		{
#ifdef _WIN32
			string conn_string =  "Driver={SQL Server};Server=" + m_pTermsInDomain->m_strMMXIDataSource + ";Database=AMW_CF1;Uid=seg_mmxi;Pwd=Segmmxi100";
			m_pTermsInDomain->OpenDBConn_Input(conn_string );
			
			string str_sql;	
			//str_sql = "SELECT LastTerm,	SecondLastTerm, TermCount, LastUpdatedAt FROM dbo.cs_terms_in_domain order by lastterm"; //where is_primary_c2_id = 1
			/*stringstream ssCommand;
				ssCommand << "SELECT Idx, Time_stamp, Dictionary, Stored_proc, Title "
			  << "FROM dbo.cs_dictionary_config "*/
			stringstream ssCommand;
			ssCommand << "SELECT Idx, crdate as Time_stamp, Dictionary, Stored_proc, Title FROM dbo.cs_dictionary_config "
					<< "join sysobjects on name = Dictionary "
					 << "WHERE Title = '" << m_strGivenDictionaryName << "'";

			str_sql = ssCommand.str().c_str();

			printf("Executing %s\n", str_sql.c_str() );
	
			string url;	
			_CommandPtr pCommand;
			HRESULT hr=pCommand.CreateInstance(__uuidof(Command));
			if(SUCCEEDED(hr))
			{
				pCommand->ActiveConnection = m_pTermsInDomain->m_dbConn_Input;
				
				pCommand->CommandText=str_sql.c_str();

				pCommand->CommandTimeout = 36000;

				_RecordsetPtr pRecordset;

				hr=pRecordset.CreateInstance(__uuidof(Recordset));
				if( SUCCEEDED(hr))
				{			
					FieldsPtr pFields=pRecordset->Fields;

					hr = pRecordset->Open((IDispatch *) pCommand,
						vtMissing, adOpenForwardOnly, adLockReadOnly, adCmdUnknown);			

					//Nilesh - May 11, 2011 - Added support for blanking this out so that case of invalid dictionary name can be handled
					m_strTableName = "" ;

					while( !pRecordset->EndOfFile )
					{								
						m_strTableName = m_pTermsInDomain->CheckString( pRecordset, "Dictionary" );							
						strDictionaryDate = m_pTermsInDomain->CheckString ( pRecordset , "Time_stamp" ) ;
						{
							struct tm objTm ;
							int iVal = -1 ;

							fprintf ( stderr , "[%s]\n" , strDictionaryDate.c_str ( ) ) ;

							memset ( &objTm , 0 , sizeof ( struct tm ) ) ;
							if 
								(
									(iVal = sscanf
									( strDictionaryDate.c_str ( ) , "%i/%i/%i %i:%i:%i" ,
										&(objTm.tm_mon) ,
										&(objTm.tm_mday) ,
										&(objTm.tm_year) ,
										&(objTm.tm_hour) ,
										&(objTm.tm_min) ,
										&(objTm.tm_sec) 
									)) >= 3 

								)
							{
								if( strDictionaryDate.find("PM") != -1)
									objTm.tm_hour+=12;

								struct tm objTmJan012000 = { 0 , 0 , 0 , 1 , 0 , 100 } ;
								time_t objTime ;

								objTm.tm_mon -- ;
								objTm.tm_year -= 1900 ;

								objTime = mktime ( &objTm ) ;

								time_t objTimeJan012000 = mktime ( &objTmJan012000 ) - 86400 ;
								objDictionaryTimestampDatabase = objTime - objTimeJan012000 ;
							}
							else
							{
								ostringstream objOssTimestampParsingError ;

								objOssTimestampParsingError << "Unable to Parse dictionary timestamp: [" << strDictionaryDate.c_str ( ) << "] read from database[" << iVal << "]" ;
								throw CCSCFDException ( objOssTimestampParsingError.str ( ) , CS_ERROR_OS_ERROR ) ;
							}
						}
						pRecordset->MoveNext();
					}
					ostringstream objOssDictionaryTimestampDatabase ;

					objOssDictionaryTimestampDatabase << "Dictionary date, just retrieved from Database: [" << objDictionaryTimestampDatabase<< "]" ;
					objLogger.LogINFO ( objOssDictionaryTimestampDatabase.str ( ).c_str ( ) ) ;
					fprintf ( stdout , "%s\n" , objOssDictionaryTimestampDatabase.str ( ).c_str ( ) ) ;
				}
			}
#else
	string conn_string =  "Driver=FreeTDS;Server=" + m_pTermsInDomain->m_strMMXIDataSource + ";Database=AMW_CF1;Uid=seg_mmxi;Pwd=Segmmxi100";
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
                    fprintf ( stdout , "Returned conection string: [%s]\n" , sqlcharReturnedConnectionString ) ;
                    ostringstream objOssTableNameDeterminationQuery ;
                    char szTableName [ 128 ] = {0} ;
                    SQLLEN sqlintegerIndicator ;

                    SQLAllocHandle ( SQL_HANDLE_STMT , hDatabaseConnection , &hStatement ) ;
                    objOssTableNameDeterminationQuery << "SELECT dictionary FROM dbo.cs_dictionary_config " ;
                    objOssTableNameDeterminationQuery << "WHERE title='" << m_strTableName.c_str ( ) << "'" ;

                    fprintf ( stderr , "Executing: [%s]\n" , objOssTableNameDeterminationQuery.str ( ).c_str ( ) ) ;

                    SQLAllocHandle ( SQL_HANDLE_STMT , hDatabaseConnection , &hStatement ) ;

                    if ( ! SQL_SUCCEEDED ( SQLExecDirect ( hStatement , (SQLCHAR*) objOssTableNameDeterminationQuery.str ( ).c_str ( ) , objOssTableNameDeterminationQuery.str ( ).size ( )  ) ) )
                    {
						ostringstream objOssErrorMessage ;

						objOssErrorMessage << "Error: (" << CS_ERROR_LOAD_FAILED << "): Error: " ;
						objOssErrorMessage << "[" << extract_error ( "loadConfiguration" , hStatement , SQL_HANDLE_STMT ) << "]" ;
						objOssErrorMessage << " occurred while loading configuration from database" ;
						throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_LOAD_FAILED ) ;
                    }

                    while ( SQL_SUCCEEDED ( retVal = SQLFetch ( hStatement ) ) )
                    {
                            memset ( szTableName , 0 , 128 ) ;

                            if ( SQL_SUCCEEDED ( SQLGetData ( hStatement , 1 , SQL_C_CHAR , szTableName , 127 , (SQLLEN*) &sqlintegerIndicator ) ) )
                                    ;
                            else
                            {
                                    //TODO - Nilesh add support for handling this failure
                            }
                    }

                    SQLFreeHandle ( SQL_HANDLE_STMT , hStatement ) ;

                    m_strTableName = szTableName ;
            }
			else
			{
				ostringstream objOssErrorMessage ;

				objOssErrorMessage << "Error: (" << CS_ERROR_LOAD_FAILED << "): Error: " ;
				objOssErrorMessage << "[" << extract_error ( "loadDictionaryFromDatabase" , hDatabaseConnection , SQL_HANDLE_DBC ) << "]" ;
				objOssErrorMessage << " occurred while loading Dictionary from database" ;
				throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_LOAD_FAILED ) ;
			}

            SQLFreeHandle ( SQL_HANDLE_DBC , hDatabaseConnection ) ;
            SQLFreeHandle ( SQL_HANDLE_ENV , hEnvironment ) ;
#endif
			if ( ! m_strTableName.size ( ) )
			{
				ostringstream objOssErrorMessage ;

				objOssErrorMessage << "Error: (" << CS_ERROR_NO_DATA << "): Error: " ;
				objOssErrorMessage << "Unable to retrieve Title for Dictionary name: [" << strGivenDictionaryName.c_str ( ) << "]" ;
				objOssErrorMessage << " occurred while retrieving Dictionary title from database" ;
				throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_NO_DATA ) ;
			}

			ostringstream objOssCurrentTime ;

			objLogger.LogINFO ( "Dictionary date, Retrieved earlier: [" , m_lDictionaryDate , "]" ) ;
			if ( m_lDictionaryDate != objDictionaryTimestampDatabase )
			{
				bRetVal = true ;
				objLogger.LogINFO ( "Dictionary database has changed" ) ;
			}
			else
				objLogger.LogINFO ( "Dictionary database hasn't changed" ) ;
		}
#ifdef _WIN32
		catch(_com_error& error)
		{		
			ostringstream objOssErrorMessage ;

			objOssErrorMessage << "Error: (" << CS_ERROR_LOAD_FAILED << "): COM Error: " ;
			objOssErrorMessage << "[" << error.Error ( ) << "]=[" << error.ErrorMessage ( ) << "]" ;
			objOssErrorMessage << " occurred while loading Terms In Domain from database" ;
			throw CCSCFDException ( objOssErrorMessage.str ( ).c_str ( ) , CS_ERROR_LOAD_FAILED ) ;
		}
		m_pTermsInDomain->m_dbConn_Input->Close();
#endif
	return bRetVal ;
}
#endif

const char* CCFDManager :: getDomainList ( )
{
	return m_pszAdNetDomainList ;
}

const char* CCFDManager :: getDomainList ( set < string >& setAdNetDomain )
{
	setAdNetDomain = m_setAdNetDomain ;
	return NULL ;
}
