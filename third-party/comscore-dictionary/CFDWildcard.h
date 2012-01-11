#pragma once
#include <list>
#include <algorithm>
#include <string>
#include "CFDBase.h"
#include "c_membuffer.h"

using namespace std ;

struct SPrefixPageMaskPatternID
{
	char*	m_pszPrefix ;
	char*	m_pszPageMask ;
	int		m_iPatternID ;
	int		m_iPrecedencePlusSubPrecedence ;
	bool	m_bNEDOM ;

	SPrefixPageMaskPatternID ( const char* pszPrefix , const char* pszPageMask , int iPatternID , int iPrecedence , int iSubchannelPrecedence )
	{
		m_pszPrefix = strdup ( pszPrefix ) ;
		m_pszPageMask = strdup ( pszPageMask ) ;
		m_iPatternID = iPatternID ;
		m_iPrecedencePlusSubPrecedence = ( iSubchannelPrecedence * 10000 ) + iPrecedence ;
		m_bNEDOM = false ;
	}

	~SPrefixPageMaskPatternID ( )
	{
		free ( m_pszPrefix ) ;
		free ( m_pszPageMask ) ;
	}
} ;

static bool myPredicate ( const SPrefixPageMaskPatternID* pKey1 , const SPrefixPageMaskPatternID* pKey2 )
{
	char* pIndexOfPercentPrefix1 = strchr ( pKey1->m_pszPrefix , '%' ) ;
	char* pIndexOfPercentPrefix2 = strchr ( pKey2->m_pszPrefix , '%' ) ;
	char* pIndexOfPercentPageMask1 = strchr ( pKey1->m_pszPageMask , '%' ) ;
	char* pIndexOfPercentPageMask2 = strchr ( pKey2->m_pszPageMask , '%' ) ;

	if ( pKey1->m_iPrecedencePlusSubPrecedence > pKey2->m_iPrecedencePlusSubPrecedence )
		return true ;
	else if ( pKey1->m_iPrecedencePlusSubPrecedence < pKey2->m_iPrecedencePlusSubPrecedence )
		return false ;
	else if ( 0 == strcmp ( pKey1->m_pszPrefix , pKey2->m_pszPrefix ) ) //Reaching upto this else clause or the next one ensures that Precedence of both the entries are exactly the same
	{
		if ( (pIndexOfPercentPageMask1) && (!pIndexOfPercentPageMask2) )
			return false ;
		else if ( (!pIndexOfPercentPageMask1) && (pIndexOfPercentPageMask2) )
			return true ;
		else if ( (pIndexOfPercentPageMask1) && (pIndexOfPercentPageMask2) )
		{
			int iPageMask1Len = strlen ( pKey1->m_pszPageMask ) ;
			int iPageMask2Len = strlen ( pKey2->m_pszPageMask ) ;

			if ( iPageMask1Len <= iPageMask2Len )
				return false ;
			else if ( iPageMask1Len > iPageMask2Len )
				return true ;
		}
		else 
		{
			if ( 0 > strcmp ( pKey1->m_pszPageMask , pKey2->m_pszPageMask ) )
				return true ;
			else
				return false ;
		}
	}
	else
	{
		if ( (pIndexOfPercentPrefix1) && (!pIndexOfPercentPrefix2) )
			return false ;
		else if ( (!pIndexOfPercentPrefix1) && (pIndexOfPercentPrefix2) )
			return true ;
		else if ( (pIndexOfPercentPrefix1) && (pIndexOfPercentPrefix2) )
		{
			int iPrefixLen1 = strlen ( pKey1->m_pszPrefix ) ;
			int iPrefixLen2 = strlen ( pKey2->m_pszPrefix ) ;

			if ( iPrefixLen1 < iPrefixLen2 )
				return false ;
			else if ( iPrefixLen1 >= iPrefixLen2 )
				return true ;
		}
		else if ( 0 > strcmp ( pKey1->m_pszPrefix , pKey2->m_pszPrefix ) )
			return true ;
	}

	return false ;
}

static bool DsciWildCardSearch(const char *pszWildCardString, const char *pszOriginalString, char cMuitiCharWildCard, char cSingleCharWildCard)
{
	//--Start--Comparing upto first cMuitiCharWildCard.
	while ((*pszOriginalString) && (*pszWildCardString != cMuitiCharWildCard))
	{
		if ((/*tolower*/(*pszWildCardString) != /*tolower*/(*pszOriginalString)) && (*pszWildCardString != cSingleCharWildCard))
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
		else if((/*tolower*/(*pszWildCardString) == /*tolower*/(*pszOriginalString)) || (*pszWildCardString == cSingleCharWildCard))
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

class CCFDWildcard :
	public CCFDBase
{
public:
	CCFDWildcard(C_MemBuffer* pBuffer , bool bNEDOM = true , int iAppsMask = 1);
	virtual ~CCFDWildcard(void);

	bool LoadRow
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
		) ;
	long Apply ( const char* pszPrefix , const char* pszDomain , const char* pszQueryString , int& iWeight ) ;

	bool LoadAdNetRow
		(
			const char* pszRequestURLWildcard ,
			const char* pszReferURLWildcard ,
			const long& lPatternID ,
			const int& iPrecedence ,
			const int& iSubchannelPrecedence
		) ;
	long ApplyAdNet ( const char* pszRequest , const char* pszRefer ) ;

private:
	bool								m_bDataSortOccurred ;
	list <SPrefixPageMaskPatternID* >	m_listData ;

	int									m_iSizeArrayPattern ;
	SPrefixPageMaskPatternID**			m_arrayPattern ;

	C_MemBuffer*						m_pObjMemBuffer ;
	bool								m_bNEDOM ;
	int									m_iAppsMask ;
};

