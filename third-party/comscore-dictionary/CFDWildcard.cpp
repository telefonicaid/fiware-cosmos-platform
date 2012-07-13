#include "stdafx.h"
#include "CFDWildcard.h"

extern bool myPredicate ( const SPrefixPageMaskPatternID* pKey1 , const SPrefixPageMaskPatternID* pKey2 ) ;
extern bool DsciWildCardSearch(const char *pszWildCardString, const char *pszOriginalString, char cMuitiCharWildCard, char cSingleCharWildCard);

CCFDWildcard::CCFDWildcard( C_MemBuffer* pObjMemBuffer , bool bNEDOM , int iAppsMask )
{
	m_bDataSortOccurred = false ;
	m_iSizeArrayPattern = -1 ;

	m_pObjMemBuffer = pObjMemBuffer ;
	m_bNEDOM = bNEDOM ;
	m_iAppsMask = iAppsMask ;
}


CCFDWildcard::~CCFDWildcard(void)
{
	//for ( int i = 0 ; i < m_iSizeArrayPattern ; i ++ )
	//	delete m_arrayPattern [ i ] ;
	delete[] m_arrayPattern ;
}

bool CCFDWildcard :: LoadRow
					(
						const char* pszPrefix ,
						const char* pszEntityName ,
						const char* pszPageMask ,
						const char* pszPropertyName ,
						const char* pszAppsMask ,
						const long& lPatternID ,
						const int& iPrecedence ,
						const int& iSubChannelPrecedence ,
						bool& bIsNEDOM
					)
{
	int ( *ptrFun ) ( int ) = toupper ;
	bool bRetVal = false ;
	int iPrefixLength = strlen ( pszPrefix ) ;
	int iPageMaskLength = strlen ( pszPageMask ) ;

	bIsNEDOM = false ;

	//if ( string :: npos != ( stLocation = strWildcard.find ( "%%" ) ) )
	//	strWildcard.erase ( stLocation , 1 ) ;

	SPrefixPageMaskPatternID* pObjPrefixPageMaskPatternID = 
		(SPrefixPageMaskPatternID*) m_pObjMemBuffer->pAllocBuffer ( sizeof(SPrefixPageMaskPatternID) ) ;

	pObjPrefixPageMaskPatternID->m_pszPrefix = (char*) m_pObjMemBuffer->pAllocBuffer ( sizeof(char) * ( iPrefixLength + 1 ) ) ;
	//memset ( pObjPrefixPageMaskPatternID->m_pszPrefix , 0 , (iPrefixLength + 1) ) ;
	strcpy ( pObjPrefixPageMaskPatternID->m_pszPrefix , pszPrefix ) ;
	transform
		(
			pObjPrefixPageMaskPatternID->m_pszPrefix ,
			pObjPrefixPageMaskPatternID->m_pszPrefix + iPrefixLength ,
			pObjPrefixPageMaskPatternID->m_pszPrefix ,
			ptrFun
		) ;

	pObjPrefixPageMaskPatternID->m_pszPageMask = (char*) m_pObjMemBuffer->pAllocBuffer ( sizeof(char) * ( iPageMaskLength + 1 ) ) ;
	//memset ( pObjPrefixPageMaskPatternID->m_pszPageMask , 0 , ( iPageMaskLength + 1 ) ) ;
	strcpy ( pObjPrefixPageMaskPatternID->m_pszPageMask , pszPageMask ) ;
	transform
		(
			pObjPrefixPageMaskPatternID->m_pszPageMask ,
			pObjPrefixPageMaskPatternID->m_pszPageMask + iPrefixLength ,
			pObjPrefixPageMaskPatternID->m_pszPageMask ,
			ptrFun
		) ;

	pObjPrefixPageMaskPatternID->m_iPatternID = lPatternID ;

	pObjPrefixPageMaskPatternID->m_iPrecedencePlusSubPrecedence = ( iSubChannelPrecedence * 10000 )  + iPrecedence ;

	//Using App Mask for identifying NEDOMs - As informed by Jason
	if ( atol ( pszAppsMask ) & m_iAppsMask )
	{
		pObjPrefixPageMaskPatternID->m_iPatternID *= -1 ;
		bIsNEDOM = true ;

		// Load if user didn't disable NEDOMS
		if( m_bNEDOM == true )
		{
			m_listData.push_back ( pObjPrefixPageMaskPatternID ) ;
			bRetVal = true ;
		}
	}
	else
	{
		m_listData.push_back ( pObjPrefixPageMaskPatternID ) ;
		bRetVal = true ;
	}

	return bRetVal ;
}

long CCFDWildcard :: Apply ( const char* pszHost , const char* pszDomain , const char* pszQueryString , int& iWeight )
{
	if ( ! m_bDataSortOccurred )
	{
		int i = 0 ;
		m_iSizeArrayPattern = m_listData.size ( ) ;
		m_arrayPattern = new SPrefixPageMaskPatternID* [ m_iSizeArrayPattern ] ;

		for ( list < SPrefixPageMaskPatternID* > :: iterator lItr = m_listData.begin ( ) ; lItr != m_listData.end ( ) ; lItr ++ )
			m_arrayPattern [ i ++ ] = *lItr ;

		sort ( m_arrayPattern , m_arrayPattern + m_iSizeArrayPattern , myPredicate ) ;
		m_bDataSortOccurred = true ;
	}

	//TODO - Nilesh - Discuss with Santosh if frequent use of C_MemBuffer is Ok or not
	char* pszTempHost = strdup ( pszHost ) ;
	char* pszTempDomain = strdup ( pszDomain ) ;
	char* pszTempQueryString = strdup ( pszQueryString ) ;
	char* pIndexOfDot = NULL ;
	char* pHostToken [ 32 ] = {0} ;
	int iHostTokenCount = 0 ;
	char* pDomainToken [ 32 ] = {0} ;
	int iDomainTokenCount = 0 ;
	long lPatternID = 1 ;

	for ( unsigned int iLoop = 0 ; iLoop < strlen ( pszHost ) ; iLoop ++ )
		pszTempHost [ iLoop ] = toupper ( pszHost [ iLoop ] ) ;

	for ( unsigned int iLoop = 0 ; iLoop < strlen ( pszQueryString ) ; iLoop ++ )
		pszTempQueryString [ iLoop ] = toupper ( pszQueryString [ iLoop ] ) ;

	//Tokenize Host
	{
		pHostToken [ iHostTokenCount ++ ] = strtok ( pszTempHost , "." ) ;
		while ( ( pIndexOfDot = strtok ( NULL , "." ) ) )
		{
			pHostToken [ iHostTokenCount ++ ] = pIndexOfDot ;
		}
	}
	//Tokenize Domain
	{
		pDomainToken [ iDomainTokenCount ++ ] = strtok ( pszTempDomain , "." ) ;
		while ( ( pIndexOfDot = strtok ( NULL , "." ) ) )
		{
			pDomainToken [ iDomainTokenCount ++ ] = pIndexOfDot ;
		}
	}

	//Prepare Prefix
	string strPrefix ;
	int iLoop = 0 ;

	if ( 0 == strcmp ( pszHost , pszDomain ) ) //Case where host and domain are exactly same Eg, aim.exe, abc.exe, ya.ru etc
		strPrefix = "" ;
	else
	{
		int iLoopEnd = 1 ;

		if ( iHostTokenCount > 2 )
			iLoopEnd = iHostTokenCount - iDomainTokenCount - 1 ;
		else if ( 1 == iHostTokenCount )	//TODO - Nilesh - Gather cases where flow reaches this point
			iLoopEnd = 0 ;
		for ( ; iLoop < iLoopEnd ; iLoop ++ )
		{
			strPrefix += pHostToken [ iLoop ] ;
			strPrefix += "." ;
		}
		strPrefix += pHostToken [ iLoop ] ;
	}

	for ( iLoop = 0 ; iLoop < m_iSizeArrayPattern ; iLoop ++ )
	{
		if
			(
			( DsciWildCardSearch ( m_arrayPattern [ iLoop ]->m_pszPrefix , strPrefix.c_str ( ) , '%' , '\t' ) ) &&
			( DsciWildCardSearch ( m_arrayPattern [ iLoop ]->m_pszPageMask , pszTempQueryString , '%' , '\t' ) )
			)
		{
			lPatternID = m_arrayPattern [ iLoop ]->m_iPatternID ;
			iWeight = m_arrayPattern [ iLoop ]->m_iPrecedencePlusSubPrecedence ;
			break ;
		}
	}

	free ( pszTempHost ) ;
	free ( pszTempDomain ) ;
	free ( pszTempQueryString ) ;
	return lPatternID ;
}

bool CCFDWildcard :: LoadAdNetRow
		(
			const char* pszRequestURLWildcard ,
			const char* pszReferURLWildcard ,
			const long& lPatternID ,
			const int& iPrecedence ,
			const int& iSubchannelPrecedence
		)
{
	SPrefixPageMaskPatternID* pObjPrefixPageMaskPatternID = 
		(SPrefixPageMaskPatternID*) m_pObjMemBuffer->pAllocBuffer ( sizeof(SPrefixPageMaskPatternID) ) ;
	int iRequestWildcardLength = strlen ( pszRequestURLWildcard ) ;
	int iReferWildcardLength = strlen ( pszReferURLWildcard ) ;

	pObjPrefixPageMaskPatternID->m_pszPrefix = (char*) m_pObjMemBuffer->pAllocBuffer ( iRequestWildcardLength + 1 ) ;
	strcpy ( pObjPrefixPageMaskPatternID->m_pszPrefix , pszRequestURLWildcard ) ;
	pObjPrefixPageMaskPatternID->m_pszPrefix [ iRequestWildcardLength ] = '\0' ;

	pObjPrefixPageMaskPatternID->m_pszPageMask = (char*) m_pObjMemBuffer->pAllocBuffer ( iReferWildcardLength + 1 ) ;
	strcpy ( pObjPrefixPageMaskPatternID->m_pszPageMask , pszReferURLWildcard ) ;
	pObjPrefixPageMaskPatternID->m_pszPageMask [ iReferWildcardLength ] = '\0' ;

	pObjPrefixPageMaskPatternID->m_iPatternID = lPatternID ;

	pObjPrefixPageMaskPatternID->m_iPrecedencePlusSubPrecedence = iPrecedence ;

	m_listData.push_back ( pObjPrefixPageMaskPatternID ) ;

	return true ;
}

long CCFDWildcard :: ApplyAdNet ( const char* pszRequest , const char* pszRefer )
{
	long lRetVal = 1 ;

	for ( list < SPrefixPageMaskPatternID* > :: iterator lItr = m_listData.begin ( ) ; lItr != m_listData.end ( ) ; lItr ++ )
	{
		if
			(
			( DsciWildCardSearch ( (*lItr)->m_pszPrefix , pszRequest , '%' , '\n' ) ) &&
			( DsciWildCardSearch ( (*lItr)->m_pszPageMask , pszRefer , '%' , '\n' ) )
			)
		{
			lRetVal = (*lItr)->m_iPatternID ;
		}
	}

	return lRetVal ;
}

