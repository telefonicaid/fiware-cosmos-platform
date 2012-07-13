#pragma once

#include <string>
#include <sstream>
#include <map>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#include <time.h>
#else
#if defined(ENABLE_LINUX_DATABASE_LOAD)
#include <sql.h>
#include <sqlext.h>
#include <uuid/uuid.h>
#endif
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#endif
using namespace std ;

#include "TermsInDomain.h"
#include "CFDBase.h"
#include "CFDTree.h"
#include "c_membuffer.h"
#include "column.h"
#include "Utils.h"
#include "CSCFDException.h"
#include "ErrorCodes.h"
#include "Logger.h"

#include "CFDWildcard.h"

#define TIME_OPTIMIZED_MODE		1
#define SPACE_OPTIMIZED_MODE	2

#define NS_OPTIMIZATION //Nilesh - Sep 12, 2011 - Made wildcard dictionary optimization controllable via #define

struct SConstCharComparator
{
	bool operator() ( const char* pszKey1 , const char* pszKey2 ) 
	{
		return 0 > strcmp ( pszKey1 , pszKey2 ) ;
	}
};

//static bool AdNetPredicate ( const pair < char* , CCFDTree* >& objKey1 , const pair < char* , CCFDTree* >& objKey2 )
static bool AdNetPredicate ( const pair < char* , CCFDBase* >& objKey1 , const pair < char* , CCFDBase* >& objKey2 )
{
	int iLen1 = strlen ( objKey1.first ) ;
	int iLen2 = strlen ( objKey2.first ) ;

	if ( iLen1 < iLen2 )
		return false ;
	else if ( iLen1 > iLen2 )
		return true ;
	else
	{
		int iStringComparisonResult = strcmp ( objKey1.first , objKey2.first ) ;

		if ( 0 > iStringComparisonResult )
			return false ;
		else
			return true ;
	}
}

class CCFDManager
{
public:
	CCFDManager(CTermsInDomain* pTermsInDomain, int iMode);
	~CCFDManager(void);

	void SetCachePath ( const char* pszCachePath ) ;
	bool IsNumericIPAddress(string sHost);
	void LoadDictionary ( const char* pszDictionaryName ) ;

	void LoadAdNetDictionary ( const char* pszConfiguration ) ;

	long ApplyDictionary ( string sHost , string sQueryString , int& iWeight );
	long ApplyDictionary ( string sHost , string sDomain , string sQueryString , int& iWeight );

	long ApplyAdNetDictionary ( string strRequestURL , string strReferURL ) ;

	string GetDomain ( const char* pszHostName ) ;
	void ParseURL ( const char* pszURL , string& strDomain , string& strHost , string& strDir , string& strPage , string& strQueryString ) ;
	string GetMD5Hash ( const char* pszInputString ) ;
	long long GetDomainID ( const char* pszInputString , string& strDomainID ) ;

	//Nilesh - Added these functions after discussion with Santosh regarding missing functions BEGIN
	//Apr 15, 2010 1245 hrs
	void GetStats ( long& lRecordsLoaded , long& lCacheDate , long& lNEDOMRecords ) ;
	void GetDictionaryList ( string& strDictionaryList ) ;
	//Nilesh - Added these functions after discussion with Santosh regarding missing functions END

	void DisableNEDOM ( ) ;
	void SetAppsMask ( int iAppsMask ) ;

	bool LoadFilterHostList ( const char* pszFilterHostList ) ;
	bool LoadFilterHostFile ( const char* pszFilterHostFile ) ;

#if ( defined(_WIN32) ) || ( !defined(_WIN32) && defined (ENABLE_LINUX_DATABASE_LOAD) )
	void DisableNonEssentialDomainExclusion ( ) ;
	bool IsDictionaryInDBChanged ( ) ;
#endif
private:
	CTermsInDomain*	m_pTermsInDomain ;
	int	m_lRowsLoaded ;
	long	m_lCacheDate ;
	long	m_lNEDOMRecords ;
	string	m_strTableName ;
	map < string , CCFDBase* >	m_mapDictionary ;
	map < string , CCFDBase* >	m_mapWildcardDictionary ;
#ifdef NS_OPTIMIZATION
	const char**	m_arrayWildcardDomainName ;
	CCFDBase** m_arrayCCFDBase ;
	int m_iWildcardArraySize ;
#endif
	C_MemBuffer*	m_pDictionaryMemBuffer ;
	string	m_strPermanentCacheFileName ;
	string	m_strTemporaryCacheFileName ;
	
	bool	m_bNEDOM ;	//This would get logically ANDed
	int		m_iAppsMask ;
	vector < const char* >	m_vecHostList ;
	vector < string >	m_vecKeepDomainList ;
	vector < string >	m_vecDiscDomainList ;
	string	m_strCachePath ;

	int		m_iMode ;

	int		m_iCFDWildcardCount ;
	int		m_iCFDTreeCount ;

	long	m_lDictionaryDate ;
	bool	m_bDictionaryNameFlatFile ;

	CCFDTree*	m_pAdNetTree ;
	//Key = Request Domain + TAB + Refer Domain
	//Value = A CCFDTree in which (Request URL Pattern - Domain) + TAB + (Reqfer URL Pattern - Domain) has been stored
	//map < string , CCFDTree* >	m_mapAdNet ;
	map < string , CCFDBase* >	m_mapAdNet ;
	int							m_iPercentEntryCount ;
	//pair < char* , CCFDTree* >*	m_arrayCCFDTree ;
	pair < char* , CCFDBase* >*	m_arrayCCFDTree ;

	string	m_strGivenDictionaryName ; //Nilesh - June 27, 2011 - Support for storing supplied dictionary name so that IsDictionaryInDBChanged can work properly

	bool	m_bLoadAdNetDictionary ;
	set < string > m_setAdNetDomain ;
	char*	m_pszAdNetDomainList ;

#if ( defined(_WIN32) ) || ( !defined(_WIN32) && defined (ENABLE_LINUX_DATABASE_LOAD) )
	void loadConfiguration ( ) ;
	void loadDictionaryFromDatabase ( ) ;
#endif
	bool loadDictionaryFromCache ( ) ;
	bool prepareKey ( const string& strURL , string& strDomainName , string& strHostName , string& strKey ) ;
	bool DsciWildCardSearch(const char *pszWildCardString, const char *pszOriginalString, char cMuitiCharWildCard, char cSingleCharWildCard);
public:
	const char* getDomainList ( ) ;
	const char* getDomainList ( set < string >& setDomainList ) ;
};

